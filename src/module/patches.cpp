#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include "game/game.hpp"
#include "console.hpp"
#include "scheduler.hpp"

#include <utils/concurrency.hpp>
#include <utils/hook.hpp>

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

class patches final : public module
{
public:
	void post_start() override
	{

	}

	void post_load() override
	{
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
	}
};



REGISTER_MODULE(patches)