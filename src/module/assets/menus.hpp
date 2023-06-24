#pragma once

#define MAX_SOURCEFILES	64
#undef LoadMenu

namespace components
{
	class Menus : public module
	{
	public:
		void post_load() override;
		~Menus();

		static void FreeEverything();

		static void Add(const std::string& menu);

		static game::native::MenuList* LoadCustomMenuList(const std::string& menu, utils::memory::allocator* allocator);
		static std::vector<std::pair<bool, game::native::menuDef_t*>> LoadMenu(game::native::menuDef_t* menudef);
		static std::vector<std::pair<bool, game::native::menuDef_t*>> LoadMenu(const std::string& file);

	private:
		static std::unordered_map<std::string, game::native::menuDef_t*> MenuList;
		static std::unordered_map<std::string, game::native::MenuList*> MenuListList;
		static std::vector<std::string> CustomMenus;

		static game::native::XAssetHeader MenuFindHook(game::native::XAssetType type, const std::string& filename);
		static game::native::XAssetHeader MenuListFindHook(game::native::XAssetType type, const std::string& filename);

		static game::native::MenuList* LoadMenuList(game::native::MenuList* menuList);
		static game::native::MenuList* LoadScriptMenu(const char* menu);

		static void SafeMergeMenus(std::vector<std::pair<bool, game::native::menuDef_t*>>* menus, std::vector<std::pair<bool, game::native::menuDef_t*>> newMenus);

		static game::native::script_t* LoadMenuScript(const std::string& name, const std::string& buffer);
		static int LoadMenuSource(const std::string& name, const std::string& buffer);

		static int ReserveSourceHandle();
		static bool IsValidSourceHandle(int handle);

		static game::native::menuDef_t* ParseMenu(int handle);

		static void FreeMenuSource(int handle);

		static void FreeMenuList(game::native::MenuList* menuList);
		static void FreeMenu(game::native::menuDef_t* menudef);

		static void RemoveMenu(const std::string& menu);
		static void RemoveMenu(game::native::menuDef_t* menudef);
		static void RemoveMenuList(const std::string& menuList);
		static void RemoveMenuList(game::native::MenuList* menuList);

		static void OverrideMenu(game::native::menuDef_t* menu);

		static bool IsMenuVisible(game::native::UiContext* dc, game::native::menuDef_t* menu);

		static void RemoveMenuFromContext(game::native::UiContext* dc, game::native::menuDef_t* menu);

		static void RegisterCustomMenusHook();

		// Ugly!
		static int KeywordHash(char* key);
	};
}