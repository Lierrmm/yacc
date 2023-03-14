#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include "game/game.hpp"

#include <utils/hook.hpp>

#include "bullet.hpp"

const game::native::dvar_t* bullet::bg_bulletRange;

DWORD bullet::bullet_fire_addr;

__declspec(naked) void bullet::bullet_fire_stub()
{
	__asm
	{
		push eax
		mov eax, bg_bulletRange
		fld dword ptr [eax + 0xC] //dvar_t.current
		pop eax

		jmp bullet_fire_addr
	}
}

void bullet::post_load()
{
	//bg_bulletRange = game::native::Dvar_RegisterFloat("bg_bulletRange", "Max range used when calculating the bullet end position", 8192.0f, 0.0f, std::numeric_limits<float>::max(), game::native::server_info);

	bullet_fire_addr = SELECT_VALUE(0x0, 0x4A28FC);
	utils::hook(SELECT_VALUE(0x0, 0x4A28F6), &bullet_fire_stub, HOOK_JUMP).install()->quick();
}

//REGISTER_MODULE(bullet)
