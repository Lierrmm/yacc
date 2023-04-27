#pragma once

namespace dvars
{
	extern game::native::dvar_t* ui_ultrawide;
	extern game::native::dvar_t* r_aspectRatio_custom;
	extern game::native::dvar_t* r_aspectRatio;

	extern game::native::dvar_t* con_minicon_position;
	extern game::native::dvar_t* con_minicon_font;
	extern game::native::dvar_t* con_minicon_fontHeight;
	extern game::native::dvar_t* con_minicon_fontColor;
	extern game::native::dvar_t* con_minicon_fontStyle;

	extern game::native::dvar_t* yacc_con_fltCon;
	extern game::native::dvar_t* yacc_con_fltConLeft;
	extern game::native::dvar_t* yacc_con_fltConTop;
	extern game::native::dvar_t* yacc_con_fltConRight;
	extern game::native::dvar_t* yacc_con_fltConBottom;

	extern game::native::dvar_t* yacc_con_outputHeight;
	extern game::native::dvar_t* yacc_con_maxMatches;
	extern game::native::dvar_t* yacc_con_useDepth;
	extern game::native::dvar_t* yacc_con_fontSpacing;
	extern game::native::dvar_t* yacc_con_padding;

	extern game::native::dvar_t* yacc_con_cursorOverdraw;
	extern game::native::dvar_t* yacc_con_cursorState;

	extern game::native::dvar_t* yacc_con_hintBoxTxtColor_currentDvar;
	extern game::native::dvar_t* yacc_con_hintBoxTxtColor_currentValue;
	extern game::native::dvar_t* yacc_con_hintBoxTxtColor_defaultValue;
	extern game::native::dvar_t* yacc_con_hintBoxTxtColor_dvarDescription;
	extern game::native::dvar_t* yacc_con_hintBoxTxtColor_domainDescription;
	void initialize();
}
