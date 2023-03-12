#pragma once

#define WEAK __declspec(selectany)

// *
// Version
#define YACC_BUILDVERSION_DATE __DATE__
#define YACC_BUILDSTRING "YACC :: build :: %s", __DATE__

// *
// Gui
#define GET_GGUI game::native::glob::gui
#define GGUI_READY game::native::glob::gui.imgui_initialized
#define GGUI_READY_DVARS game::native::glob::gui.dvars_initialized
#define GGUI_ANY_MENUS_OPEN game::native::glob::gui.any_menus_open

#define FONT_SMALL_DEV  "fonts/smallDevFont"
#define FONT_BIG_DEV    "fonts/bigDevFont"
#define FONT_CONSOLE    "fonts/consoleFont"
#define FONT_BIG        "fonts/bigFont"
#define FONT_SMALL      "fonts/smallFont"
#define FONT_BOLD       "fonts/boldFont"
#define FONT_NORMAL     "fonts/normalFont"
#define FONT_EXTRA_BIG  "fonts/extraBigFont"
#define FONT_OBJECTIVE  "fonts/objectiveFont"

#define HORIZONTAL_ALIGN_SUBLEFT		0	// left edge of a 4:3 screen (safe area not included)
#define HORIZONTAL_ALIGN_LEFT			1	// left viewable (safe area) edge
#define HORIZONTAL_ALIGN_CENTER			2	// center of the screen (reticle)
#define HORIZONTAL_ALIGN_RIGHT			3	// right viewable (safe area) edge
#define HORIZONTAL_ALIGN_FULLSCREEN		4	// disregards safe area
#define HORIZONTAL_ALIGN_NOSCALE		5	// uses exact parameters - neither adjusts for safe area nor scales for screen size
#define HORIZONTAL_ALIGN_TO640			6	// scales a real-screen resolution x down into the 0 - 640 range
#define HORIZONTAL_ALIGN_CENTER_SAFEAREA 7	// center of the safearea

#define VERTICAL_ALIGN_SUBTOP			0	// top edge of the 4:3 screen (safe area not included)
#define VERTICAL_ALIGN_TOP				1	// top viewable (safe area) edge
#define VERTICAL_ALIGN_CENTER			2	// center of the screen (reticle)
#define VERTICAL_ALIGN_BOTTOM			3	// bottom viewable (safe area) edge
#define VERTICAL_ALIGN_FULLSCREEN		4	// disregards safe area
#define VERTICAL_ALIGN_NOSCALE			5	// uses exact parameters - neither adjusts for safe area nor scales for screen size
#define VERTICAL_ALIGN_TO480			6	// scales a real-screen resolution y down into the 0 - 480 range
#define VERTICAL_ALIGN_CENTER_SAFEAREA	7	// center of the save area

#define HORIZONTAL_APPLY_LEFT				0
#define HORIZONTAL_APPLY_CENTER				1
#define HORIZONTAL_APPLY_RIGHT				2
#define HORIZONTAL_APPLY_FULLSCREEN			3
#define HORIZONTAL_APPLY_NONE				4
#define HORIZONTAL_APPLY_TO640				5
#define HORIZONTAL_APPLY_CENTER_SAFEAREA	6
#define HORIZONTAL_APPLY_CONSOLE_SPECIAL	7

#define VERTICAL_APPLY_TOP					0
#define VERTICAL_APPLY_CENTER				1
#define VERTICAL_APPLY_BOTTOM				2
#define VERTICAL_APPLY_FULLSCREEN			3
#define VERTICAL_APPLY_NONE					4
#define VERTICAL_APPLY_TO640				5
#define VERTICAL_APPLY_CENTER_SAFEAREA		6

namespace game
{
	namespace native
	{
		WEAK symbol < void()> Sys_ShowConsole{ 0x0, 0x574DC0 };
		WEAK symbol < void* (const char* message)> Conbuf_AppendText{ 0x0, 0x574E40 };
		WEAK symbol < void* (LocalClientNum_t localClientNum, int controllerIndex, const char* text)> Cmd_ExecuteSingleCommand{ 0x0, 0x4F4660 };
		WEAK symbol <dvar_t* (const char* dvarName)> Dvar_FindMalleableVar { 0x0, 0x565C80 };
		WEAK symbol <Font_s* (const char* fontName, int fontSize)> R_RegisterFont{ 0x0, 0x5D1B20 };
		WEAK symbol <void()> Sys_InitMainThread{ 0x0, 0x505D70 };
		WEAK symbol <bool()> Sys_IsMainThread{ 0x0, 0x506110 };
		WEAK symbol <dvar_t* (const char* dvar_name, dvar_type type_bool, std::uint16_t flags, const char* description, std::int32_t default_value, std::int32_t null1, std::int32_t null2, std::int32_t null3, std::int32_t null4, std::int32_t null5)> Dvar_RegisterBool_r{ 0x0, 0x5667F0 };
		WEAK symbol <dvar_t* (const char* dvar_name, dvar_type type_enum, std::uint16_t flags, const char* description, std::int32_t default_index, std::int32_t null1, std::int32_t null2, std::int32_t null3, std::int32_t enumSize, const char** enum_data)> Dvar_RegisterEnum_r{ 0x0, 0x5667F0 };
		WEAK symbol <dvar_t* (const char* dvar_name, float default_value, float min_value, float max_value, std::uint16_t flags, const char* description)> Dvar_RegisterFloat_r{ 0x0, 0x566B70 };

		// symbols
		WEAK symbol<ScreenPlacement> scrPlace {0x0, 0x42A0B3};
		WEAK symbol<UiContext> ui_context{0x0, 0xCAE6200};

	}
}