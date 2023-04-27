#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include "game/game.hpp"

#include "_ui.hpp"

namespace components
{
	float _ui::scrplace_apply_x(int horz_align, const float x, const float x_offs)
	{
		float result;
		switch (horz_align)
		{
		case HORIZONTAL_APPLY_LEFT:
			result = game::native::scrPlace->scaleVirtualToReal[0] * x + x_offs + game::native::scrPlace->realViewableMin[0];
			break;

		case HORIZONTAL_APPLY_CENTER:
			result = game::native::scrPlace->scaleVirtualToReal[0] * x + x_offs + game::native::scrPlace->realViewportSize[0] * 0.5f;
			break;

		case HORIZONTAL_APPLY_RIGHT:
			result = game::native::scrPlace->scaleVirtualToReal[0] * x + x_offs + game::native::scrPlace->realViewableMax[0];
			break;

		case HORIZONTAL_APPLY_FULLSCREEN:
			result = game::native::scrPlace->scaleVirtualToFull[0] * x + x_offs;
			break;

		case HORIZONTAL_APPLY_NONE:
			result = x;
			break;

		case HORIZONTAL_APPLY_TO640:
			result = game::native::scrPlace->scaleRealToVirtual[0] * x + x_offs;
			break;

		case HORIZONTAL_APPLY_CENTER_SAFEAREA:
			result = (game::native::scrPlace->realViewableMax[0] + game::native::scrPlace->realViewableMin[0]) * 0.5f + x_offs + game::native::scrPlace->scaleVirtualToReal[0] * x;
			break;

		case HORIZONTAL_APPLY_CONSOLE_SPECIAL:
			result = game::native::scrPlace->realViewableMax[0] - x_offs + game::native::scrPlace->scaleVirtualToReal[0] * x;
			break;

		default:
			result = x;
		}

		return result;
	}

	float _ui::scrplace_apply_y(int vert_align, const float y, const float y_offs)
	{
		float result;
		switch (vert_align)
		{
		case VERTICAL_APPLY_TOP:
			result = game::native::scrPlace->scaleVirtualToReal[1] * y + y_offs + game::native::scrPlace->realViewableMin[1];
			break;

		case VERTICAL_APPLY_CENTER:
			result = game::native::scrPlace->scaleVirtualToReal[1] * y + y_offs + game::native::scrPlace->realViewportSize[1] * 0.5f;
			break;

		case VERTICAL_APPLY_BOTTOM:
			result = game::native::scrPlace->scaleVirtualToReal[1] * y + y_offs + game::native::scrPlace->realViewableMax[1];
			break;

		case VERTICAL_APPLY_FULLSCREEN:
			result = game::native::scrPlace->scaleVirtualToFull[1] * y + y_offs;
			break;

		case VERTICAL_APPLY_NONE:
			result = y;
			break;

		case VERTICAL_APPLY_TO640:
			result = game::native::scrPlace->scaleRealToVirtual[1] * y + y_offs;
			break;

		case VERTICAL_APPLY_CENTER_SAFEAREA:
			result = (game::native::scrPlace->realViewableMax[1] + game::native::scrPlace->realViewableMin[1]) * 0.5f + y_offs + game::native::scrPlace->scaleVirtualToReal[1] * y;
			break;

		default:
			result = y;
		}

		return result;
	}

	void _ui::scrplace_apply_rect(float* offs_x, float* w, float* offs_y, float* h, int horz_anker, int vert_anker)
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

	void _ui::scrplacefull_apply_rect(float* offs_x, float* w, float* offs_y, float* h, int horz_anker, int vert_anker)
	{
		float _x, _w, _y, _h;

		switch (horz_anker)
		{
		case HORIZONTAL_ALIGN_SUBLEFT:
			_x = game::native::scrPlaceFull->scaleVirtualToReal[0] * *offs_x + game::native::scrPlaceFull->subScreenLeft;
			_w = game::native::scrPlaceFull->scaleVirtualToReal[0] * *w;
			break;

		case HORIZONTAL_ALIGN_LEFT:
			_x = game::native::scrPlaceFull->scaleVirtualToReal[0] * *offs_x + game::native::scrPlaceFull->realViewableMin[0];
			_w = game::native::scrPlaceFull->scaleVirtualToReal[0] * *w;
			break;

		case HORIZONTAL_ALIGN_CENTER:
			_x = game::native::scrPlaceFull->scaleVirtualToReal[0] * *offs_x + game::native::scrPlaceFull->realViewportSize[0] * 0.5f;
			_w = game::native::scrPlaceFull->scaleVirtualToReal[0] * *w;
			break;

		case HORIZONTAL_ALIGN_RIGHT:
			_x = game::native::scrPlaceFull->scaleVirtualToReal[0] * *offs_x + game::native::scrPlaceFull->realViewableMax[0];
			_w = game::native::scrPlaceFull->scaleVirtualToReal[0] * *w;
			break;

		case HORIZONTAL_ALIGN_FULLSCREEN:
			_x = game::native::scrPlaceFull->scaleVirtualToFull[0] * *offs_x;
			_w = game::native::scrPlaceFull->scaleVirtualToFull[0] * *w;
			break;

		case HORIZONTAL_ALIGN_NOSCALE:
			goto USE_VERT_ALIGN; // we might wan't vertical alignment

		case HORIZONTAL_ALIGN_TO640:
			_x = game::native::scrPlaceFull->scaleRealToVirtual[0] * *offs_x;
			_w = game::native::scrPlaceFull->scaleRealToVirtual[0] * *w;
			break;

		case HORIZONTAL_ALIGN_CENTER_SAFEAREA:
			_x = (game::native::scrPlaceFull->realViewableMax[0] + game::native::scrPlaceFull->realViewableMin[0]) * 0.5f + game::native::scrPlaceFull->scaleVirtualToReal[0] * *offs_x;
			_w = game::native::scrPlaceFull->scaleVirtualToReal[0] * *w;
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
			_y = game::native::scrPlaceFull->scaleVirtualToReal[1] * *offs_y + game::native::scrPlaceFull->realViewableMin[1];
			_h = game::native::scrPlaceFull->scaleVirtualToReal[1] * *h;
			break;

		case VERTICAL_ALIGN_CENTER:
			_y = game::native::scrPlaceFull->scaleVirtualToReal[1] * *offs_y + game::native::scrPlaceFull->realViewportSize[1] * 0.5f;
			_h = game::native::scrPlaceFull->scaleVirtualToReal[1] * *h;
			break;

		case VERTICAL_ALIGN_BOTTOM:
			_y = game::native::scrPlaceFull->scaleVirtualToReal[1] * *offs_y + game::native::scrPlaceFull->realViewableMax[1];
			_h = game::native::scrPlaceFull->scaleVirtualToReal[1] * *h;
			break;

		case VERTICAL_ALIGN_FULLSCREEN:
			_y = game::native::scrPlaceFull->scaleVirtualToFull[1] * *offs_y;
			_h = game::native::scrPlaceFull->scaleVirtualToFull[1] * *h;
			break;

		case VERTICAL_ALIGN_NOSCALE:
			return;

		case VERTICAL_ALIGN_TO480:
			_y = game::native::scrPlaceFull->scaleRealToVirtual[1] * *offs_y;
			_h = game::native::scrPlaceFull->scaleRealToVirtual[1] * *h;
			break;

		case VERTICAL_ALIGN_CENTER_SAFEAREA:
			_y = game::native::scrPlaceFull->scaleVirtualToReal[1] * *offs_y + (game::native::scrPlaceFull->realViewableMax[1] + game::native::scrPlaceFull->realViewableMin[1]) * 0.5f;
			_h = game::native::scrPlaceFull->scaleVirtualToReal[1] * *h;
			break;

		case VERTICAL_ALIGN_SUBTOP:
			_y = game::native::scrPlaceFull->scaleVirtualToReal[1] * *offs_y;
			_h = game::native::scrPlaceFull->scaleVirtualToReal[1] * *h;
			break;

		default:
			return;
		}

		*offs_y = _y;
		*h = _h;
	}

	void _ui::redraw_cursor()
	{
		// get material handle
		const auto cur_material = game::native::Material_RegisterHandle("ui_cursor", 3);

		float cur_w = (32.0f * game::native::scrPlace->scaleVirtualToReal[0]) / game::native::scrPlace->scaleVirtualToFull[0];
		float cur_h = (32.0f * game::native::scrPlace->scaleVirtualToReal[1]) / game::native::scrPlace->scaleVirtualToFull[1];
		float cur_x = game::native::ui_context->cursor.x - 0.5f * cur_w;
		float cur_y = game::native::ui_context->cursor.y - 0.5f * cur_h;

		float s0, s1;
		float t0, t1;

		if (cur_w >= 0.0f)
		{
			s0 = 0.0f;
			s1 = 1.0f;
		}
		else
		{
			cur_w = -cur_w;
			s0 = 1.0f;
			s1 = 0.0f;
		}

		if (cur_h >= 0.0f)
		{
			t0 = 0.0f;
			t1 = 1.0f;
		}
		else
		{
			cur_h = -cur_h;
			t0 = 1.0f;
			t1 = 0.0f;
		}

		// scale 640x480 rect to viewport resolution and draw the cursor
		scrplace_apply_rect(&cur_x, &cur_w, &cur_y, &cur_h, VERTICAL_ALIGN_FULLSCREEN, VERTICAL_ALIGN_FULLSCREEN);
		game::native::R_AddCmdDrawStretchPic(cur_material, cur_x, cur_y, cur_w, cur_h, s0, t0, s1, t1, nullptr);
	}
}