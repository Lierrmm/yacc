#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include <game/game.hpp>

#include "localization.hpp"

#include <utils/string.hpp>
#include <utils/hook.hpp>
#include <utils/memory.hpp>

#include "asset_handler.hpp"
#include "stringtable.hpp"

std::recursive_mutex localization::LocalizeMutex;
game::native::dvar_t* localization::UseLocalization;
std::unordered_map<std::string, game::native::LocalizeEntry*> localization::LocalizeMap;
std::unordered_map<std::string, game::native::LocalizeEntry*> localization::TempLocalizeMap;

utils::hook::detour SELoadLanguageHook;
utils::hook::detour SELoadHook;

void localization::Set(const std::string& key, const std::string& value)
{
	std::lock_guard<std::recursive_mutex> _(localization::LocalizeMutex);
	utils::memory::allocator* allocator = utils::memory::get_allocator();

	if (localization::LocalizeMap.find(key) != localization::LocalizeMap.end())
	{
		game::native::LocalizeEntry* entry = localization::LocalizeMap[key];

		char* newStaticValue = allocator->duplicate_string(value);
		if (!newStaticValue) return;
		if (entry->value) allocator->free(entry->value);
		entry->value = newStaticValue;
		return;
	}

	game::native::LocalizeEntry* entry = allocator->allocate<game::native::LocalizeEntry>();
	if (!entry) return;

	entry->name = allocator->duplicate_string(key);
	if (!entry->name)
	{
		allocator->free(entry);
		return;
	}

	entry->value = allocator->duplicate_string(value);
	if (!entry->value)
	{
		allocator->free(entry->name);
		allocator->free(entry);
		return;
	}

	localization::LocalizeMap[key] = entry;
}

const char* localization::Get(const char* key)
{
	if (!localization::UseLocalization->current.enabled) return key;

	game::native::LocalizeEntry* entry = nullptr;
	{
		std::lock_guard<std::recursive_mutex> _(localization::LocalizeMutex);

		if (localization::TempLocalizeMap.find(key) != localization::TempLocalizeMap.end())
		{
			entry = localization::TempLocalizeMap[key];
		}
		else if (localization::LocalizeMap.find(key) != localization::LocalizeMap.end())
		{
			entry = localization::LocalizeMap[key];
		}
	}

	if (!entry || !entry->value)
	{
		entry = game::native::DB_FindXAssetHeader(game::native::XAssetType::ASSET_TYPE_LOCALIZE_ENTRY, key).localize;
	}

	if (entry && entry->value)
	{
		return entry->value;
	}

	return key;
}

void localization::SetTemp(const std::string& key, const std::string& value)
{
	std::lock_guard<std::recursive_mutex> _(localization::LocalizeMutex);
	utils::memory::allocator* allocator = utils::memory::get_allocator();

	if (localization::TempLocalizeMap.find(key) != localization::TempLocalizeMap.end())
	{
		game::native::LocalizeEntry* entry = localization::TempLocalizeMap[key];
		if (entry->value) allocator->free(entry->value);
		entry->value = allocator->duplicate_string(value);
	}
	else
	{
		game::native::LocalizeEntry* entry = allocator->allocate<game::native::LocalizeEntry>();
		if (!entry) return;

		entry->name = allocator->duplicate_string(key);
		if (!entry->name)
		{
			allocator->free(entry);
			return;
		}

		entry->value = allocator->duplicate_string(value);
		if (!entry->value)
		{
			allocator->free(entry->name);
			allocator->free(entry);
			return;
		}

		localization::TempLocalizeMap[key] = entry;
	}
}

void localization::ClearTemp()
{
	std::lock_guard<std::recursive_mutex> _(localization::LocalizeMutex);
	utils::memory::allocator* allocator = utils::memory::get_allocator();

	for (auto i = localization::TempLocalizeMap.begin(); i != localization::TempLocalizeMap.end(); ++i)
	{
		if (i->second)
		{
			if (i->second->name)  allocator->free(i->second->name);
			if (i->second->value) allocator->free(i->second->value);
			allocator->free(i->second);
		}
	}

	localization::TempLocalizeMap.clear();
}

void __stdcall localization::SetStringStub(const char* value, bool isEnglish)
{
	const static uint32_t retn_addr = 0x53474E;
	const static uint32_t block{};
	__asm
	{
		pushad
		mov eax, [esp + 18h]
		popad

		lea     edi, [esp + 434h + eax]
		push    eax
		push    ebx
		mov ecx, ebp

		pushad
		call localization::Set
		popad

		jmp retn_addr
	}
}

void localization::LoadLanguageStrings(bool forceEnglish)
{
	if (stringtable::FFile(utils::string::va("localizedstrings/yacc_%s.str", game::native::Win_GetLanguage())).exists())
	{
		const char* localizedstrings = utils::string::va("localizedstrings/yacc_%s.str", game::native::Win_GetLanguage());
		game::native::SE_Load(localizedstrings, 0);
	}
	else if (stringtable::FFile("localizedstrings/yacc_english.str").exists())
	{
		game::native::SE_Load("localizedstrings/yacc_english.str", 0);
	}
}

__declspec(naked) void localization::SELoadLanguageStub()
{
	__asm
	{
		pushad
		call localization::LoadLanguageStrings
		popad

		push 534A60h
		retn
	}
}

void localization::post_load()
{
	AssetHandler::OnFind(game::native::XAssetType::ASSET_TYPE_LOCALIZE_ENTRY, [](game::native::XAssetType, const std::string& filename)
	{
		game::native::XAssetHeader header = { nullptr };
		std::lock_guard<std::recursive_mutex> _(localization::LocalizeMutex);

		if (localization::TempLocalizeMap.find(filename) != localization::TempLocalizeMap.end())
		{
			header.localize = localization::TempLocalizeMap[filename];
		}
		else if (localization::LocalizeMap.find(filename) != localization::LocalizeMap.end())
		{
			header.localize = localization::LocalizeMap[filename];
		}

		return header;
	});

	// Resolving hook
	utils::hook(0x533500, localization::Get, HOOK_JUMP).install()->quick();

	//SELoadLanguageHook.create(game::native::SE_LoadLanguage, localization::LoadLanguageStrings); // Hook the actual function instead of its caller
	utils::hook(0x534DBF, localization::SELoadLanguageStub, HOOK_CALL).install()->quick();

	// Overwrite SetString
	//utils::hook(0x534741, localization::SetStringStub, HOOK_CALL).install()->quick();

	localization::UseLocalization = game::native::Dvar_RegisterBool("ui_localize", "Use localization strings", true, game::native::dvar_flags::dvar_none);
}

void localization::pre_destroy()
{
	localization::ClearTemp();
	localization::LocalizeMap.clear();
}

REGISTER_MODULE(localization);