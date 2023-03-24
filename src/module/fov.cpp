#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include "game/game.hpp"

#include "console.hpp"

#include <utils/hook.hpp>

class fov final : public module
{
public:
	void post_load() override
	{		
		// Set dvar limit
		static const auto cg_fov_limit = 120.0f;
		utils::hook::set(0x438F7E, &cg_fov_limit);

		// Prevent value change via internal scripts
		utils::hook(0x448CC5, &set_server_command_dvar_stub, HOOK_CALL).install()->quick();
	}

private:
	static void set_server_command_dvar_stub(const char* dvar_name, const char* value)
	{
		const auto* dvar = game::native::Dvar_FindVar(dvar_name);
		if (dvar != nullptr && ((dvar->flags & game::native::saved) != 0))
		{
			console::info("Not allowing server to override saved dvar '%s'\n", dvar_name);
			return;
		}

		game::native::Dvar_SetFromStringByName(dvar_name, value, 0);
	}
};

REGISTER_MODULE(fov)
