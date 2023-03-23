#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include <game/game.hpp>

#include "stringtable.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/memory.hpp>

#include "menus.hpp"

#include "asset_handler.hpp"
#include "command.hpp"

std::vector<std::string> menus::CustomMenus;
std::unordered_map<std::string, game::native::menuDef_t*> menus::MenuList;
std::unordered_map<std::string, game::native::MenuList*> menus::MenuListList;

int menus::ReserveSourceHandle()
{
	// Check if a free slot is available
	int i = 1;
	for (; i < MAX_SOURCEFILES; ++i)
	{
		if (!game::native::sourceFiles[i])
			break;
	}

	if (i >= MAX_SOURCEFILES)
		return 0;

	// Reserve it, if yes
	game::native::sourceFiles[i] = reinterpret_cast<game::native::source_t*>(1);

	return i;
}

game::native::script_t* menus::LoadMenuScript(const std::string& name, const std::string& buffer)
{
	game::native::script_t* script = game::native::Script_Alloc(sizeof(game::native::script_t) + 1 + buffer.length());
	if (!script) return nullptr;

	strcpy_s(script->filename, sizeof(script->filename), name.data());
	script->buffer = reinterpret_cast<char*>(script + 1);

	*(script->buffer + buffer.length()) = '\0';

	script->script_p = script->buffer;
	script->lastscript_p = script->buffer;
	script->length = buffer.length();
	script->end_p = &script->buffer[buffer.length()];
	script->line = 1;
	script->lastline = 1;
	script->tokenavailable = 0;

	game::native::Script_SetupTokens(script);
	script->punctuations = reinterpret_cast<game::native::punctuation_t*>(0x71F6E8);

	std::memcpy(script->buffer, buffer.data(), script->length + 1);

	script->length = game::native::Script_CleanString(script->buffer);

	return script;
}

int menus::LoadMenuSource(const std::string& name, const std::string& buffer)
{
	utils::memory::allocator* allocator = utils::memory::get_allocator();

	int handle = menus::ReserveSourceHandle();
	if (!menus::IsValidSourceHandle(handle)) return 0; // No free source slot!

	game::native::script_t* script = menus::LoadMenuScript(name, buffer);

	if (!script)
	{
		game::native::sourceFiles[handle] = nullptr; // Free reserved slot
		return 0;
	}

	script->next = nullptr;

	auto* source = allocator->allocate<game::native::source_t>();
	if (!source)
	{
		game::native::FreeMemory(script);
		return 0;
	}

	strncpy_s(source->filename, 64, "string", 64);
	source->scriptstack = script;
	source->tokens = nullptr;
	source->defines = nullptr;
	source->indentstack = nullptr;
	source->skip = 0;
	source->definehash = static_cast<game::native::define_t**>(allocator->allocate(4096));

	game::native::sourceFiles[handle] = source;

	return handle;
}

bool menus::IsValidSourceHandle(int handle)
{
	return (handle > 0 && handle < MAX_SOURCEFILES && game::native::sourceFiles[handle]);
}

int menus::KeywordHash(char* key)
{
	int hash = 0;

	if (*key)
	{
		int sub = 3523 - reinterpret_cast<DWORD>(key);
		do
		{
			char _chr = *key;
			hash += reinterpret_cast<DWORD>(&(key++)[sub]) * tolower(_chr);
		} while (*key);
	}

	return (static_cast<uint16_t>(hash) + static_cast<uint16_t>(hash >> 8)) & 0x7F;
}

game::native::menuDef_t* menus::ParseMenu(int handle)
{
	utils::memory::allocator* allocator = utils::memory::get_allocator();

	auto* menu = allocator->allocate<game::native::menuDef_t>();
	if (!menu) return nullptr;

	menu->items = allocator->allocate_array<game::native::itemDef_s*>(512);
	if (!menu->items)
	{
		allocator->free(menu);
		return nullptr;
	}

	game::native::pc_token_t token;
	if (!game::native::PC_ReadTokenHandle(handle, &token) || token.string[0] != '{')
	{
		allocator->free(menu->items);
		allocator->free(menu);
		return nullptr;
	}

	while (true)
	{
		ZeroMemory(&token, sizeof(token));

		if (!game::native::PC_ReadTokenHandle(handle, &token))
		{
			game::native::PC_SourceError(handle, "end of file inside menu\n");
			break; // Fail
		}

		if (*token.string == '}')
		{
			break; // Success
		}

		int idx = menus::KeywordHash(token.string);

		game::native::keywordHash_t* key = game::native::menuParseKeywordHash[idx];

		if (!key)
		{
			game::native::PC_SourceError(handle, "unknown menu keyword %s", token.string);
			continue;
		}

		if (!key->func(menu, handle))
		{
			game::native::PC_SourceError(handle, "couldn't parse menu keyword %s", token.string);
			break; // Fail
		}
	}

	menus::OverrideMenu(menu);
	menus::RemoveMenu(menu->window.name);
	menus::MenuList[menu->window.name] = menu;

	return menu;
}

game::native::MenuList* menus::LoadCustomMenuList(const std::string& menu, utils::memory::allocator* allocator)
{
	std::vector<std::pair<bool, game::native::menuDef_t*>> menus;
	stringtable::FFile menuFile(menu);

	if (!menuFile.exists()) return nullptr;

	game::native::pc_token_t token;
	int handle = menus::LoadMenuSource(menu, menuFile.getBuffer());

	if (menus::IsValidSourceHandle(handle))
	{
		while (true)
		{
			ZeroMemory(&token, sizeof(token));

			if (!game::native::PC_ReadTokenHandle(handle, &token) || token.string[0] == '}')
			{
				break;
			}

			if (!_stricmp(token.string, "loadmenu"))
			{
				game::native::PC_ReadTokenHandle(handle, &token);

				utils::string::Merge(&menus, menus::LoadMenu(utils::string::va("ui_mp\\%s.menu", token.string)));
			}

			if (!_stricmp(token.string, "menudef"))
			{
				game::native::menuDef_t* menudef = menus::ParseMenu(handle);
				if (menudef) menus.push_back({ true, menudef }); // Custom menu
			}
		}

		menus::FreeMenuSource(handle);
	}

	if (menus.empty()) return nullptr;

	// Allocate new menu list
	game::native::MenuList* list = allocator->allocate<game::native::MenuList>();
	if (!list) return nullptr;

	list->menus = allocator->allocate_array<game::native::menuDef_t*>(menus.size());
	if (!list->menus)
	{
		allocator->free(list);
		return nullptr;
	}

	list->name = allocator->duplicate_string(menu);
	list->menuCount = menus.size();

	// Copy new menus
	for (unsigned int i = 0; i < menus.size(); ++i)
	{
		list->menus[i] = menus[i].second;
	}

	return list;
}

std::vector<std::pair<bool, game::native::menuDef_t*>> menus::LoadMenu(const std::string& menu)
{
	std::vector<std::pair<bool, game::native::menuDef_t*>> menus;
	stringtable::FFile menuFile(menu);

	if (menuFile.exists())
	{
		game::native::pc_token_t token;
		int handle = menus::LoadMenuSource(menu, menuFile.getBuffer());

		if (menus::IsValidSourceHandle(handle))
		{
			while (true)
			{
				ZeroMemory(&token, sizeof(token));

				if (!game::native::PC_ReadTokenHandle(handle, &token) || token.string[0] == '}')
				{
					break;
				}

				if (!_stricmp(token.string, "loadmenu"))
				{
					game::native::PC_ReadTokenHandle(handle, &token);

					utils::string::Merge(&menus, menus::LoadMenu(utils::string::va("ui_mp\\%s.menu", token.string)));
				}

				if (!_stricmp(token.string, "menudef"))
				{
					game::native::menuDef_t* menudef = menus::ParseMenu(handle);
					if (menudef) menus.push_back({ true, menudef }); // Custom menu
				}
			}

			menus::FreeMenuSource(handle);
		}
	}

	return menus;
}

std::vector<std::pair<bool, game::native::menuDef_t*>> menus::LoadMenu(game::native::menuDef_t* menudef)
{
	std::vector<std::pair<bool, game::native::menuDef_t*>> menus = menus::LoadMenu(utils::string::va("ui_mp\\%s.menu", menudef->window.name));

	if (menus.empty())
	{
		menus.push_back({ false, menudef }); // Native menu
	}

	return menus;
}

game::native::MenuList* menus::LoadScriptMenu(const char* menu)
{
	utils::memory::allocator* allocator = utils::memory::get_allocator();

	std::vector<std::pair<bool, game::native::menuDef_t*>> menus = menus::LoadMenu(menu);
	if (menus.empty()) return nullptr;

	// Allocate new menu list
	auto* newList = allocator->allocate<game::native::MenuList>();
	if (!newList) return nullptr;

	newList->menus = allocator->allocate_array<game::native::menuDef_t*>(menus.size());
	if (!newList->menus)
	{
		allocator->free(newList);
		return nullptr;
	}

	newList->name = allocator->duplicate_string(menu);
	newList->menuCount = static_cast<int>(menus.size());

	// Copy new menus
	for (unsigned int i = 0; i < menus.size(); ++i)
	{
		newList->menus[i] = menus[i].second;
	}

	menus::RemoveMenuList(newList->name);
	menus::MenuListList[newList->name] = newList;

	return newList;
}

void menus::SafeMergeMenus(std::vector<std::pair<bool, game::native::menuDef_t*>>* menus, std::vector<std::pair<bool, game::native::menuDef_t*>> newMenus)
{
	// Check if we overwrote a menu
	for (auto i = menus->begin(); i != menus->end();)
	{
		// Try to find the native menu
		bool found = !i->first; // Only if custom menu, try to find it

		// If there is none, try to find a custom menu
		if (!found)
		{
			for (auto& entry : menus::MenuList)
			{
				if (i->second == entry.second)
				{
					found = true;
					break;
				}
			}
		}

		// Remove the menu if it has been deallocated (not found)
		if (!found)
		{
			i = menus->erase(i);
			continue;
		}

		bool increment = true;

		// Remove the menu if it has been loaded twice
		for (auto& newMenu : newMenus)
		{
			if (i->second->window.name == std::string(newMenu.second->window.name))
			{
				menus::RemoveMenu(i->second);

				i = menus->erase(i);
				increment = false;
				break;
			}
		}

		if (increment) ++i;
	}

	utils::string::Merge(menus, newMenus);
}

game::native::MenuList* menus::LoadMenuList(game::native::MenuList* menuList)
{
	utils::memory::allocator* allocator = utils::memory::get_allocator();

	std::vector<std::pair<bool, game::native::menuDef_t*>> menus;

	for (int i = 0; i < menuList->menuCount; ++i)
	{
		if (!menuList->menus[i]) continue;
		menus::SafeMergeMenus(&menus, menus::LoadMenu(menuList->menus[i]));
	}

	// Load custom menus
	if (menuList->name == "ui_mp/code.txt"s) // Should be menus, but code is loaded ingame
	{
		for (auto menu : menus::CustomMenus)
		{
			bool hasMenu = false;
			for (auto& loadedMenu : menus)
			{
				if (loadedMenu.second->window.name == menu)
				{
					hasMenu = true;
					break;
				}
			}

			if (!hasMenu) menus::SafeMergeMenus(&menus, menus::LoadMenu(menu));
		}
	}

	// Allocate new menu list
	game::native::MenuList* newList = allocator->allocate<game::native::MenuList>();
	if (!newList) return menuList;

	size_t size = menus.size();
	newList->menus = allocator->allocate_array<game::native::menuDef_t*>(size);
	if (!newList->menus)
	{
		allocator->free(newList);
		return menuList;
	}

	newList->name = allocator->duplicate_string(menuList->name);
	newList->menuCount = size;

	// Copy new menus
	for (unsigned int i = 0; i < menus.size(); ++i)
	{
		newList->menus[i] = menus[i].second;
	}

	menus::RemoveMenuList(newList->name);
	menus::MenuListList[newList->name] = newList;

	return newList;
}

void menus::FreeMenuSource(int handle)
{
	utils::memory::allocator* allocator = utils::memory::get_allocator();

	if (!menus::IsValidSourceHandle(handle)) return;

	game::native::source_t* source = game::native::sourceFiles[handle];

	while (source->scriptstack)
	{
		game::native::script_t* script = source->scriptstack;
		source->scriptstack = source->scriptstack->next;
		game::native::FreeMemory(script);
	}

	while (source->tokens)
	{
		game::native::token_t* token = source->tokens;
		source->tokens = source->tokens->next;
		game::native::FreeMemory(token);
	}

	while (source->defines)
	{
		game::native::define_t* define = source->defines;
		source->defines = source->defines->next;
		game::native::FreeMemory(define);
	}

	while (source->indentstack)
	{
		game::native::indent_t* indent = source->indentstack;
		source->indentstack = source->indentstack->next;
		allocator->free(indent);
	}

	if (source->definehash) allocator->free(source->definehash);

	allocator->free(source);

	game::native::sourceFiles[handle] = nullptr;
}

void menus::FreeMenu(game::native::menuDef_t* menudef)
{
	utils::memory::allocator* allocator = utils::memory::get_allocator();

	// Do i need to free expressions and strings?
	// Or does the game take care of it?
	// Seems like it does...

	if (menudef->items)
	{
		// Seems like this is obsolete as well,
		// as the game handles the memory

		//for (int i = 0; i < menudef->itemCount; ++i)
		//{
		//	game::native::Menu_FreeItemMemory(menudef->items[i]);
		//}

		allocator->free(menudef->items);
	}

	allocator->free(menudef);
}

void menus::FreeMenuList(game::native::MenuList* menuList)
{
	if (!menuList) return;
	utils::memory::allocator* allocator = utils::memory::get_allocator();

	// Keep our compiler happy
	game::native::MenuList list = { menuList->name, menuList->menuCount, menuList->menus };

	if (list.name)
	{
		allocator->free(list.name);
	}

	if (list.menus)
	{
		allocator->free(list.menus);
	}

	allocator->free(menuList);
}

void menus::RemoveMenu(const std::string& menu)
{
	auto i = menus::MenuList.find(menu);
	if (i != menus::MenuList.end())
	{
		if (i->second) menus::FreeMenu(i->second);
		i = menus::MenuList.erase(i);
	}
}

void menus::RemoveMenu(game::native::menuDef_t* menudef)
{
	for (auto i = menus::MenuList.begin(); i != menus::MenuList.end();)
	{
		if (i->second == menudef)
		{
			menus::FreeMenu(menudef);
			i = menus::MenuList.erase(i);
		}
		else
		{
			++i;
		}
	}
}

void menus::RemoveMenuList(const std::string& menuList)
{
	auto i = menus::MenuListList.find(menuList);
	if (i != menus::MenuListList.end())
	{
		if (i->second)
		{
			for (auto j = 0; j < i->second->menuCount; ++j)
			{
				menus::RemoveMenu(i->second->menus[j]);
			}

			menus::FreeMenuList(i->second);
		}

		i = menus::MenuListList.erase(i);
	}
}

// This is actually a really important function
// It checks if we have already loaded the menu we passed and replaces its instances in memory
// Due to deallocating the old menu, the game might crash on not being able to handle its old instance
// So we need to override it in our menu lists and the game's ui context
// EDIT: We might also remove the old instances inside RemoveMenu
// EDIT2: Removing old instances without having a menu to replace them with might leave a nullptr
// EDIT3: Wouldn't it be better to check if the new menu we're trying to load has already been loaded and not was not deallocated and return that one instead of loading a new one?
void menus::OverrideMenu(game::native::menuDef_t* menu)
{
	if (!menu || !menu->window.name) return;
	std::string name = menu->window.name;

	// Find the old menu
	auto i = menus::MenuList.find(name);
	if (i != menus::MenuList.end())
	{
		// We have found it, *yay*
		game::native::menuDef_t* oldMenu = i->second;

		// Replace every old instance with our new one in the ui context
		for (int j = 0; j < game::native::ui_context->menuCount; ++j)
		{
			if (game::native::ui_context->Menus[j] == oldMenu)
			{
				game::native::ui_context->Menus[j] = menu;
			}
		}

		// Replace every old instance with our new one in our menu lists
		for (auto j = menus::MenuListList.begin(); j != menus::MenuListList.end(); ++j)
		{
			game::native::MenuList* list = j->second;

			if (list && list->menus)
			{
				for (int k = 0; k < list->menuCount; ++k)
				{
					if (list->menus[k] == oldMenu)
					{
						list->menus[k] = menu;
					}
				}
			}
		}
	}
}

void menus::RemoveMenuList(game::native::MenuList* menuList)
{
	if (!menuList || !menuList->name) return;
	menus::RemoveMenuList(menuList->name);
}

void menus::FreeEverything()
{
	for (auto i = menus::MenuListList.begin(); i != menus::MenuListList.end(); ++i)
	{
		menus::FreeMenuList(i->second);
	}

	menus::MenuListList.clear();

	for (auto i = menus::MenuList.begin(); i != menus::MenuList.end(); ++i)
	{
		menus::FreeMenu(i->second);
	}

	menus::MenuList.clear();
}

game::native::XAssetHeader menus::MenuFindHook(game::native::XAssetType /*type*/, const std::string& filename)
{
	return { game::native::Menus_FindByName(game::native::ui_context, filename.data()) };
}

game::native::XAssetHeader menus::MenuListFindHook(game::native::XAssetType type, const std::string& filename)
{
	game::native::XAssetHeader header = { nullptr };

	// Free the last menulist and ui context, as we have to rebuild it with the new menus
	if (menus::MenuListList.find(filename) != menus::MenuListList.end())
	{
		game::native::MenuList* list = menus::MenuListList[filename];

		for (int i = 0; list && list->menus && i < list->menuCount; ++i)
		{
			menus::RemoveMenuFromContext(game::native::ui_context, list->menus[i]);
		}

		menus::RemoveMenuList(filename);
	}

	if (utils::string::ends_with(filename, ".menu"))
	{
		if (stringtable::FFile(filename).exists())
		{
			header.menuList = menus::LoadScriptMenu(filename.data());
			if (header.menuList) return header;
		}
	}

	game::native::MenuList* menuList = game::native::DB_FindXAssetHeader(type, filename.data()).menuList;
	header.menuList = menuList;

	if (menuList && reinterpret_cast<DWORD>(menuList) != 0xDDDDDDDD)
	{
		// Parse scriptmenus!
		if ((menuList->menuCount > 0 && menuList->menus[0] && menuList->menus[0]->window.name == "default_menu"s))
		{
			if (stringtable::FFile(filename).exists())
			{
				header.menuList = menus::LoadScriptMenu(filename.data());

				// Reset, if we didn't find scriptmenus
				if (!header.menuList)
				{
					header.menuList = menuList;
				}
			}
		}
		else
		{
			header.menuList = menus::LoadMenuList(menuList);
		}
	}
	else
	{
		header.menuList = nullptr;
	}

	return header;
}

bool menus::IsMenuVisible(game::native::UiContext* dc, game::native::menuDef_t* menu)
{
	if (menu && menu->window.name)
	{
		if (menu->window.name == "connect"s) // Check if we're supposed to draw the loadscreen
		{
			game::native::menuDef_t* originalConnect = AssetHandler::FindOriginalAsset(game::native::XAssetType::ASSET_TYPE_MENU, "connect").menu;

			if (originalConnect == menu) // Check if we draw the original loadscreen
			{
				if (menus::MenuList.find("connect") != menus::MenuList.end()) // Check if we have a custom loadscreen, to prevent drawing the original one on top
				{
					return false;
				}
			}
		}
	}

	return game::native::Menu_IsVisible(dc, menu);
}

void menus::RemoveMenuFromContext(game::native::UiContext* dc, game::native::menuDef_t* menu)
{
	// Search menu in context
	int i = 0;
	for (; i < dc->menuCount; ++i)
	{
		if (dc->Menus[i] == menu)
		{
			break;
		}
	}

	// Remove from stack
	if (i < dc->menuCount)
	{
		for (; i < dc->menuCount - 1; ++i)
		{
			dc->Menus[i] = dc->Menus[i + 1];
		}

		// Clear last menu
		dc->Menus[--dc->menuCount] = nullptr;
	}
}

void menus::Add(const std::string& menu)
{
	menus::CustomMenus.push_back(menu);
}

void LoadOriginalMenus()
{
	// This is inlined in COD4 - so we need to re-write it..
	game::native::MenuList* menuList{};
	auto useFastFile = game::native::Dvar_FindVar("useFastFile");
	if (useFastFile->current.enabled)
	{
		menuList = game::native::UI_LoadMenus("ui_mp/code.txt");
		game::native::UI_AddMenuList(game::native::ui_context, menuList);
		useFastFile = game::native::Dvar_FindVar("useFastFile");
	}
	if (!*game::native::serverMap || !useFastFile->current.enabled)
	{
		bool shouldUseFastFiles = !useFastFile->current.enabled;
		auto func = game::native::UI_LoadMenus;
		if (shouldUseFastFiles)
			func = game::native::sub_552E10;
		menuList = func("ui_mp/menus.txt");
		game::native::UI_AddMenuList(game::native::ui_context, menuList);
		useFastFile = game::native::Dvar_FindVar("useFastFile");
	}
}

void menus::post_load()
{
	menus::FreeEverything();

	AssetHandler::OnFind(game::native::ASSET_TYPE_MENU, menus::MenuFindHook);
	AssetHandler::OnFind(game::native::ASSET_TYPE_MENULIST, menus::MenuListFindHook);

	//utils::hook(0x543725, LoadOriginalMenus, HOOK_CALL).install()->quick();
}

void menus::pre_destroy()
{
	menus::FreeEverything();
}

//REGISTER_MODULE(menus);