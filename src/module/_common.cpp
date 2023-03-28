#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include "console.hpp"
#include <utils/io.hpp>
#include "command.hpp"

void force_dvars_on_init()
{
	if (const auto dedicated = game::native::Dvar_FindVar("dedicated"); dedicated && !dedicated->current.integer)
	{
		const auto sv_pure = game::native::Dvar_FindVar("sv_pure");
		const auto com_introPlayed = game::native::Dvar_FindVar("com_introPlayed");
		const auto sv_punkbuster = game::native::Dvar_FindVar("sv_punkbuster");
		const auto r_zFeather = game::native::Dvar_FindVar("r_zFeather");
		const auto r_distortion = game::native::Dvar_FindVar("r_distortion");
		const auto r_fastSkin = game::native::Dvar_FindVar("r_fastSkin");

		if (com_introPlayed && com_introPlayed->current.enabled)
		{
			game::native::dvar_set_value_dirty(com_introPlayed, false);
			game::native::Cmd_ExecuteSingleCommand((game::native::LocalClientNum_t)0, 0, "com_introPlayed 0\n");
		}

		if (sv_pure && sv_pure->current.enabled)
		{
			game::native::dvar_set_value_dirty(sv_pure, false);
			game::native::Cmd_ExecuteSingleCommand((game::native::LocalClientNum_t)0, 0, "sv_pure 0\n");
		}

		if (sv_punkbuster && sv_punkbuster->current.enabled)
		{
			game::native::dvar_set_value_dirty(sv_punkbuster, false);
			game::native::Cmd_ExecuteSingleCommand((game::native::LocalClientNum_t)0, 0, "sv_punkbuster 0\n");
		}

		// force depthbuffer
		if (r_zFeather && !r_zFeather->current.enabled)
		{
			game::native::Cmd_ExecuteSingleCommand((game::native::LocalClientNum_t)0, 0, "r_zFeather 1\n");
		}

		// enable distortion (it creates a rendertarget thats needed)
		if (r_distortion && !r_distortion->current.enabled)
		{
			game::native::Cmd_ExecuteSingleCommand((game::native::LocalClientNum_t)0, 0, "r_distortion 1\n");
		}

		// disable r_fastSkin in-case someone increased renderbuffers
		if (r_fastSkin && r_fastSkin->current.enabled)
		{
			game::native::Cmd_ExecuteSingleCommand((game::native::LocalClientNum_t)0, 0, "r_fastSkin 0\n");
		}
	}

	if (const auto	in_mouse = game::native::Dvar_FindVar("in_mouse");
		in_mouse && !in_mouse->current.enabled)
	{
		game::native::dvar_set_value_dirty(in_mouse, true); // quick set the value
		game::native::Cmd_ExecuteSingleCommand((game::native::LocalClientNum_t)0, 0, "in_mouse 1\n");
	}
}

__declspec(naked) void R_BeginRegistration_stub()
{
	const static uint32_t retn_addr = 0x468F1E;
	__asm
	{
		pushad;
		call	force_dvars_on_init;
		popad;

		mov     ecx, 0Ch;	// overwritten op
		jmp		retn_addr;
	}
}

// disable cheat / write check
__declspec(naked) void disable_dvar_cheats_stub()
{
	const static uint32_t retn_addr = 0x565A4F;
	__asm
	{
		movzx   eax, word ptr[ebx + 8];	// overwritten op
		jmp		retn_addr;
	}
}

// *
	// FastFiles

	// load common + addon fastfiles (only on init or when changing CGame)
void load_common_fast_files()
{
	/*
	only unload zones with a set flag
	:: zone.name = 0;
	:: zone.allocFlags = 0;
	:: zone.freeFlags = ZONE_FLAG_TO_UNLOAD
	*/

	int i = 0;
	game::native::XZoneInfo xzone_info_stack[8];

	// ------------------------------------

	xzone_info_stack[i].name = *game::native::zone_code_post_gfx_mp;
	xzone_info_stack[i].allocFlags = game::native::XZONE_FLAGS::XZONE_POST_GFX;
	xzone_info_stack[i].freeFlags = game::native::XZONE_FLAGS::XZONE_POST_GFX_FREE;
	++i;

	// ------------------------------------

	if (*game::native::zone_localized_code_post_gfx_mp)
	{
		xzone_info_stack[i].name = *game::native::zone_localized_code_post_gfx_mp;
		xzone_info_stack[i].allocFlags = game::native::XZONE_FLAGS::XZONE_LOC_POST_GFX;
		xzone_info_stack[i].freeFlags = game::native::XZONE_FLAGS::XZONE_LOC_POST_GFX_FREE;
		++i;
	}

	// ---------------------------------------------------------------------------------------------------------

	// if addon_menu loading is enabled
	if (FF_LOAD_ADDON_MENU)
	{
		// if the fastfile exists
		if (FF_ADDON_MENU_NAME && game::native::DB_FileExists(FF_ADDON_MENU_NAME, game::native::DB_FILE_EXISTS_PATH::DB_PATH_ZONE))
		{
			// file exists, check if a mod was loaded
			if (!*game::native::zone_mod)
			{
				// only load the menu when no mod is loaded -- unload on mod | map load
				xzone_info_stack[i].name = FF_ADDON_MENU_NAME;

				xzone_info_stack[i].allocFlags = game::native::XZONE_FLAGS::XZONE_MOD | game::native::XZONE_FLAGS::XZONE_DEBUG;
				++i;

				// we have to sync assets or we run into issues when the game is trying to access unloaded assets
				game::native::DB_LoadXAssets(&xzone_info_stack[0], i, 0);

				game::native::R_BeginRemoteScreenUpdate();
				WaitForSingleObject(game::native::database_handle, 0xFFFFFFFF);
				game::native::R_EndRemoteScreenUpdate();
				game::native::DB_SyncXAssets();

				// start a new zone stack
				i = 0;
			}
		}

		// if addon_menu loading is enabled and file not found
		else if (FF_LOAD_ADDON_MENU)
		{
			game::native::Com_PrintMessage(0, utils::string::va("^1DB_LoadCommonFastFiles^7:: %s.ff not found. \n", FF_ADDON_MENU_NAME), 0);
		}
	}

	// mod loading -- maybe sync assets for addon menu too?
	if (*game::native::zone_mod)
	{
		xzone_info_stack[i].name = *game::native::zone_mod;
		xzone_info_stack[i].allocFlags = game::native::XZONE_FLAGS::XZONE_MOD;
		xzone_info_stack[i].freeFlags = game::native::XZONE_FLAGS::XZONE_MOD_FREE;
		++i;

		game::native::DB_LoadXAssets(&xzone_info_stack[0], i, 0);

		game::native::R_BeginRemoteScreenUpdate();
		WaitForSingleObject(game::native::database_handle, 0xFFFFFFFF);

		game::native::R_EndRemoteScreenUpdate();
		game::native::DB_SyncXAssets();

		// start a new zone stack
		i = 0;
	}

	// ---------------------------------------------------------------------------------------------------------

	if (*game::native::zone_ui_mp) // not loaded when starting dedicated servers
	{
		xzone_info_stack[i].name = *game::native::zone_ui_mp;
		xzone_info_stack[i].allocFlags = game::native::XZONE_FLAGS::XZONE_UI;
		xzone_info_stack[i].freeFlags = game::native::XZONE_FLAGS::XZONE_UI_FREE;
		++i;
	}

	// ------------------------------------

	xzone_info_stack[i].name = *game::native::zone_common_mp;
	xzone_info_stack[i].allocFlags = game::native::XZONE_FLAGS::XZONE_COMMON;
	xzone_info_stack[i].freeFlags = game::native::XZONE_FLAGS::XZONE_COMMON_FREE;
	++i;

	// ------------------------------------

	if (*game::native::zone_localized_common_mp) // not loaded on when starting dedicated servers
	{
		xzone_info_stack[i].name = *game::native::zone_localized_common_mp;
		xzone_info_stack[i].allocFlags = game::native::XZONE_FLAGS::XZONE_LOC_COMMON;
		xzone_info_stack[i].freeFlags = game::native::XZONE_FLAGS::XZONE_LOC_COMMON_FREE;
		++i;
	}

	// ------------------------------------

	game::native::DB_LoadXAssets(&xzone_info_stack[0], i, 0);
}

// realloc zones that get unloaded on map load (eg. ui_mp)
void com_start_hunk_users()
{
	if (const auto useFastFile = game::native::Dvar_FindVar("useFastFile"); useFastFile && useFastFile->current.enabled)
	{
		int i = 0;
		game::native::XZoneInfo XZoneInfoStack[2];

		// if addon menu loading is enabled
		if (FF_LOAD_ADDON_MENU)
		{
			// if the fastfile exists
			if (FF_ADDON_MENU_NAME && game::native::DB_FileExists(FF_ADDON_MENU_NAME, game::native::DB_FILE_EXISTS_PATH::DB_PATH_ZONE))
			{
				// file exists, check if a mod was loaded
				if (!*game::native::zone_mod)
				{
					// only load the menu when no mod is loaded -- unload on mod | map load
					XZoneInfoStack[i].name = FF_ADDON_MENU_NAME;

					XZoneInfoStack[i].allocFlags = game::native::XZONE_FLAGS::XZONE_MOD | game::native::XZONE_FLAGS::XZONE_DEBUG;
					XZoneInfoStack[i].freeFlags = game::native::XZONE_FLAGS::XZONE_UI_FREE_INGAME;
					++i;
				}
			}

			// if addon menu loading is enabled and file not found
			else if (FF_LOAD_ADDON_MENU)
			{
				game::native::Com_PrintMessage(0, utils::string::va("^1Com_StartHunkUsers^7:: %s.ff not found. \n", FF_ADDON_MENU_NAME), 0);
			}
		}

		// ------------------------------------

		XZoneInfoStack[i].name = *game::native::zone_ui_mp;
		XZoneInfoStack[i].allocFlags = game::native::XZONE_FLAGS::XZONE_UI;
		XZoneInfoStack[i].freeFlags = game::native::XZONE_FLAGS::XZONE_UI_FREE_INGAME;
		++i;

		// ------------------------------------

		game::native::DB_LoadXAssets(&XZoneInfoStack[0], i, 0);
	}
}

// :: Com_StartHunkUsers
__declspec(naked) void com_start_hunk_users_stub()
{
	const static uint32_t retn_addr = 0x4FABF8;
	__asm
	{
		pushad;
		call	com_start_hunk_users;
		popad;

		jmp		retn_addr;
	}
}

// *
// IWDs

int q_stricmpn(const char* s1, const char* s2, int n)
{
	int	c1, c2;

	if (s1 == nullptr)
	{
		if (s2 == nullptr)
		{
			return 0;
		}

		return -1;

	}

	if (s2 == nullptr)
	{
		return 1;
	}

	do
	{
		c1 = *s1++;
		c2 = *s2++;

		if (!n--)
		{
			return 0; // strings are equal until end point
		}

		if (c1 != c2)
		{
			if (c1 >= 'a' && c1 <= 'z')
			{
				c1 -= ('a' - 'A');
			}

			if (c2 >= 'a' && c2 <= 'z')
			{
				c2 -= ('a' - 'A');
			}

			if (c1 != c2)
			{
				return c1 < c2 ? -1 : 1;
			}
		}

	} while (c1);

	return 0; // strings are equal
}

bool iwd_match_xcommon(const char* s0)
{
	if (!q_stricmpn(s0, "xcommon_", 8))
	{
		return false;
	}

	return true;
}

bool iwd_match_iw(const char* s0)
{
	if (!q_stricmpn(s0, "iw_", 3))
	{
		return false;
	}

	return true;
}

// load "iw_" iwds and "xcommon_" iwds as localized ones
__declspec(naked) void FS_MakeIWDsLocalized()
{
	const static uint32_t err_msg = 0x55826A;
	const static uint32_t retn_addr = 0x558288;
	__asm
	{
		push	edi;				// current iwd string + ext
		call	iwd_match_iw;
		add		esp, 4;
		test    eax, eax;

		je		MATCH;				// jump if iwd matched iw_
		// if not, cmp to xcommon_
		push	edi;				// current iwd string + ext
		call	iwd_match_xcommon;
		add		esp, 4;
		test    eax, eax;

		je		MATCH;				// jump if iwd matched xcommon_
		jmp		err_msg;			// yeet


	MATCH:
		mov     ebx, [ebp - 4];		// whatever df that is
		mov[ebp - 8], 1;		// set qLocalized to true
		mov[ebp - 0Ch], esi;	// whatever df that is
		jmp		retn_addr;
	}
}

void db_realloc_entry_pool()
{
	const size_t ALLOC_SIZE = 789312;
	game::native::XAssetEntry* entry_pool = utils::memory::get_allocator()->allocate_array<game::native::XAssetEntry>(ALLOC_SIZE);

	// Apply new size
	utils::hook::set<DWORD>(0x484430, ALLOC_SIZE);

	// Apply new pool
	DWORD asset_entry_pool_patches[] =
	{
		0x484428, 0x484658, 0x484685, 0x4847A4, 0x484815,
		0x484868, 0x48492E, 0x484DD4, 0x484E84, 0x485047,
		0x485103, 0x4851BA, 0x4851E3, 0x48528D, 0x4853D6,
		0x48634D, 0x4864A4, 0x486529, 0x48664C, 0x4866C4,
		0x486984, 0x4869D8
	};

	for (int i = 0; i < ARRAYSIZE(asset_entry_pool_patches); ++i)
	{
		utils::hook::set<game::native::XAssetEntry*>(asset_entry_pool_patches[i], entry_pool);
	}

	utils::hook::set<game::native::XAssetEntry*>(0x484411, entry_pool + 1);
	utils::hook::set<game::native::XAssetEntry*>(0x484422, entry_pool + 1);
}

void copy_custom_fastfile()
{
	const auto resource = FindResource(utils::nt::library(), MAKEINTRESOURCE(COMMON_FF), RT_RCDATA);
	if (!resource)
	{
		console::info("Failed to find custom FastFiles!\n");
		return;
	}

	const auto handle = LoadResource(nullptr, resource);
	if (!handle) 
	{
		console::info("Failed to load custom FastFiles!\n");
		return;
	}

	const auto out_name = utils::string::va("zone/english/%s.ff", FF_ADDON_MENU_NAME);
	utils::io::write_file(out_name, std::string(LPSTR(LockResource(handle)), SizeofResource(nullptr, resource)));
}

class common final : public module
{
public:
	void post_start() override
	{
		copy_custom_fastfile();
	}

	void post_load() override
	{
		// courtesy of iw3xo
		db_realloc_entry_pool();

		game::native::db_realloc_xasset_pool(game::native::XAssetType::ASSET_TYPE_IMAGE, 7168);
		game::native::db_realloc_xasset_pool(game::native::XAssetType::ASSET_TYPE_LOADED_SOUND, 2700);
		game::native::db_realloc_xasset_pool(game::native::XAssetType::ASSET_TYPE_FX, 1200);
		game::native::db_realloc_xasset_pool(game::native::XAssetType::ASSET_TYPE_LOCALIZE_ENTRY, 14000);
		game::native::db_realloc_xasset_pool(game::native::XAssetType::ASSET_TYPE_XANIMPARTS, 8192);
		game::native::db_realloc_xasset_pool(game::native::XAssetType::ASSET_TYPE_XMODEL, 5125);
		game::native::db_realloc_xasset_pool(game::native::XAssetType::ASSET_TYPE_PHYSPRESET, 128);
		game::native::db_realloc_xasset_pool(game::native::XAssetType::ASSET_TYPE_MENU, 1280);
		game::native::db_realloc_xasset_pool(game::native::XAssetType::ASSET_TYPE_MENULIST, 256);
		game::native::db_realloc_xasset_pool(game::native::XAssetType::ASSET_TYPE_MATERIAL, 8192);
		game::native::db_realloc_xasset_pool(game::native::XAssetType::ASSET_TYPE_WEAPON, 2400);
		game::native::db_realloc_xasset_pool(game::native::XAssetType::ASSET_TYPE_STRINGTABLE, 800);
		game::native::db_realloc_xasset_pool(game::native::XAssetType::ASSET_TYPE_GAMEWORLD_SP, 1);

		// increase hunkTotal from 10mb to 15mb
		utils::hook::set<BYTE>(0x55E091 + 8, 0xF0);

		// gmem from 128 to 512
		utils::hook::set<BYTE>(0x4F9C91 + 4, 0x20);
		//gmem prim pos ^
		utils::hook::set<BYTE>(0x4F9CBD + 9, 0x20);


		//hide punkbuster error message
		utils::hook::set<BYTE>(0x571DB8, 0xEB);
		utils::hook::set<BYTE>(0x571DD6, 0xEB);
		utils::hook::set<BYTE>(0x571DC3, 0xEB);

		utils::hook(0x468F19, R_BeginRegistration_stub, HOOK_JUMP).install()->quick();

		// Disable dvar cheat / write protection
		utils::hook(0x5659E3, disable_dvar_cheats_stub, HOOK_JUMP).install()->quick();
		utils::hook::nop(0x5659E7 + 1, 1);

		// *
		// FastFiles

		// rewritten the whole function
		utils::hook(0x5D4420, load_common_fast_files, HOOK_CALL).install()->quick();

		// ^ Com_StartHunkUsers Mid-hook (realloc files that were unloaded on map load)
		utils::hook::nop(0x4FABCF, 6);		
		utils::hook(0x4FABCF, com_start_hunk_users_stub, HOOK_JUMP).install()->quick();

		// *
		// IWDs

		// Remove Impure client (iwd) check 
		utils::hook::nop(0x556673, 30);

		utils::hook(0x558254, FS_MakeIWDsLocalized, HOOK_JUMP).install()->quick();

		command::add("loadzone", [](command::params params) // unload zone and load zone again
		{
			if (params.length() < 2)
			{
				game::native::Com_PrintMessage(0, "Usage :: loadzone <zoneName>\n", 0);
				return;
			}

			game::native::XZoneInfo info[2];
			std::string zone = params[1];

			// unload
			info[0].name = 0;
			info[0].allocFlags = game::native::XZONE_FLAGS::XZONE_ZERO;
			info[0].freeFlags = game::native::XZONE_FLAGS::XZONE_MOD;

			info[1].name = zone.data();
			info[1].allocFlags = game::native::XZONE_FLAGS::XZONE_MOD;
			info[1].freeFlags = game::native::XZONE_FLAGS::XZONE_ZERO;

			game::native::DB_LoadXAssets(info, 2, 1);
		});
	}
};



REGISTER_MODULE(common)