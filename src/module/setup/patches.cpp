#include <std_include.hpp>
#include <loader/module_loader.hpp>

#include "game/game.hpp"

#include "console.hpp"
#include "scheduler.hpp"
#include "version.hpp"

#include <module/gameplay/command.hpp>
#include <module/assets/localization.hpp>
#include <module/assets/ui_script.hpp>

#include <utils/concurrency.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/io.hpp>
#include <utils/_utils.hpp>

// on renderer initialization
void print_loaded_modules()
{
	// add FS Path output print
	if (const auto& dedicated = game::native::Dvar_FindVar("dedicated");
		dedicated && dedicated->current.integer == 0)
	{
		game::native::FS_DisplayPath(1);
	}
}

__declspec(naked) void CL_PreInitRenderer_stub()
{
	const static uint32_t CL_PreInitRenderer_func = 0x4690C0;
	const static uint32_t retn_addr = 0x46B25F;
	__asm
	{
		pushad;
		call	print_loaded_modules;
		popad;

		call	CL_PreInitRenderer_func;
		jmp		retn_addr;
	}
}


// helper function because cba to do that in asm
void print_build_on_init()
{
	game::native::Com_PrintMessage(0, "\n-------- Game Initialization ----------\n", 0);
	game::native::Com_PrintMessage(0, utils::string::va("> Build: YACC %s :: %s\n", VERSION, __TIMESTAMP__), 0);
}

void FS_ReplaceSeparators(char* path) {
	char* s;

	for (s = path; *s; s++) {
		if (*s == '/' || *s == '\\') {
			*s = '\\';
		}
	}
}

int __cdecl FileWrapper_GetFileSize(FILE* h)
{
	int startPos;
	int fileSize;

	startPos = ftell(h);
	fseek(h, 0, SEEK_END);
	fileSize = ftell(h);
	fseek(h, startPos, SEEK_SET);
	return fileSize;
}

int __cdecl FS_FileGetFileSize(FILE* file)
{
	return FileWrapper_GetFileSize(file);
}

int FS_FileLengthOSPathByName(const char* osPath) {

	FILE* fh = fopen(osPath, "rb");

	if (fh == NULL)
		return -1;

	int len = FS_FileGetFileSize(fh);

	fclose(fh);
	return len;

}

bool DB_FileExistsLoadscreen(const char* fileName)
{
	auto mapname = std::string(utils::string::va("%s", fileName));

	char* find = strstr(mapname.data(), "_load");
	if (find)
	{
		find[0] = '\0';
	}

	auto path = std::string(utils::string::va("%s/zone/%s/%s_load.ff", game::native::Sys_DefaultInstallPath(), game::native::Win_GetLanguage(), mapname.data()));

	FS_ReplaceSeparators(path.data());
	if (FS_FileLengthOSPathByName(path.data()) > 50)
	{
		return true;
	}

	path = std::string(utils::string::va("%s/usermaps/%s/%s_load.ff", game::native::Sys_DefaultInstallPath(), mapname.data(), mapname.data()));
	FS_ReplaceSeparators(path.data());
	if (FS_FileLengthOSPathByName(path.data()) > 50)
	{
		return true;
	}
	return false;
}

__declspec(naked) void LoadMapLoadscreen(const char* name)
{
	if (DB_FileExistsLoadscreen(name))
	{
		game::native::LoadMapLoadScreenInternal(name);
	}
}

//void copy_startup_files()
//{
//	const auto splashResource = FindResource(utils::nt::library(), MAKEINTRESOURCE(IMAGE_SPLASH), "Image");
//	if (!splashResource)
//	{
//#if DEBUG
//		MessageBoxA(nullptr, "Failed to find custom splashscreen!", "ERROR", MB_ICONERROR);
//#endif
//		return;
//	}
//
//	const auto splashHandle = LoadResource(nullptr, splashResource);
//	if (!splashHandle)
//	{
//#if DEBUG
//		MessageBoxA(nullptr, "Failed to load custom splashscreen!", "ERROR", MB_ICONERROR);
//#endif
//		return;
//	}
//
//	utils::io::write_file("yacc/images/splash.bmp", std::string(LPSTR(LockResource(splashHandle)), SizeofResource(nullptr, splashResource)));
//
//	const auto logoResource = FindResource(utils::nt::library(), MAKEINTRESOURCE(IMAGE_LOGO), "Image");
//	if (!logoResource)
//	{
//#if DEBUG
//		MessageBoxA(nullptr, "Failed to find custom logo!", "ERROR", MB_ICONERROR);
//#endif
//		return;
//	}
//
//	const auto logoHandle = LoadResource(nullptr, logoResource);
//	if (!logoHandle)
//	{
//#if DEBUG
//		MessageBoxA(nullptr, "Failed to load custom logo!", "ERROR", MB_ICONERROR);
//#endif
//		return;
//	}
//
//	utils::io::write_file("yacc/images/logo.bmp", std::string(LPSTR(LockResource(logoHandle)), SizeofResource(nullptr, logoResource)));
//
//	const auto introResource = FindResource(utils::nt::library(), MAKEINTRESOURCE(INTRO_VIDEO), RT_RCDATA);
//	if (!introResource)
//	{
//#if DEBUG
//		MessageBoxA(nullptr, "Failed to find custom intro!", "ERROR", MB_ICONERROR);
//#endif
//		return;
//	}
//
//	const auto introHandle = LoadResource(nullptr, introResource);
//	if (!introHandle)
//	{
//#if DEBUG
//		MessageBoxA(nullptr, "Failed to load custom intro!", "ERROR", MB_ICONERROR);
//#endif
//		return;
//	}
//
//	utils::io::write_file("yacc/video/IW_logo.bik", std::string(LPSTR(LockResource(introHandle)), SizeofResource(nullptr, introResource)));
//
//	// yacc.iwd
//	const auto yaccIwdResource = FindResource(utils::nt::library(), MAKEINTRESOURCE(YACC_IWD), RT_RCDATA);
//	if (!yaccIwdResource)
//	{
//#if DEBUG
//		MessageBoxA(nullptr, "Failed to find custom yacc.iwd!", "ERROR", MB_ICONERROR);
//#endif
//		return;
//	}
//
//	const auto yaccIwdHandle = LoadResource(nullptr, yaccIwdResource);
//	if (!yaccIwdHandle)
//	{
//#if DEBUG
//		MessageBoxA(nullptr, "Failed to load custom yacc.iwd!", "ERROR", MB_ICONERROR);
//#endif
//		return;
//	}
//
//	utils::io::write_file("yacc/yacc.iwd", std::string(LPSTR(LockResource(yaccIwdHandle)), SizeofResource(nullptr, yaccIwdResource)));
//}

class patches final : public module
{
public:
	void post_start() override
	{
		//copy_startup_files();
	}

	void post_load() override
	{
		/* Overrides error in CG_Init() about bad client/server game */
		utils::hook::nop(0x43D40C, 5);

		// Force debug logging
		utils::hook::nop(0x4F772C, 8);

		// Enable console log
		utils::hook::nop(0x4F7732, 2);

		// Ignore hardware changes
		utils::hook::set<DWORD>(0x570E7B, 0xC359C032);
		utils::hook::set<DWORD>(0x570DA1, 0xC359C032);

		// Remove improper quit popup
		utils::hook::set<BYTE>(0x571AF6, 0xEB);

		// Fix microphone shit
		utils::hook::set<BYTE>(0x575145, 0xEB);
		utils::hook::nop(0x4E7F26, 5);

		// Disable AutoUpdate Check?
		utils::hook::nop(0x4D217C, 5);

		// Disable developer check for Alt + Enter
		utils::hook::nop(0x57650E, 2);

		// Precaching beyond level load
		utils::hook::nop(0x4DCB06, 2); // model 1
		utils::hook::set<BYTE>(0x4DCB72, 0xEB); // model 2

		// Disable PC changed message
		utils::hook::nop(0x570E85, 2);

		// Disable CD Key Checks
		utils::hook::set(0x46C830, 0x90C301B0);

		// Print loaded modules to console
		utils::hook(0x46B25A, CL_PreInitRenderer_stub, HOOK_JUMP).install()->quick();

		// Remove "setstat: developer_script must be false"
		utils::hook::nop(0x46B255, 5);

		utils::hook::nop(0x4BA29B, 5); // gamename
		utils::hook::nop(0x4BA2AC, 5); // gamedate

		// *
		// Fix fps on windows 10 (stuck at 500) :: sleep(1) to sleep(0)

		utils::hook::set<BYTE>(0x4FA9D4 + 1, 0x0);
		utils::hook::set<BYTE>(0x4FAA3F + 1, 0x0);


		command::add("menu_list", "", "print all loaded menus to the console", [this](command::params)
		{
			if (!game::native::ui_context || !*game::native::ui_context->Menus)
			{
				return;
			}

			game::native::Com_PrintMessage(0, "^2menu list:\n", 0);

			for (int i = 0; i < game::native::ui_context->menuCount; i++)
			{
				game::native::Com_PrintMessage(0, utils::string::va("|> %s \n", game::native::ui_context->Menus[i]->window.name), 0);
			}

			// # in-game menus

			if (const auto& cl_ingame = game::native::Dvar_FindVar("cl_ingame"); cl_ingame)
			{
				if (cl_ingame->current.enabled && game::native::ui_cg_dc)
				{
					game::native::Com_PrintMessage(0, "^2in-game menu list:\n", 0);

					for (int i = 0; i < game::native::ui_cg_dc->menuCount; i++)
					{
						game::native::Com_PrintMessage(0, utils::string::va("|> %s \n", game::native::ui_cg_dc->Menus[i]->window.name), 0);
					}
				}
			}
		});

		utils::hook::set<uint8_t>(0x4643A2, 0xEB);

		// remove MAX_PACKET_CMDs com_error
		utils::hook::nop(0x4603D3, 5);

		// fs_basegame
		utils::hook::set<const char*>(0x558B78, BASEGAME);

		utils::hook::set(0x5D45D0, "YACC (" VERSION ") - COD4 1.8");
		utils::hook::set(0x574BF7, "YACC (" VERSION ") - Console");

		//sv_hostname
		utils::hook::set(0x52AC3A, "YACCHost");

		utils::hook::set(0x573774, BASEGAME "/images/splash.bmp");
		utils::hook::set(0x574C7A, BASEGAME "/images/logo.bmp");

		// increase font sizes for chat on higher resolutions
		static float float13 = 13.0f;
		static float float10 = 10.0f;
		utils::hook::set<float*>(0x42B6DB, &float13);
		utils::hook::set<float*>(0x42B6F5, &float10);

		// remove limit on IWD file loading
		utils::hook::set<BYTE>(0x558268, 0xEB);

		// allow joining 'developer 1' servers
		utils::hook::set<BYTE>(0x4A6343, 0xEB);

		// disable atvi and codintro videos
		utils::hook::nop(0x4F9602, 5);

		#ifdef USING_CUSTOM_MASTER
			// change MasterServer IP Address to our custom 
			DWORD masterserver_patches [] =
			{
				0x465F64,
				0x465FCF,
				0x46CDDB,
				0x4F977B,
				0x4F97BE,
				0x5242BB,
				0x5242D4,
				0x524341,
				0x52E08E,
				0x52E0A7,
				0x52E0B6,
				0x52E130,
				0x52E184,
				0x52E1FC
			};

			for (int i = 0; i < ARRAYSIZE(masterserver_patches); ++i)
			{
				utils::hook::set<const char*>(masterserver_patches[i] + 1, CUSTOM_MASTER_IP);
			}

			
			//[client] change MasterServer port to our custom target
			utils::hook::set<int>(0x46CE16, CUSTOM_MASTER_PORT);
			//[server] change MasterServer port to our custom target for HeartBeat packet
			utils::hook::set<int>(0x52E0E5, CUSTOM_MASTER_PORT);

			DWORD masterserver_heartbeat_patches[] =
			{
				0x52DF7E,
				0x52DFB8
			};

			for (int i = 0; i < ARRAYSIZE(masterserver_heartbeat_patches); ++i)
			{
				utils::hook::set<const char*>(masterserver_heartbeat_patches[i] + 1, "IW3 7");
			}


			utils::hook::set<const char*>(0x5452C6, "globalservers 1 IW3 7 full empty");
			utils::hook::set<const char*>(0x5452D1, "globalservers 1 IW3 7 full empty");
		#endif
		

		// make intro unskippable if in release
#ifndef DEBUG
		utils::hook::set<const char*>(0x4F95EC, "unskippablecinematic IW_logo\n");
#endif // !DEBUG

		utils::hook::set<const char*>(0x5EC9FC, "%s\\" BASEGAME "\\video\\%s.%s");

		utils::hook::nop(0x57616C, 8);
		scheduler::on_frame([]()
		{
			SetThreadExecutionState(ES_DISPLAY_REQUIRED);
		});

		// stop connect menu showing mapname via engine
		utils::hook::nop(0x544D10, 5);   // GAMETYPE
		utils::hook::nop(0x544D56, 5);   // MAPNAME
		// These handle connecting to server 
		//utils::hook::nop(0x544BD1, 5);
		//utils::hook::nop(0x544F58, 5);
		//utils::hook::nop(0x544F84, 5);
		utils::hook::nop(0x544FF0, 5);   // MODNAME

		// Set UI_SHOWLIST font to normalFont so it's actually readable...
		utils::hook::set(0x54F10F, 0xCAE8708);

		//scripting demo - move later

		UIScript::Add("visitWebsite", [](UIScript::Token)
		{
			utils::_utils::OpenUrl("https://yacc.app");
		});
	}

	module_priority priority() const override
	{
		return module_priority::patches;
	}
};



REGISTER_MODULE(patches);