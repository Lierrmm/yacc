#include <std_include.hpp>
#include <utils/string.hpp>

#include "loader.hpp"
#include "binary_loader.hpp"
#include <utils/hook.hpp>

loader::loader(const launcher::mode mode) : mode_(mode)
{
}

FARPROC loader::load(const utils::nt::library& module, const std::string& buffer) const
{
	//const auto buffer = binary_loader::load(this->mode_);
	if (buffer.empty()) return nullptr;

	const utils::nt::library source(HMODULE(buffer.data()));
	if (!source) return nullptr;

	this->load_sections(module, source);
	this->load_imports(module, source);
	this->load_tls(module, source);
	

	DWORD oldProtect;
	VirtualProtect(module.get_nt_headers(), 0x1000, PAGE_EXECUTE_READWRITE, &oldProtect);

	module.get_optional_header()->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT] = source
	                                                                            .get_optional_header()->DataDirectory[
		IMAGE_DIRECTORY_ENTRY_IMPORT];
	std::memmove(module.get_nt_headers(), source.get_nt_headers(),
	             sizeof(IMAGE_NT_HEADERS) + source.get_nt_headers()->FileHeader.NumberOfSections * sizeof(
		             IMAGE_SECTION_HEADER));

	return FARPROC(module.get_ptr() + source.get_relative_entry_point());
}

void loader::set_import_resolver(const std::function<void*(const std::string&, const std::string&)>& resolver)
{
	this->import_resolver_ = resolver;
}

void loader::load_section(const utils::nt::library& target, const utils::nt::library& source,
                          IMAGE_SECTION_HEADER* section)
{
	void* target_ptr = target.get_ptr() + section->VirtualAddress;
	const void* source_ptr = source.get_ptr() + section->PointerToRawData;

	if (PBYTE(target_ptr) >= (target.get_ptr() + BINARY_PAYLOAD_SIZE))
	{
		throw std::runtime_error(utils::string::va("Section exceeds the binary payload size, please increase it! %p | %p", PBYTE(target_ptr), (target.get_ptr() + BINARY_PAYLOAD_SIZE)));
	}

	if (section->SizeOfRawData > 0)
	{
		const auto size_of_data = std::min(section->SizeOfRawData, section->Misc.VirtualSize);
		std::memmove(target_ptr, source_ptr, size_of_data);

		DWORD old_protect;
		VirtualProtect(target_ptr, size_of_data, PAGE_EXECUTE_READWRITE, &old_protect);
	}
}

void loader::load_sections(const utils::nt::library& target, const utils::nt::library& source) const
{
	for (auto& section : source.get_section_headers())
	{
		this->load_section(target, source, section);
	}
}

void loader::load_imports(const utils::nt::library& target, const utils::nt::library& source) const
{
	const auto import_directory = &source.get_optional_header()->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

	auto* descriptor = PIMAGE_IMPORT_DESCRIPTOR(target.get_ptr() + import_directory->VirtualAddress);

	while (descriptor->Name)
	{
		std::string name = LPSTR(target.get_ptr() + descriptor->Name);

		auto* name_table_entry = reinterpret_cast<uintptr_t*>(target.get_ptr() + descriptor->OriginalFirstThunk);
		auto* address_table_entry = reinterpret_cast<uintptr_t*>(target.get_ptr() + descriptor->FirstThunk);

		if (!descriptor->OriginalFirstThunk)
		{
			name_table_entry = reinterpret_cast<uintptr_t*>(target.get_ptr() + descriptor->FirstThunk);
		}

		while (*name_table_entry)
		{
			FARPROC function = nullptr;
			std::string function_name;

			// is this an ordinal-only import?
			if (IMAGE_SNAP_BY_ORDINAL(*name_table_entry))
			{
				auto module = utils::nt::library::load(name);
				if (module)
				{
					function = GetProcAddress(module, MAKEINTRESOURCEA(IMAGE_ORDINAL(*name_table_entry)));
				}

				function_name = "#" + std::to_string(IMAGE_ORDINAL(*name_table_entry));
			}
			else
			{
				auto import = PIMAGE_IMPORT_BY_NAME(target.get_ptr() + *name_table_entry);
				function_name = import->Name;

				if (this->import_resolver_) function = FARPROC(this->import_resolver_(name, function_name));
				if (!function)
				{
					auto module = utils::nt::library::load(name);
					if (module)
					{
						function = GetProcAddress(module, function_name.data());
					}
				}
			}

			if (!function)
			{
				throw std::runtime_error(utils::string::va("Unable to load import '%s' from module '%s'",
				                                           function_name.data(), name.data()));
			}

			//*address_table_entry = reinterpret_cast<uintptr_t>(function);
			utils::hook::set(address_table_entry, reinterpret_cast<uintptr_t>(function));

			name_table_entry++;
			address_table_entry++;
		}

		descriptor++;
	}
}

void loader::load_tls(const utils::nt::library& module, const utils::nt::library& source) const
{
	if (source.get_optional_header()->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size)
	{
		const auto target_tls = reinterpret_cast<PIMAGE_TLS_DIRECTORY>(module.get_ptr() + module
			.get_optional_header()
			->
			DataDirectory
			[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);
		const auto source_tls = reinterpret_cast<PIMAGE_TLS_DIRECTORY>(module.get_ptr() + source
			.get_optional_header()
			->
			DataDirectory
			[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);

		const auto tls_size = source_tls->EndAddressOfRawData - source_tls->StartAddressOfRawData;
		const auto tls_index = *reinterpret_cast<DWORD*>(target_tls->AddressOfIndex);
		*reinterpret_cast<DWORD*>(source_tls->AddressOfIndex) = tls_index;

		if (tls_size > TLS_PAYLOAD_SIZE)
		{
			throw std::runtime_error(utils::string::va(
				"TLS data is of size 0x%X, but we have only reserved 0x%X bytes!", tls_size, TLS_PAYLOAD_SIZE));
		}

		DWORD old_protect;
		VirtualProtect(PVOID(target_tls->StartAddressOfRawData),
			source_tls->EndAddressOfRawData - source_tls->StartAddressOfRawData, PAGE_READWRITE,
			&old_protect);

		const auto tls_base = *reinterpret_cast<LPVOID*>(__readfsdword(0x2C) + 4 * tls_index);
		std::memmove(tls_base, PVOID(source_tls->StartAddressOfRawData), tls_size);
		std::memmove(PVOID(target_tls->StartAddressOfRawData), PVOID(source_tls->StartAddressOfRawData), tls_size);
	}
}

FARPROC loader::load_library(const std::string& filename) const
{
	const auto target = utils::nt::library::load(filename);
	if (!target)
	{
		throw std::runtime_error("Failed to map binary!");
	}

	this->load_imports(target, target);
	this->load_tls(target, target);

	return FARPROC(target.get_ptr() + target.get_relative_entry_point());
}