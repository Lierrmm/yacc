#include "std_include.hpp"
#include "launcher/launcher.hpp"
#include "loader/loader.hpp"
#include "loader/module_loader.hpp"
#include "game/game.hpp"
#include "utils/string.hpp"
#include "utils/flags.hpp"

//#define GENERATE_DIFFS

#ifdef GENERATE_DIFFS
#include "loader/binary_loader.hpp"
#endif
#include <utils/io.hpp>
#include <utils/properties.hpp>

DECLSPEC_NORETURN void WINAPI exit_hook(const int code)
{
	module_loader::pre_destroy();
	//module_loader::uninitialize();
	exit(code);
}

void verify_tls()
{
	const utils::nt::library self;
	const auto self_tls = reinterpret_cast<PIMAGE_TLS_DIRECTORY>(self.get_ptr()
		+ self.get_optional_header()->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);

	const auto ref = DWORD(&tls_data);
	const auto tls_index = *reinterpret_cast<DWORD*>(self_tls->AddressOfIndex);
	const auto tls_vector = *reinterpret_cast<DWORD*>(__readfsdword(0x2C) + 4 * tls_index);
	const auto offset = ref - tls_vector;

	if (offset != 0 && offset != 8) // Actually 8 is bad, but I think msvc places custom stuff before
	{
		throw std::runtime_error(utils::string::va("TLS payload is at offset 0x%X, but should be at 0!", offset));
	}
}

launcher::mode detect_mode_from_arguments()
{
	if (utils::flags::has_flag("multiplayer"))
	{
		return launcher::mode::multiplayer;
	}

	return launcher::mode::none;
}

DECLSPEC_NORETURN void WINAPI RaiseException_Stub(DWORD dwExceptionCode,
	DWORD dwExceptionFlags,
	DWORD nNumberOfArguments,
	CONST ULONG_PTR* lpArguments)
{

#ifdef DEBUG
	printf("[RaiseException] %d %d %d %p\n", dwExceptionCode, dwExceptionFlags, nNumberOfArguments, lpArguments);
#endif // DEBUG
}

BOOL WINAPI system_parameters_info_a(const UINT uiAction, const UINT uiParam, const PVOID pvParam, const UINT fWinIni)
{
	module_loader::post_unpack();
	return SystemParametersInfoA(uiAction, uiParam, pvParam, fWinIni);
}

FARPROC WINAPI get_proc_address(const HMODULE hModule, const LPCSTR lpProcName)
{
	if (lpProcName == "GlobalMemoryStatusEx"s)
	{
		module_loader::post_unpack();
	}

	return GetProcAddress(hModule, lpProcName);
}

bool is_valid_version()
{
	auto version = *reinterpret_cast<unsigned int*>(0x45DE76);
	return version == 13620;
}

void get_patched_binary(std::string* binary, std::string* data)
{
	const auto patched_binary = (utils::properties::get_appdata_path() / "bin" / *binary).generic_string();

	try
	{
		if (!utils::io::file_exists(patched_binary) && !utils::io::write_file(patched_binary, *data, false))
		{
			throw std::runtime_error("Could not write file");
		}
	}
	catch (const std::exception& e)
	{
		throw std::runtime_error(
			utils::string::va("Could not create patched binary for %s! %s",
				binary->data(), e.what())
		);
	}

	*binary = patched_binary;
}

FARPROC load_binary(const launcher::mode mode)
{
	loader loader(mode);
	utils::nt::library self;

	loader.set_import_resolver([self](const std::string& module, const std::string& function) -> void*
	{
		if (module == "steam_api.dll")
		{
			return self.get_proc<FARPROC>(function);
		}
		else if (function == "ExitProcess")
		{
			return exit_hook;
		}
		else if (function == "RaiseException")
		{
			return RaiseException_Stub;
		}
		else if (function == "SystemParametersInfoA")
		{
			return system_parameters_info_a;
		}
		else if (function == "GetProcAddress")
		{
			return get_proc_address;
		}

		return module_loader::load_import(module, function);
	});

	std::string binary;
	switch (mode)
	{
	case launcher::mode::multiplayer:
		binary = "iw3mp.exe";
		break;
	case launcher::mode::none:
	default:
		throw std::runtime_error("Invalid game mode!");
	}

	std::string data;
	if (!utils::io::read_file(binary, &data))
	{
		throw std::runtime_error(utils::string::va(
			"Failed to read game binary (%s)!\nPlease copy the yacc.exe into your Call of Duty: Modern Warfare installation folder and run it from there.",
			binary.data()));
	}

	get_patched_binary(&binary, &data);

	#ifdef INJECT_HOST_AS_LIB
		return loader.load_library(binary);
	#else
		return loader.load(self, data);
	#endif
}

void enable_dpi_awareness()
{
	const utils::nt::library user32{"user32.dll"};
	const auto set_dpi = user32
		                     ? user32.get_proc<BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT)>("SetProcessDpiAwarenessContext")
		                     : nullptr;
	if (set_dpi)
	{
		set_dpi(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	}
}

void limit_parallel_dll_loading()
{
	const utils::nt::library self;
	const auto registry_path = R"(Software\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\)" + self.
		get_name();

	HKEY key = nullptr;
	if (RegCreateKeyA(HKEY_LOCAL_MACHINE, registry_path.data(), &key) == ERROR_SUCCESS)
	{
		RegCloseKey(key);
	}

	key = nullptr;
	if (RegOpenKeyExA(
		HKEY_LOCAL_MACHINE, registry_path.data(), 0,
		KEY_ALL_ACCESS, &key) != ERROR_SUCCESS)
	{
		return;
	}

	DWORD value = 1;
	RegSetValueExA(key, "MaxLoaderThreads", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));

	RegCloseKey(key);
}

int main()
{
	FARPROC entry_point;
	enable_dpi_awareness();
	limit_parallel_dll_loading();

	std::srand(static_cast<std::uint32_t>(time(nullptr)) ^ ~(GetTickCount() * GetCurrentProcessId()));
	{
		module_loader::sort();

		auto premature_shutdown = true;
		const auto _ = gsl::finally([&premature_shutdown]()
		{
			if (premature_shutdown)
			{
				module_loader::pre_destroy();
			}
		});

		try
		{
#ifdef GENERATE_DIFFS
			binary_loader::create();
			return 0;
#endif

			verify_tls();
			if (!module_loader::post_start()) return 0;

			auto mode = detect_mode_from_arguments();
			if (mode == launcher::mode::none)
			{
				const launcher launcher;
				mode = launcher.run();
				if (mode == launcher::mode::none) return 0;
			}

			entry_point = load_binary(mode);
			if (!entry_point)
			{
				throw std::runtime_error("Unable to load binary into memory");
			}

			if (!is_valid_version())
			{
				MessageBoxA(nullptr, "COD4 version not supported\nMake sure you're running the latest Steam version (1.8.13620)", "ERROR", MB_ICONERROR);
				return 1;
			}
		
			game::initialize(mode);
			if (!module_loader::post_load()) return 0;

			premature_shutdown = false;
		}
		catch (std::exception& e)
		{
			MessageBoxA(nullptr, e.what(), "ERROR", MB_ICONERROR);
			return 1;
		}
	}

	try 
	{
		return entry_point();
	}
	catch (std::exception ex)
	{
		MessageBoxA(nullptr, ex.what(), "ENTRYPOINT ERROR", MB_ICONINFORMATION);
	}
}


int APIENTRY WinMain(HINSTANCE, HINSTANCE, PSTR, int) 
{ 
	DWORD oldProtect;
	VirtualProtect(GetModuleHandle(nullptr), 0xD536000, PAGE_EXECUTE_READWRITE, &oldProtect);

	return main();
}
