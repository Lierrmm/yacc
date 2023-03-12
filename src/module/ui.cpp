#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include "game/game.hpp"
#include "console.hpp"
#include "scheduler.hpp"
#include "version.hpp"

#include <utils/concurrency.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <game/dvars.hpp>

#define MAINMENU_INIT_FADE_TIME 50.0f

void scrplace_apply_rect(float* offs_x, float* w, float* offs_y, float* h, int horz_anker, int vert_anker)
{
	float _x, _w, _y, _h;

	switch (horz_anker)
	{
	case HORIZONTAL_ALIGN_SUBLEFT:
		_x = game::native::scrPlace->scaleVirtualToReal[0] * *offs_x + game::native::scrPlace->subScreenLeft;
		_w = game::native::scrPlace->scaleVirtualToReal[0] * *w;
		break;

	case HORIZONTAL_ALIGN_LEFT:
		_x = game::native::scrPlace->scaleVirtualToReal[0] * *offs_x + game::native::scrPlace->realViewableMin[0];
		_w = game::native::scrPlace->scaleVirtualToReal[0] * *w;
		break;

	case HORIZONTAL_ALIGN_CENTER:
		_x = game::native::scrPlace->scaleVirtualToReal[0] * *offs_x + game::native::scrPlace->realViewportSize[0] * 0.5f;
		_w = game::native::scrPlace->scaleVirtualToReal[0] * *w;
		break;

	case HORIZONTAL_ALIGN_RIGHT:
		_x = game::native::scrPlace->scaleVirtualToReal[0] * *offs_x + game::native::scrPlace->realViewableMax[0];
		_w = game::native::scrPlace->scaleVirtualToReal[0] * *w;
		break;

	case HORIZONTAL_ALIGN_FULLSCREEN:
		_x = game::native::scrPlace->scaleVirtualToFull[0] * *offs_x;
		_w = game::native::scrPlace->scaleVirtualToFull[0] * *w;
		break;

	case HORIZONTAL_ALIGN_NOSCALE:
		goto USE_VERT_ALIGN; // we might wan't vertical alignment

	case HORIZONTAL_ALIGN_TO640:
		_x = game::native::scrPlace->scaleRealToVirtual[0] * *offs_x;
		_w = game::native::scrPlace->scaleRealToVirtual[0] * *w;
		break;

	case HORIZONTAL_ALIGN_CENTER_SAFEAREA:
		_x = (game::native::scrPlace->realViewableMax[0] + game::native::scrPlace->realViewableMin[0]) * 0.5f + game::native::scrPlace->scaleVirtualToReal[0] * *offs_x;
		_w = game::native::scrPlace->scaleVirtualToReal[0] * *w;
		break;

	default:
		goto USE_VERT_ALIGN; // we might wan't vertical alignment

	}

	*offs_x = _x;
	*w = _w;

USE_VERT_ALIGN:
	switch (vert_anker)
	{
	case VERTICAL_ALIGN_TOP:
		_y = game::native::scrPlace->scaleVirtualToReal[1] * *offs_y + game::native::scrPlace->realViewableMin[1];
		_h = game::native::scrPlace->scaleVirtualToReal[1] * *h;
		break;

	case VERTICAL_ALIGN_CENTER:
		_y = game::native::scrPlace->scaleVirtualToReal[1] * *offs_y + game::native::scrPlace->realViewportSize[1] * 0.5f;
		_h = game::native::scrPlace->scaleVirtualToReal[1] * *h;
		break;

	case VERTICAL_ALIGN_BOTTOM:
		_y = game::native::scrPlace->scaleVirtualToReal[1] * *offs_y + game::native::scrPlace->realViewableMax[1];
		_h = game::native::scrPlace->scaleVirtualToReal[1] * *h;
		break;

	case VERTICAL_ALIGN_FULLSCREEN:
		_y = game::native::scrPlace->scaleVirtualToFull[1] * *offs_y;
		_h = game::native::scrPlace->scaleVirtualToFull[1] * *h;
		break;

	case VERTICAL_ALIGN_NOSCALE:
		return;

	case VERTICAL_ALIGN_TO480:
		_y = game::native::scrPlace->scaleRealToVirtual[1] * *offs_y;
		_h = game::native::scrPlace->scaleRealToVirtual[1] * *h;
		break;

	case VERTICAL_ALIGN_CENTER_SAFEAREA:
		_y = game::native::scrPlace->scaleVirtualToReal[1] * *offs_y + (game::native::scrPlace->realViewableMax[1] + game::native::scrPlace->realViewableMin[1]) * 0.5f;
		_h = game::native::scrPlace->scaleVirtualToReal[1] * *h;
		break;

	case VERTICAL_ALIGN_SUBTOP:
		_y = game::native::scrPlace->scaleVirtualToReal[1] * *offs_y;
		_h = game::native::scrPlace->scaleVirtualToReal[1] * *h;
		break;

	default:
		return;
	}

	*offs_y = _y;
	*h = _h;
}

float mainmenu_fade_time = MAINMENU_INIT_FADE_TIME;
float mainmenu_fade_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
void main_menu_fade_in()
{
	if (!game::native::Sys_IsMainThread() || game::native::glob::mainmenu_fade_done || !game::native::ui_context)
	{
		return;
	}

	if (mainmenu_fade_time >= 0.0f)
	{
		mainmenu_fade_time -= 0.5f;
		mainmenu_fade_color[3] = (mainmenu_fade_time * (1.0f / MAINMENU_INIT_FADE_TIME));

		game::native::ConDraw_Box(mainmenu_fade_color, 0.0f, 0.0f,
			static_cast<float>(game::native::ui_context->screenWidth),
			static_cast<float>(game::native::ui_context->screenHeight));

		return;
	}

	// reset fade vars
	mainmenu_fade_time = MAINMENU_INIT_FADE_TIME;
	mainmenu_fade_color[3] = 1.0f;

	game::native::glob::mainmenu_fade_done = true;
}
// *
// draw additional stuff to the main menu
void main_menu()
{
	if (!game::native::Sys_IsMainThread())
	{
		return;
	}

	const char* font;
	const auto max = game::native::scrPlace->scaleVirtualToReal[1] * 0.3f;

	const auto ui_smallFont = game::native::Dvar_FindVar("ui_smallFont");
	const auto ui_extraBigFont = game::native::Dvar_FindVar("ui_extraBigFont");
	const auto ui_bigFont = game::native::Dvar_FindVar("ui_bigFont");

	if (ui_smallFont && ui_smallFont->current.value < max)
	{
		if (ui_extraBigFont && ui_extraBigFont->current.value > max)
		{
			font = FONT_BIG;

			if (ui_bigFont && ui_bigFont->current.value > max)
			{
				font = FONT_NORMAL;
			}
		}
		else
		{
			font = FONT_EXTRA_BIG;
		}
	}
	else
	{
		font = FONT_SMALL;
	}

	const auto font_handle = game::native::R_RegisterFont(font, sizeof(font));
	if (!font_handle)
	{
		return;
	}

	float offs_x = 10.0f;
	float offs_y = -10.0f;
	const float scale = 0.25f;

	float scale_x = scale * 48.0f / static_cast<float>(font_handle->pixelHeight);
	float scale_y = scale_x;

	// place container
	scrplace_apply_rect(&offs_x, &scale_x, &offs_y, &scale_y, HORIZONTAL_ALIGN_LEFT, VERTICAL_ALIGN_BOTTOM);

	const char* text_foreground = utils::string::va("YACC :: %s :: %s", VERSION, YACC_BUILDVERSION_DATE);
	const char* text_background = text_foreground;

	if (DEBUG)
	{
		text_foreground = utils::string::va("YACC :: %s :: %s :: %s", VERSION, YACC_BUILDVERSION_DATE, "^1DEBUG");
		text_background = utils::string::va("YACC :: %s :: %s :: %s", VERSION, YACC_BUILDVERSION_DATE, "DEBUG");
	}

	// Background String
	const float color_background[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

	game::native::R_AddCmdDrawTextASM(
		/* txt */ text_background,
		/* max */ 0x7FFFFFFF,
		/* fot */ font_handle,
		/*  x  */ offs_x + 3.0f,
		/*  y  */ offs_y + 3.0f,
		/* scX */ scale_x,
		/* scY */ scale_y,
		/* rot */ 0.0f,
		/* col */ color_background,
		/* sty */ 0);

	// Foreground String
	const float color_foreground[4] = { 1.0f, 0.8f, 0.7f, 1.0f };

	game::native::R_AddCmdDrawTextASM(
		/* txt */ text_foreground,
		/* max */ 0x7FFFFFFF,
		/* fot */ font_handle,
		/*  x  */ offs_x,
		/*  y  */ offs_y,
		/* scX */ scale_x,
		/* scY */ scale_y,
		/* rot */ 0.0f,
		/* col */ color_foreground,
		/* sty */ 0);

	// fade in the menu on first start
	main_menu_fade_in();


	// *
	// changelog gui

	game::native::glob::loaded_main_menu = true;
}

// Main Menu Version (UI_VersionNumber Call in UI_Refresh)
__declspec(naked) void main_menu_stub()
{
	__asm
	{
		call	main_menu;
		push	0x53DC3A;
		retn;
	}
}

/* ---------------------------------------------------------- */
	/* ---------------------- aspect ratio ---------------------- */

void set_ultrawide_dvar(bool state)
{
	if (dvars::ui_ultrawide)
	{
		game::native::dvar_set_value_dirty(dvars::ui_ultrawide, state);
	}
}

void set_custom_aspect_ratio()
{
	if (dvars::r_aspectRatio_custom)
	{
		*(float*)(0xCC9D0F8) = dvars::r_aspectRatio_custom->current.value;
	}
}

// hook R_AspectRatio to initially reset the ultrawide dvar (menu helper)
__declspec(naked) void aspect_ratio_custom_reset_stub()
{
	const static uint32_t retn_addr = 0x5D3194;
	__asm
	{
		pushad;
		push	0;
		call	set_ultrawide_dvar;
		add		esp, 4h;
		popad;

		mov     eax, [eax + 0Ch];	// overwritten op
		cmp     eax, 3;				// overwritten op

		jmp		retn_addr;			// jump back to break op
	}
}

// set custom aspect ratio by using the default switchcase in R_AspectRatio
__declspec(naked) void aspect_ratio_custom_stub()
{
	const static uint32_t retn_addr = 0x5D3245;
	__asm
	{
		pushad;
		push	1;
		call	set_ultrawide_dvar;
		add		esp, 4h;

		Call	set_custom_aspect_ratio;
		popad;

		mov     ecx, 1;				// widescreen true
		jmp		retn_addr;			// jump back to break op
	}
}

class ui final : public module
{
public:
	void post_start() override
	{

	}

	void post_load() override
	{
		// Main Menu Version (UI_VersionNumber Call in UI_Refresh)
		utils::hook(0x53DC35, main_menu_stub, HOOK_JUMP).install()->quick();


		// disable drawing of upper arrow :: nop "UI_DrawHandlePic" call
		utils::hook::nop(0x54D937, 5);

		// disable drawing of lower arrow :: nop "UI_DrawHandlePic" call
		utils::hook::nop(0x54D9F6, 5);


		// *
		// Display

		// hook R_AspectRatio to initially reset the ultrawide dvar (menu helper)
		utils::hook::nop(0x5D318E, 6);		utils::hook(0x5D318E, aspect_ratio_custom_reset_stub, HOOK_JUMP).install()->quick();

		// Set custom aspect ratio by using the default switchcase in R_AspectRatio
		utils::hook::nop(0x5D325B, 6);		utils::hook(0x5D325B, aspect_ratio_custom_stub, HOOK_JUMP).install()->quick();

		dvars::r_aspectRatio_custom = game::native::Dvar_RegisterFloat(
			/* name		*/ "r_aspectRatio_custom",
			/* desc		*/ "description",
			/* default	*/ 2.3333333f,
			/* minVal	*/ 0.1f,
			/* maxVal	*/ 10.0f,
			/* flags	*/ game::native::dvar_flags::saved);

		static std::vector <const char*> r_customAspectratio =
		{
			"auto",
			"4:3",
			"16:10",
			"16:9",
			"custom",
		};

		dvars::r_aspectRatio = game::native::Dvar_RegisterEnum(
			/* name		*/ "r_aspectRatio",
			/* desc		*/ "Screen aspect ratio. Use \"custom\" and \"r_aspectRatio_custom\" if your aspect ratio is not natively supported! (21/9 = 2.3333)",
			/* default	*/ 0,
			/* enumSize	*/ r_customAspectratio.size(),
			/* enumData */ r_customAspectratio.data(),
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::ui_ultrawide = game::native::Dvar_RegisterBool(
			/* name		*/ "ui_ultrawide",
			/* desc		*/ "menu helper",
			/* default	*/ false,
			/* flags	*/ game::native::dvar_flags::read_only);
	}
};



REGISTER_MODULE(ui)