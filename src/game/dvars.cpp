#include <std_include.hpp>
#include "game.hpp"
#include "dvars.hpp"

namespace dvars
{
	extern game::native::dvar_t* ui_ultrawide = nullptr;
	extern game::native::dvar_t* r_aspectRatio_custom = nullptr;
	extern game::native::dvar_t* r_aspectRatio = nullptr;

	void initialize()
	{
		/*com_sv_running = reinterpret_cast<const game::native::dvar_t**>(SELECT_VALUE(0x1769F50, 0x1CEF588));

		sv_maxclients = reinterpret_cast<const game::native::dvar_t**>(SELECT_VALUE(0x0, 0x21223C0));
		sv_g_gametype = reinterpret_cast<const game::native::dvar_t**>(SELECT_VALUE(0x0, 0x21223F8));
		sv_mapname = reinterpret_cast<const game::native::dvar_t**>(SELECT_VALUE(0x47D7C6, 0x58D4784));

		loc_language = reinterpret_cast<const game::native::dvar_t**>(SELECT_VALUE(0x1BF6938, 0x58D5A90));*/
	}
}
