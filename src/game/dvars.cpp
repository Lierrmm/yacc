#include <std_include.hpp>
#include "game.hpp"
#include "dvars.hpp"

namespace dvars
{
	extern game::native::dvar_t* ui_ultrawide = nullptr;
	extern game::native::dvar_t* r_aspectRatio_custom = nullptr;
	extern game::native::dvar_t* r_aspectRatio = nullptr;
	

	// Console 
	extern game::native::dvar_t* con_minicon_position = nullptr;
	extern game::native::dvar_t* con_minicon_font = nullptr;
	extern game::native::dvar_t* con_minicon_fontHeight = nullptr;
	extern game::native::dvar_t* con_minicon_fontColor = nullptr;
	extern game::native::dvar_t* con_minicon_fontStyle = nullptr;

	extern game::native::dvar_t* yacc_con_fltCon = nullptr;
	extern game::native::dvar_t* yacc_con_fltConLeft = nullptr;
	extern game::native::dvar_t* yacc_con_fltConTop = nullptr;
	extern game::native::dvar_t* yacc_con_fltConRight = nullptr;
	extern game::native::dvar_t* yacc_con_fltConBottom = nullptr;

	extern game::native::dvar_t* yacc_con_outputHeight = nullptr;
	extern game::native::dvar_t* yacc_con_maxMatches = nullptr;
	extern game::native::dvar_t* yacc_con_useDepth = nullptr;
	extern game::native::dvar_t* yacc_con_fontSpacing = nullptr;
	extern game::native::dvar_t* yacc_con_padding = nullptr;

	extern game::native::dvar_t* yacc_con_cursorOverdraw = nullptr;
	extern game::native::dvar_t* yacc_con_cursorState = nullptr;

	extern game::native::dvar_t* yacc_con_hintBoxTxtColor_currentDvar = nullptr;
	extern game::native::dvar_t* yacc_con_hintBoxTxtColor_currentValue = nullptr;
	extern game::native::dvar_t* yacc_con_hintBoxTxtColor_defaultValue = nullptr;
	extern game::native::dvar_t* yacc_con_hintBoxTxtColor_dvarDescription = nullptr;
	extern game::native::dvar_t* yacc_con_hintBoxTxtColor_domainDescription = nullptr;

	void initialize()
	{
	}
}
