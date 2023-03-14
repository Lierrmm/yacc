#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include "game/game.hpp"
#include "console.hpp"
#include "scheduler.hpp"

#include "version.hpp"

#include <utils/concurrency.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>
#include "command.hpp"

void force_dvars_on_init()
{
	if (const auto dedicated = game::native::Dvar_FindVar("dedicated"); dedicated && !dedicated->current.integer)
	{
		const auto sv_pure = game::native::Dvar_FindVar("sv_pure");
		const auto sv_punkbuster = game::native::Dvar_FindVar("sv_punkbuster");
		const auto r_zFeather = game::native::Dvar_FindVar("r_zFeather");
		const auto r_distortion = game::native::Dvar_FindVar("r_distortion");
		const auto r_fastSkin = game::native::Dvar_FindVar("r_fastSkin");

		if (sv_pure && sv_pure->current.enabled)
		{
			game::native::dvar_set_value_dirty(sv_pure, false); // quick set the value
			game::native::Cmd_ExecuteSingleCommand((game::native::LocalClientNum_t)0, 0, "sv_pure 0\n");
		}

		if (sv_punkbuster && sv_punkbuster->current.enabled)
		{
			game::native::dvar_set_value_dirty(sv_punkbuster, false); // quick set the value
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

// on renderer initialization
void print_loaded_modules()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);


	game::native::Com_PrintMessage(0, utils::string::va("-------------- Loaded Modules -------------- \n%s\n", game::native::glob::loaded_modules.c_str()), 0);

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

// r_init
__declspec(naked) void console_printfix_stub_01()
{
	const static uint32_t retn_addr = 0x5D4AF6;
	const static char* print = "\n-------------- R_Init --------------\n";
	__asm
	{
		push	print;
		jmp		retn_addr;
	}
}

// working directory
__declspec(naked) void console_printfix_stub_02()
{
	const static uint32_t retn_addr = 0x571DA2;
	const static char* print = "Working directory: %s\n\n";
	__asm
	{
		push	print;
		jmp		retn_addr;
	}
}

// server initialization
__declspec(naked) void console_printfix_stub_03()
{
	const static uint32_t retn_addr = 0x52A023;
	const static char* print = "\n------- Server Initialization ---------\n";
	__asm
	{
		push	print;
		jmp		retn_addr;
	}
}

// helper function because cba to do that in asm
void print_build_on_init()
{
	game::native::Com_PrintMessage(0, "\n-------- Game Initialization ----------\n", 0);
	game::native::Com_PrintMessage(0, utils::string::va("> Build: YACC %s :: %s\n", VERSION, __TIMESTAMP__), 0);
}

// game init + game name and version
__declspec(naked) void console_printfix_stub_04()
{
	const static uint32_t retn_addr = 0x4BA28F;
	__asm
	{
		pushad;
		call	print_build_on_init;
		popad;

		jmp		retn_addr;
	}
}

const char* Con_LinePrefix()
{
	return ">";
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


class patches final : public module
{
public:
	void post_start() override
	{

	}

	void post_load() override
	{
		/* Overrides error in CG_Init() about bad client/server game */
		utils::hook::nop(0x43D40C, 5);

		// increase hunkTotal from 10mb to 15mb
		utils::hook::set<BYTE>(0x55E091 + 8, 0xF0);

		// gmem from 128 to 512
		utils::hook::set<BYTE>(0x4F9C91 + 4, 0x20);
		//gmem prim pos ^
		utils::hook::set<BYTE>(0x4F9CBD + 9, 0x20);


		//hide punkbuster error message
		utils::hook::nop(0x571DCA, 5);
		utils::hook::nop(0x571DEC, 5);

		utils::hook(0x468F19, R_BeginRegistration_stub, HOOK_JUMP).install()->quick();

		// Disable dvar cheat / write protection
		utils::hook(0x5659E3, disable_dvar_cheats_stub, HOOK_JUMP).install()->quick();
		utils::hook::nop(0x5659E7 + 1, 1);

		// Remove Impure client (iwd) check 
		utils::hook::nop(0x556673, 30);

		utils::hook(0x558254, FS_MakeIWDsLocalized, HOOK_JUMP).install()->quick();


		// More
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

		// (c) Snake :: Fix mouse lag by moving SetThreadExecutionState call out of event loop
		utils::hook::nop(0x57616C, 8);
		/*scheduler::on_frame([]()
		{
			SetThreadExecutionState(ES_DISPLAY_REQUIRED);
		});*/


		// *
		// Console prints

		// Print loaded modules to console
		utils::hook(0x46B25A, CL_PreInitRenderer_stub, HOOK_JUMP).install()->quick();

		// Remove "setstat: developer_script must be false"
		utils::hook::nop(0x46B255, 5);

		// Add newlines 
		utils::hook(0x5D4AF1, console_printfix_stub_01, HOOK_JUMP).install()->quick();
		utils::hook(0x571D9D, console_printfix_stub_02, HOOK_JUMP).install()->quick();
		utils::hook(0x52A01E, console_printfix_stub_03, HOOK_JUMP).install()->quick();
		utils::hook(0x4BA28A, console_printfix_stub_04, HOOK_JUMP).install()->quick();
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

		utils::hook(0x45C8F5, Con_LinePrefix, HOOK_CALL).install()->quick();

		//utils::hook::nop(0x429719, 1033);
		//utils::hook(0x429719, CG_InitConsoleCommandsPatched, HOOK_CALL).install()->quick();
		//utils::hook(0x46BB6C, LoadMapLoadscreen, HOOK_CALL).install()->quick();

		// remove MAX_PACKET_CMDs com_error
		utils::hook::nop(0x4603D3, 5);
	}
};



REGISTER_MODULE(patches)