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
	}
};

REGISTER_MODULE(fov)
