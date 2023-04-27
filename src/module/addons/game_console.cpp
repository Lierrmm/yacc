/*
  Taken from IW3XO by xoxor4d
  https://github.com/xoxor4d/iw3xo-dev
*/


#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include "game/game.hpp"
#include "game/dvars.hpp"

#include <module/setup/console.hpp>
#include <module/setup/scheduler.hpp>

#include <utils/concurrency.hpp>
#include <utils/_utils.hpp>
#include <utils/string.hpp>
#include <utils/hook.hpp>

#include <module/ui/_ui.hpp>
#include <version.h>
#include "game_console.hpp"

#define CON_KEY_TOGGLE_CURSOR	KEYCATCHER_F1
#define CON_KEY_RESET_FLTCON	KEYCATCHER_F2
#define GET_CONSOLEFONT (game::native::Font_s*) *(DWORD*)(game::native::con_font_ptr)

const char* CON_HELP_PRINT = "---------------- CONSOLE HELP ----------------------\"^2help^7\"--------------------------\n"
"!     F1 :: Toggle cursor in-game\n"
"!     F2 :: Reset floating console\n"
"! Mouse1 :: Press and hold mouse1 on the input bar to move / resize the console\n"
"! yacc_con :: Prefix for additional console dvars\n"
"------------------------------------------------------------------------------------\n"
"\n";

	bool new_console::using_custom_console = false;
	bool new_console::is_using_custom_console()
	{
		return new_console::using_custom_console;
	}
	// init console addon struct
	game::native::Console_Addons console_addon_ = game::native::Console_Addons();

	static const char* console_mapdir_ = "maps/mp";
	static const char* console_mapext_ = "autocomplete"; // change d3dbsp to autocomplete

	int new_console::cmd_argc()
	{
		return game::native::cmd_args->argc[game::native::cmd_args->nesting];
	}

	#pragma region CON-UTILS

	bool is_dvar_command(char* cmd)
	{
		if (!_stricmp(cmd, "set"))
		{
			return true;
		}

		if (!_stricmp(cmd, "seta"))
		{
			return true;
		}

		if (!_stricmp(cmd, "sets"))
		{
			return true;
		}

		if (!_stricmp(cmd, "reset"))
		{
			return true;
		}

		if (_stricmp(cmd, "toggle"))
		{
			return _stricmp(cmd, "togglep") == false;
		}

		return true;
	}

	// box drawing helper function -- depth material
	void draw_rect(float* color, float x, float y, float w, float h, bool depth, bool ignore_fullscreen = false)
	{
		game::native::Material* material;

		material = game::native::Material_RegisterHandle("white", 3);
		

		if (!game::native::con->outputVisible || ignore_fullscreen)
		{
			// small console
			game::native::R_AddCmdDrawStretchPic(material, x, y, w, h, 0.0f, 0.0f, 0.0f, 0.0f, color);
		}
		else
		{
			// fullscreen
			game::native::R_AddCmdDrawStretchPic(material, x, y, w, h, 0.0f, 0.0f, 0.0f, 0.0f, color);
		}
	}

	// register font and draw text
	void draw_input_text(float x, float y, float scale_x, float scale_y, const char* font, const float* color, const char* text)
	{
		void* font_handle = game::native::R_RegisterFont(font, sizeof(font));
		game::native::R_AddCmdDrawTextASM(text, 0x7FFFFFFF, font_handle, x, y, scale_x, scale_y, 0.0f, color, 0);
	}

	// draw text
	void draw_input_text(float x, float y, float scale_x, float scale_y, game::native::Font_s* font, const float* color, const char* text)
	{
		game::native::R_AddCmdDrawTextASM(text, 0x7FFFFFFF, font, x, y, scale_x, scale_y, 0.0f, color, 0);
	}

	#pragma endregion

	#pragma region CON-DRAW
	float  console_input_glob_y_fuckery_ = 15.0f;
	float* console_glob_color_;

	// mid-func hook to alter hintbox drawing (upper box with dvar name and value)
	void draw_rect_detailed_match_upper(float* color, int lines)
	{
		// small console offsets --------------------------------------------------------
		// upper.x and upper.w get set in "DrawInput" :: Exact Match - Draw Details
		console_addon_.items.s_con.hint_box_upper.y = game::native::conDrawInputGlob->y - 8.0f - 15.0f + console_input_glob_y_fuckery_;
		console_addon_.items.s_con.hint_box_upper.h = static_cast<float>(lines) * game::native::conDrawInputGlob->fontHeight + 16.0f;

		// full console offsets --------------------------------------------------------
		// upper.x and upper.w get set in "DrawInput" :: Exact Match - Draw Details
		console_addon_.items.f_con.hint_box_upper.y = game::native::conDrawInputGlob->y - 10.0f - 11.0f + console_input_glob_y_fuckery_;
		console_addon_.items.f_con.hint_box_upper.h = static_cast<float>(lines) * game::native::conDrawInputGlob->fontHeight + 16.0f;//+ 4.0f;


		// get dimensions of upper + lower hint box (needed for clipping)
		// get upper box ::

		// small console total hintbox part 1/2 --------------------------------------------------------
		console_addon_.items.s_con.hint_box_total.x = console_addon_.items.s_con.hint_box_upper.x;
		console_addon_.items.s_con.hint_box_total.y = console_addon_.items.s_con.hint_box_upper.y;
		console_addon_.items.s_con.hint_box_total.w = console_addon_.items.s_con.hint_box_upper.w;
		console_addon_.items.s_con.hint_box_total.h = console_addon_.items.s_con.hint_box_upper.h;

		// full console total hintbox part 1/2 --------------------------------------------------------
		console_addon_.items.f_con.hint_box_total.x = console_addon_.items.f_con.hint_box_upper.x;
		console_addon_.items.f_con.hint_box_total.y = console_addon_.items.f_con.hint_box_upper.y;
		console_addon_.items.f_con.hint_box_total.w = console_addon_.items.f_con.hint_box_upper.w;
		console_addon_.items.f_con.hint_box_total.h = console_addon_.items.f_con.hint_box_upper.h;

		draw_rect(
			/*    c		*/ game::native::con->outputVisible ? console_glob_color_ : color,
			/*	  x		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.x : console_addon_.items.s_con.hint_box_upper.x,
			/*	  y		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.y : console_addon_.items.s_con.hint_box_upper.y,
			/*	  w		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.w : console_addon_.items.s_con.hint_box_upper.w,
			/*	  h		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.h : console_addon_.items.s_con.hint_box_upper.h,
			/*  depth	*/ false,
			/* ignoreFS	*/ true);


		// hintbox shadow --------------------------------------------------------
		float shadow_color[4] = { 0.0f, 0.0f, 0.0f, 0.125f };

		draw_rect(
			/*    c		*/ shadow_color,
			/*	  x		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.x : console_addon_.items.s_con.hint_box_upper.x,
			/*	  y		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.y : console_addon_.items.s_con.hint_box_upper.y,
			/*	  w		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.w : console_addon_.items.s_con.hint_box_upper.w,
			/*	  h		*/ 3.0f,
			/*  depth	*/ false,
			/* ignoreFS	*/ true);
	}

	// mid-func hook to alter hintbox drawing (lower box with dvar description) -- gets called after ConDrawInput_Box_DetailedMatch_UpperBox
	void draw_rect_detailed_match_lower(float* color, int lines)
	{
		// Game::conDrawInputGlob->y changes somewhere between ConDrawInput_Box_DetailedMatch_UpperBox & ConDrawInput_Box_DetailedMatch_LowerBox
		// so we only add Game::conDrawInputGlob->y to our lower hint box

		// small console offsets --------------------------------------------------------
		console_addon_.items.s_con.hint_box_lower.x = console_addon_.items.s_con.hint_box_upper.x;
		console_addon_.items.s_con.hint_box_lower.y = game::native::conDrawInputGlob->y - 8.0f - 15.0f + console_input_glob_y_fuckery_;
		console_addon_.items.s_con.hint_box_lower.h = lines * game::native::conDrawInputGlob->fontHeight + 16.0f;
		console_addon_.items.s_con.hint_box_lower.w = console_addon_.items.s_con.hint_box_upper.w;

		// full console offsets --------------------------------------------------------
		console_addon_.items.f_con.hint_box_lower.x = console_addon_.items.f_con.hint_box_upper.x;
		console_addon_.items.f_con.hint_box_lower.y = game::native::conDrawInputGlob->y - 10.0f - 11.0f + console_input_glob_y_fuckery_;
		console_addon_.items.f_con.hint_box_lower.h = lines * game::native::conDrawInputGlob->fontHeight + 16.0f;// + 4.0f;
		console_addon_.items.f_con.hint_box_lower.w = console_addon_.items.f_con.hint_box_upper.w;


		// get dimensions of upper + lower hint box (needed for clipping)
		// add lower box to upper box ::

		// small console total hintbox height part 2/2 --------------------------------------------------------
		console_addon_.items.s_con.hint_box_total.h += console_addon_.items.s_con.hint_box_lower.h;

		// full console total hintbox height part 2/2 --------------------------------------------------------
		console_addon_.items.f_con.hint_box_total.h += console_addon_.items.f_con.hint_box_lower.h;


		draw_rect(
			/*    c		*/ game::native::con->outputVisible ? console_glob_color_ : color,
			/*	  x		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_lower.x : console_addon_.items.s_con.hint_box_lower.x,//own_x,
			/*	  y		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_lower.y : console_addon_.items.s_con.hint_box_lower.y,//own_y,
			/*	  w		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_lower.w : console_addon_.items.s_con.hint_box_lower.w,//own_w,
			/*	  h		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_lower.h : console_addon_.items.s_con.hint_box_lower.h,//own_h,
			/*  depth	*/ false,
			/* ignoreFS	*/ true);
	}

	bool is_cursor_within_bounds(game::native::vec2_t bounds, const game::native::conAddons_CursorPos* cursor_pos)
	{
		if (cursor_pos->x < 0.0f || cursor_pos->x > bounds[0])
		{
			return true;
		}

		if (cursor_pos->y < 0.0f || cursor_pos->y > bounds[1])
		{
			return true;
		}

		return false;
	}

	bool is_cursor_within_item_box_bounds(const game::native::conItem_box* box, const game::native::conAddons_CursorPos* cursorPos)
	{
		// top left pt		= 0, 0	// top right pt		= 1, 0
		// bottom left pt	= 0, 1	// bottom right pt	= 1, 1

		// if cursor to right of left pt
		if (cursorPos->x > box->x)
		{
			// if cursor to left of right pt
			if (cursorPos->x < (box->x + box->w))
			{
				// if cursor below top pt
				if (cursorPos->y > box->y)
				{
					// if cursor above bottom pt
					if (cursorPos->y < (box->y + box->h))
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	void clip_console_against_borders()
	{
		// only clip the console when its open
		if (!(game::native::clientUI->keyCatchers & 1))
		{
			return;
		}

		// -------------------------------------------
		// clip the console against the window borders

		// clip at left border
		if (console_addon_.floating.anker.left <= 0.0f)
		{
			console_addon_.floating.anker.left = 0.0f;
			console_addon_.floating.clipped_at_border.left = true;

			// if we resize by pushing into the left border or with the resize button, limit the width to min width
			if (console_addon_.floating.anker.left + console_addon_.floating.anker.right < console_addon_.floating.min_dimensions.width)
			{
				console_addon_.floating.anker.right = console_addon_.floating.min_dimensions.width;
			}
		}
		else
		{
			// not clipped at border
			console_addon_.floating.clipped_at_border.left = false;
		}

		// min width check on resizing with resize button
		if (console_addon_.floating.anker.right - console_addon_.floating.anker.left < console_addon_.floating.min_dimensions.width)
		{
			console_addon_.floating.anker.right = console_addon_.floating.min_dimensions.width + console_addon_.floating.anker.left;
		}

		// --------------------------------------------

		// clip at top border
		if (console_addon_.floating.anker.top <= 0.0f)
		{
			console_addon_.floating.anker.top = 0.0f;
			console_addon_.floating.clipped_at_border.top = true;
		}
		else
		{
			// not clipped at border
			console_addon_.floating.clipped_at_border.top = false;
		}

		// --------------------------------------------

		// clip at right border
		if (console_addon_.floating.anker.right >= console_addon_.viewport_res.width)
		{
			console_addon_.floating.anker.right = console_addon_.viewport_res.width;
			console_addon_.floating.clipped_at_border.right = true;

			// if we resize by pushing into the right border or with the resize button, limit the width to min width
			if (console_addon_.floating.anker.right - console_addon_.floating.anker.left < console_addon_.floating.min_dimensions.width)
			{
				console_addon_.floating.anker.left = console_addon_.floating.anker.right - console_addon_.floating.min_dimensions.width;
			}
		}
		else
		{
			// not clipped at border
			console_addon_.floating.clipped_at_border.right = false;
		}

		// --------------------------------------------

		// clip at bottom border
		// we cannot clip the console at the bottom of the screen with con->screenMax[1] (bottom) 
		// because con->screenMax[1] is not used in any logic for the small console
		// so we clip con->screenMin[1] (top) instead

		// calculate the bottom position of the console and check if hits the bottom border of the screen
		if (console_addon_.floating.anker.top + console_addon_.items.s_con.output_box.h + console_addon_.items.s_con.input_box.h >= console_addon_.viewport_res.height)
		{
			// subtract from the bottom towards the top to get our min y
			console_addon_.floating.anker.top = console_addon_.viewport_res.height - console_addon_.items.s_con.output_box.h - console_addon_.items.s_con.input_box.h;
			console_addon_.floating.clipped_at_border.bottom = true;
		}
		else
		{
			// not clipped at border
			console_addon_.floating.clipped_at_border.bottom = false;
		}
	}

	// fixes for consoleFont pixelHeight changes when loading / unloading mods / vid_restart?
	void check_resize_on_init()
	{
		// left HORIZONTAL_APPLY_LEFT
		game::native::con->screenMin[0] = floorf(components::_ui::scrplace_apply_x(HORIZONTAL_APPLY_LEFT, dvars::yacc_con_padding->current.value, 0.0f));

		// top
		game::native::con->screenMin[1] = floorf(components::_ui::scrplace_apply_y(VERTICAL_APPLY_TOP, dvars::yacc_con_padding->current.value, 0.0f));

		// right
		game::native::con->screenMax[0] = floorf(components::_ui::scrplace_apply_x(HORIZONTAL_APPLY_RIGHT, -dvars::yacc_con_padding->current.value, 0.0f));

		// bottom
		game::native::con->screenMax[1] = floorf(components::_ui::scrplace_apply_y(VERTICAL_APPLY_BOTTOM, -dvars::yacc_con_padding->current.value, 0.0f));

		// reinit floating console on vid_restart / cgame changes or w/e
		console_addon_.floating.initialized = false;

		// still needed as we do that in DrawInput? ... cba to test
		console_addon_.viewport_res.width = floorf(components::_ui::scrplace_apply_x(HORIZONTAL_APPLY_RIGHT, 0.0f, 0.0f));
		console_addon_.viewport_res.height = floorf(components::_ui::scrplace_apply_y(VERTICAL_APPLY_BOTTOM, 0.0f, 0.0f));

		// mouse unrelated stuff ---------------------------------------

		if (const auto  console_font = GET_CONSOLEFONT;
			console_font)
		{
			if (console_font->pixelHeight != 16)
			{
				game::native::Com_PrintMessage(0, utils::string::va("Console :: consoleFont was %d", console_font->pixelHeight), 0);
			}

			game::native::con->fontHeight = static_cast<int>((static_cast<float>(console_font->pixelHeight) * dvars::yacc_con_fontSpacing->current.value));
			game::native::con->visibleLineCount = static_cast<int>((game::native::con->screenMax[1] - game::native::con->screenMin[1] - static_cast<float>(2 * game::native::con->fontHeight)) / static_cast<float>(game::native::con->fontHeight));
			game::native::con->visiblePixelWidth = static_cast<int>(game::native::con->screenMax[0] - game::native::con->screenMin[0] - -28.0f);
		}
		else
		{
			game::native::con->fontHeight = 0;
			game::native::con->visibleLineCount = 0;
			game::native::con->visiblePixelWidth = 0;
		}
	}

	// draw a resize button at the bottom right of the console input box
	void draw_resize_button(const float alpha)
	{
		game::native::vec4_t right, bottom;

		const float line_strength = 0.5f;
		float _alpha = alpha;

		if (_alpha + 0.2f <= 1.0f)
		{
			_alpha += 0.2f;
		}

		float resize_button_color[4] = { 1.0f, 1.0f, 1.0f, _alpha };
		const float padding[2] = { 3.0f, 4.5f };

		right[0] = console_addon_.items.s_con.input_box.x + console_addon_.items.s_con.input_box.w - padding[0] - line_strength;
		right[1] = console_addon_.items.s_con.input_box.y + console_addon_.items.s_con.input_box.h * 0.5f; // draw from top to bottom -- half height of input bar
		right[2] = line_strength; // line thickness
		right[2] = console_addon_.items.s_con.input_box.h * 0.5f - padding[0];

		// right vertical bar
		game::native::ConDraw_Box(resize_button_color,
			/* x */	right[0],
			/* y */ right[1],
			/* w */ right[2],
			/* h */ right[3]);

		bottom[0] = console_addon_.items.s_con.input_box.x + console_addon_.items.s_con.input_box.w - (console_addon_.items.s_con.input_box.h * 0.5f); // draw from left to right so - its width
		bottom[1] = console_addon_.items.s_con.input_box.y + console_addon_.items.s_con.input_box.h - padding[0] - 0.5f;
		bottom[2] = console_addon_.items.s_con.input_box.h - 18.0f;
		bottom[3] = 0.5f; // line thickness

		// bottom horizontal bar
		game::native::ConDraw_Box(resize_button_color,
			/* x */	bottom[0],
			/* y */ bottom[1],
			/* w */ bottom[2],
			/* h */ bottom[3]);

		// ------------
		// small square

		resize_button_color[3] -= 0.25f;

		game::native::ConDraw_Box(resize_button_color,
			/* x */	right[0] - line_strength - padding[1] - 3.0f,
			/* y */ bottom[1] - padding[1] - 4.0f,
			/* w */ 4.0f,
			/* h */ 4.0f);

		console_addon_.items.s_con.resize_button_trigger.x = bottom[0] - 2.0f;
		console_addon_.items.s_con.resize_button_trigger.y = right[1] - 2.0f;
		console_addon_.items.s_con.resize_button_trigger.w = bottom[2] + line_strength + 4.0f;
		console_addon_.items.s_con.resize_button_trigger.h = right[3] + 5.0f;
	}

	void draw_output_scrollbar(float x, float y, float width, float height, float padding = 0.0f)
	{
		const auto outputBarColor = game::native::Dvar_FindVar("con_outputBarColor")->current.vector;
		const auto outputSliderColor = game::native::Dvar_FindVar("con_outputSliderColor")->current.vector;

		const float background_width = 16.0f;
		const float background_pos_y = y + padding;
		const float background_pos_x = x + width - background_width;

		float background_height = height;

		if (game::native::con->outputVisible)
		{
			background_height -= padding;
		}

		const float nob_width = 16.0f;
		const float nob_min_height = 30.0f;
		const float nob_padding = (background_width - nob_width) * 0.5f;

		// Background
		game::native::ConDraw_Box(outputBarColor,
			/* x */	background_pos_x,
			/* y */ background_pos_y,
			/* w */ background_width,
			/* h */ background_height);

		// if there is content out of bounds (scrolling enabled)
		if (game::native::con->consoleWindow.activeLineCount > game::native::con->visibleLineCount)
		{
			const int hidden_line_count = game::native::con->consoleWindow.activeLineCount - game::native::con->visibleLineCount;

			// current nob length
			const float portion = (background_height * 0.01f) * static_cast<float>(hidden_line_count); // portion :: 1 percent of max nob height times invisible lines
			float nob_length = (background_height - portion);					// max nob height - portion;

			// if scaled nob length < min length
			if (nob_length < nob_min_height)
			{
				nob_length = nob_min_height;
			}

			// current nob y position
			const float cursor_pos = static_cast<float>((game::native::con->displayLineOffset - game::native::con->visibleLineCount)) * (1.0f / static_cast<float>(hidden_line_count));
			float nob_y = (y + height - nob_length - y) * utils::_utils::fmaxf3(cursor_pos, 0.0f, 1.0f) + y;

			// cap scrollbar nob top
			if (nob_y < background_pos_y)
			{
				nob_y = background_pos_y;
			}

			game::native::ConDraw_Box(outputSliderColor,
				/* x */ background_pos_x + nob_padding,
				/* y */ nob_y,
				/* w */ nob_width - nob_padding,
				/* h */ nob_length);
		}
		else // nothing to scroll
		{
			game::native::ConDraw_Box(outputSliderColor,
				/* x */ background_pos_x + nob_padding,
				/* y */ background_pos_y,
				/* w */ nob_width - nob_padding,
				/* h */ background_height);
		}
	}

	void draw_output_version(float x, float y, float height)
	{
		std::string build = VERSION;

#if DEBUG
		build += " :: ^1DEBUG";
#endif

		game::native::SCR_DrawSmallStringExt(static_cast<int>(x), static_cast<int>(height - 16.0f + y), build.c_str());
	}

	void draw_output_text(float x, float y, float height = 0.0f)
	{
		// Game::con->visibleLineCount in Con_CheckResize defines the container-height (can be used as cutoff) 
		// rowIndex / rowIndex == top line (adjusting it cuts off the newest lines)
		// rowCount == lines at the bottom (^ oldest lines)

		bool first_offset_skipped = false;
		const auto font_height = static_cast<float>(game::native::con->fontHeight);

		int rowCount = game::native::con->visibleLineCount;
		int firstRow = game::native::con->displayLineOffset - game::native::con->visibleLineCount;

		const float lines_for_spacing = ((dvars::yacc_con_fontSpacing->current.value + 0.2f) - 1.0f) * 10.0f;

		// no idea what i did here but it works
		y = y + height + font_height * 4.0f + font_height * 0.5f - lines_for_spacing * font_height * 0.5f;

		// adjust "cursor" 
		if (firstRow < 0)
		{
			y -= static_cast<float>(game::native::con->fontHeight * firstRow);

			rowCount = game::native::con->displayLineOffset;
			firstRow = 0;

			// first fullcon frame stuff here -- resets only when s_con was used
			if (game::native::con->outputVisible && !console_addon_.fcon_first_frame)
			{
				// if s_con was scrolled to the top, adjust the line offset once to put our fullcon text at the top
				game::native::con->displayLineOffset = game::native::con->visibleLineCount;
				console_addon_.fcon_first_frame = true;
			}
		}

		// get font handle
		game::native::Font_s* font_handle = game::native::R_RegisterFont(FONT_CONSOLE, sizeof(FONT_CONSOLE));

		for (auto row_index = 0; row_index < rowCount; ++row_index)
		{
			const auto line_index = (row_index + firstRow + game::native::con->consoleWindow.firstLineIndex) % game::native::con->consoleWindow.lineCount;

			// skip the first y offset as we add half a font height as padding
			if (first_offset_skipped)
			{
				y += font_height;
			}
			else
			{
				first_offset_skipped = true;
			}

			float white[4] = { 1, 1, 1, 1 };

			game::native::AddBaseDrawConsoleTextCmd(
				/*charCount	*/	game::native::con->consoleWindow.lines[line_index].textBufSize,
				/* colorFlt	*/	white,
				/* textPool	*/	game::native::con->consoleWindow.circularTextBuffer,
				/* poolSize	*/	game::native::con->consoleWindow.textBufSize,
				/* firstChar*/	game::native::con->consoleWindow.lines[line_index].textBufPos,
				/*   font	*/	font_handle,
				/*     x	*/	x,
				/*     y	*/	y,
				/*  xScale	*/	1.0f,
				/*  yScale	*/	1.0f,
				/*   style	*/	3); // 0 :: default -- 3 has slight shadows -- 6 even more :)
		}
	}

	void draw_output_window()
	{
		// output box
		console_addon_.items.f_con.output_box.x = console_addon_.items.f_con.input_box.x;
		console_addon_.items.f_con.output_box.y = console_addon_.items.f_con.input_box.y + console_addon_.items.f_con.input_box.h;
		console_addon_.items.f_con.output_box.w = console_addon_.items.f_con.input_box.w;
		console_addon_.items.f_con.output_box.h = game::native::con->screenMax[1] - console_addon_.items.f_con.output_box.y;

		const auto output_window_color = game::native::Dvar_FindVar("con_outputWindowColor")->current.vector;

		// output window
		draw_rect(
			/*	 color	*/	output_window_color,
			/*	   x	*/	console_addon_.items.f_con.output_box.x,
			/*	   y	*/	console_addon_.items.f_con.output_box.y,
			/*	   w	*/	console_addon_.items.f_con.output_box.w,
			/*	   h	*/	console_addon_.items.f_con.output_box.h,
			/*	depth	*/	false,
			/* ignoreFS	*/	true);

		// scrollbar
		draw_output_scrollbar(console_addon_.items.f_con.output_box.x, console_addon_.items.f_con.output_box.y, console_addon_.items.f_con.output_box.w, console_addon_.items.f_con.output_box.h, 0.0f);

		// build version long, bottom console
		draw_output_version(console_addon_.items.f_con.output_box.x + 6.0f, console_addon_.items.f_con.output_box.y - 4.0f, console_addon_.items.f_con.output_box.h);

		// console output below input text box
		draw_output_text(console_addon_.items.f_con.output_box.x + 6.0f, dvars::yacc_con_padding->current.value * 2.0f, 0.0f);
	}

	void draw_detailed_cmd_match(game::native::cmd_function_s* cmd)
	{
		if (cmd)
		{
			if (game::native::Con_IsAutoCompleteMatch(cmd->name, game::native::conDrawInputGlob->inputText, game::native::conDrawInputGlob->inputTextLen)
				&& (!game::native::conDrawInputGlob->hasExactMatch || !cmd->name[game::native::conDrawInputGlob->inputTextLen]))
			{
				draw_rect_detailed_match_upper(game::native::Dvar_FindVar("con_inputHintBoxColor")->current.vector, (cmd->description ? 2 : 0) + 2);

				const auto console_font = GET_CONSOLEFONT;
				const float cmd_color[4] = { 0.8f, 0.8f, 1.0f, 1.0f };

				// draw cmd string
				draw_input_text(
					/* x	*/ game::native::conDrawInputGlob->x,
					/* y	*/ game::native::conDrawInputGlob->y + game::native::conDrawInputGlob->fontHeight,
					/* xScl	*/ 1.0f,
					/* yScl	*/ 1.0f,
					/* font	*/ console_font,
					/* colr	*/ cmd_color,
					/* text	*/ cmd->name);

				// draw type (cmd)
				draw_input_text(
					/* x	*/ game::native::conDrawInputGlob->x,
					/* y	*/ game::native::conDrawInputGlob->y + (game::native::conDrawInputGlob->fontHeight * 2),
					/* xScl	*/ 1.0f,
					/* yScl	*/ 1.0f,
					/* font	*/ console_font,
					/* colr	*/ game::native::con_matchtxtColor_domainDescription,
					/* text	*/ "  command");

				// draw arg usage
				if (cmd->args)
				{
					draw_input_text(
						/* x	*/ game::native::conDrawInputGlob->x + 16.0f + static_cast<float>(game::native::R_TextWidth(cmd->name, 256, console_font)),
						/* y	*/ game::native::conDrawInputGlob->y + game::native::conDrawInputGlob->fontHeight,
						/* xScl	*/ 1.0f,
						/* yScl	*/ 1.0f,
						/* font	*/ console_font,
						/* colr	*/ game::native::con_matchtxtColor_dvarDescription,
						/* text	*/ cmd->args);
				}

				// draw description
				if (cmd->description)
				{
					draw_input_text(
						/* x	*/ game::native::conDrawInputGlob->x,
						/* y	*/ game::native::conDrawInputGlob->y + (game::native::conDrawInputGlob->fontHeight * 4.0f),
						/* xScl	*/ 1.0f,
						/* yScl	*/ 1.0f,
						/* font	*/ console_font,
						/* colr	*/ game::native::con_matchtxtColor_dvarDescription,
						/* text	*/ cmd->description);
				}

				game::native::conDrawInputGlob->y += game::native::conDrawInputGlob->fontHeight;
				game::native::conDrawInputGlob->x = game::native::conDrawInputGlob->leftX;

				// skip autocomplete for cmds
			}
		}
	}

	// :: Main Console Input + Logic ( Key logic still outside in Console_Key() )
	void draw_input()
	{
		if (!(*game::native::Key_IsCatcherActive) || !game::native::Sys_IsMainThread())
		{
			return;
		}

		float	x, y, w, h;

		// hintbox txt color dvars
		memcpy(&game::native::con_matchtxtColor_currentDvar[0], &dvars::yacc_con_hintBoxTxtColor_currentDvar->current.vector, sizeof(float[4]));
		memcpy(&game::native::con_matchtxtColor_currentValue[0], &dvars::yacc_con_hintBoxTxtColor_currentValue->current.vector, sizeof(float[4]));
		memcpy(&game::native::con_matchtxtColor_defaultValue[0], &dvars::yacc_con_hintBoxTxtColor_defaultValue->current.vector, sizeof(float[4]));
		memcpy(&game::native::con_matchtxtColor_dvarDescription[0], &dvars::yacc_con_hintBoxTxtColor_dvarDescription->current.vector, sizeof(float[4]));
		memcpy(&game::native::con_matchtxtColor_domainDescription[0], &dvars::yacc_con_hintBoxTxtColor_domainDescription->current.vector, sizeof(float[4]));

		// increase max drawing width for console input
		game::native::g_consoleField->drawWidth = 512;

		const auto con_inputBoxColor = game::native::Dvar_FindVar("con_inputBoxColor")->current.vector;

		// set con globals
		const auto console_font = GET_CONSOLEFONT;

		game::native::conDrawInputGlob->fontHeight = (float)(console_font->pixelHeight);
		game::native::conDrawInputGlob->x = game::native::con->screenMin[0];
		game::native::conDrawInputGlob->y = game::native::con->screenMin[1];
		game::native::conDrawInputGlob->leftX = game::native::conDrawInputGlob->x;

		// :: if small console
		if (!game::native::con->outputVisible)
		{
			// y = top of outputbox
			// h = height of outputbox

			x = game::native::conDrawInputGlob->x;
			y = game::native::conDrawInputGlob->y;
			w = game::native::con->screenMax[0] - game::native::con->screenMin[0] - (x - game::native::con->screenMin[0]);
			h = static_cast<float>((dvars::yacc_con_outputHeight->current.integer + 4) * game::native::con->fontHeight); // add 4 because we get the first line @5

			// input box
			const float input_box_height = game::native::conDrawInputGlob->fontHeight + 15.0f;
			console_addon_.items.s_con.input_box.x = x;
			console_addon_.items.s_con.input_box.y = y + h - input_box_height;
			console_addon_.items.s_con.input_box.w = w;
			console_addon_.items.s_con.input_box.h = input_box_height;

			// output box
			console_addon_.items.s_con.output_box.x = x;
			console_addon_.items.s_con.output_box.y = y;
			console_addon_.items.s_con.output_box.w = w;
			console_addon_.items.s_con.output_box.h = h - console_addon_.items.s_con.input_box.h;

			// output box text
			console_addon_.items.s_con.output_text.x = game::native::conDrawInputGlob->x + 8.0f;
			console_addon_.items.s_con.output_text.y = y - h;
			console_addon_.items.s_con.output_text.h = h;

			// output box slider
			console_addon_.items.s_con.output_slider.x = x;
			console_addon_.items.s_con.output_slider.y = y;
			console_addon_.items.s_con.output_slider.w = w;
			console_addon_.items.s_con.output_slider.h = h - 32.0f;
			console_addon_.items.s_con.output_slider.padding = 0.0f;

			// ------------------------------ // output box
			draw_rect(
				/* col */ game::native::Dvar_FindVar("con_outputWindowColor")->current.vector,
				/*  x  */ console_addon_.items.s_con.output_box.x,
				/*  y  */ console_addon_.items.s_con.output_box.y,
				/*  z  */ console_addon_.items.s_con.output_box.w,
				/*  h  */ console_addon_.items.s_con.output_box.h,
				/* dep */ true);

			// ------------------------------ // output box text
			draw_output_text(
				/*	  x		*/ console_addon_.items.s_con.output_text.x,
				/*	  y		*/ console_addon_.items.s_con.output_text.y,
				/* bxHeight	*/ console_addon_.items.s_con.output_box.h);

			// ------------------------------ // output box slider
			draw_output_scrollbar(
				/*  x  */ console_addon_.items.s_con.output_slider.x,
				/*  y  */ console_addon_.items.s_con.output_slider.y,
				/*  w  */ console_addon_.items.s_con.output_slider.w,
				/*  h  */ console_addon_.items.s_con.output_slider.h,
				/* pad */ console_addon_.items.s_con.output_slider.padding);


			// offset input text:
			game::native::conDrawInputGlob->x += 5.0f;
			game::native::conDrawInputGlob->y += h - 24.0f;

			// ------------------------------ // input box
			draw_rect(
				/* col */ con_inputBoxColor,
				/*  x  */ console_addon_.items.s_con.input_box.x,
				/*  y  */ console_addon_.items.s_con.input_box.y,
				/*  z  */ console_addon_.items.s_con.input_box.w,
				/*  h  */ console_addon_.items.s_con.input_box.h,
				/* dep */ true);

			// draw a resize button with a trigger box bottom right
			// we have to call that after getting the newest inputBox values or else we lack a frame behind
			draw_resize_button(con_inputBoxColor[3]);

			// reset fullcon first frame toggle
			console_addon_.fcon_first_frame = false;
		}

		// :: if fullscreen console
		else
		{
			// y = top of outputbox
			// h = height of outputbox

			x = game::native::conDrawInputGlob->x;
			y = game::native::conDrawInputGlob->y;
			w = game::native::con->screenMax[0] - game::native::con->screenMin[0] - (x - game::native::con->screenMin[0]);
			h = game::native::conDrawInputGlob->fontHeight + 15.0f;

			// input box
			console_addon_.items.f_con.input_box.x = x;
			console_addon_.items.f_con.input_box.y = y;
			console_addon_.items.f_con.input_box.w = w;
			console_addon_.items.f_con.input_box.h = h;

			// ------------------------------ // input box
			draw_rect(
				/* col */ con_inputBoxColor,
				/*  x  */ console_addon_.items.f_con.input_box.x,
				/*  y  */ console_addon_.items.f_con.input_box.y,
				/*  w  */ console_addon_.items.f_con.input_box.w,
				/*  h  */ console_addon_.items.f_con.input_box.h,
				/* dep */ false);

			game::native::conDrawInputGlob->x += 5.0f;
			game::native::conDrawInputGlob->y += 5.0f;
		}

		// ------------------------------ // input text for small / full console
		float white[4] = { 1, 1,1,1 };
		draw_input_text(
			/*  x  */ game::native::conDrawInputGlob->x,
			/*  y  */ game::native::conDrawInputGlob->y + game::native::conDrawInputGlob->fontHeight + (game::native::con->outputVisible ? 2.0f : 0.0f),
			/* scX */ 0.75f,
			/* scY */ 0.75f,
			/* fon */ FONT_BIG,
			/* col */ white,
			/* txt */ utils::string::va("YACC >"));

		// move cursor position
		game::native::conDrawInputGlob->x += 40.0f;

		// --------------------------------------------------------------------------- 
#pragma region CON-LOGIC

		game::native::conDrawInputGlob->leftX = game::native::conDrawInputGlob->x;
		const auto input_text_len_prev = game::native::conDrawInputGlob->inputTextLen;

		game::native::g_consoleField->widthInPixels = static_cast<int>(game::native::con->screenMax[0] - 6.0f - game::native::conDrawInputGlob->x);
		char* con_tokenized_input = game::native::Con_TokenizeInput();

		game::native::conDrawInputGlob->inputText = con_tokenized_input;
		auto input_text = strlen(con_tokenized_input);

		game::native::conDrawInputGlob->inputTextLen = static_cast<int>(input_text);
		game::native::conDrawInputGlob->autoCompleteChoice[0] = 0;

		int	 cmd_or_dvar = 0;

		auto var_con_original_command = con_tokenized_input;
		int  var_con_current_match_count = 0;
		int  var_con_is_dvar_command = 0;

		float output_slider_width = 16.0f;

		if (input_text_len_prev != static_cast<int>(input_text))
		{
			//Game::Con_CancelAutoComplete(); // Inlined ::
			if (game::native::conDrawInputGlob->matchIndex >= 0 && game::native::conDrawInputGlob->autoCompleteChoice[0]) // T5 way
			{
				game::native::conDrawInputGlob->matchIndex = -1;
				game::native::conDrawInputGlob->autoCompleteChoice[0] = 0;
			}

			input_text = game::native::conDrawInputGlob->inputTextLen;
			con_tokenized_input = (char*)game::native::conDrawInputGlob->inputText;
		}

		if (!input_text)
		{
			game::native::conDrawInputGlob->mayAutoComplete = false;

			// Input Cursor when input text is empty
			if (console_addon_.floating.initialized)
			{
				game::native::Con_DrawInputPrompt();
			}

			game::native::Cmd_EndTokenizedString();
			goto REDRAW_CURSOR_RETURN;
		}

		var_con_original_command = con_tokenized_input;

		if (new_console::cmd_argc() > 1 && is_dvar_command(con_tokenized_input))
		{
			cmd_or_dvar = 1;
			game::native::conDrawInputGlob->inputText = game::native::Cmd_Argv(1);
			game::native::conDrawInputGlob->inputTextLen = static_cast<int>(strlen(game::native::conDrawInputGlob->inputText));

			if (!game::native::conDrawInputGlob->inputTextLen)
			{
				game::native::conDrawInputGlob->mayAutoComplete = false;
				game::native::Con_DrawInputPrompt();
				game::native::Cmd_EndTokenizedString();

				goto REDRAW_CURSOR_RETURN;
			}
		}

		else
		{
			cmd_or_dvar = 0;
		}

		game::native::conDrawInputGlob->hasExactMatch = false;
		game::native::conDrawInputGlob->matchCount = 0;

		if (game::native::Dvar_FindVar("con_matchPrefixOnly")->current.enabled == 0)
		{
			*game::native::extvar_con_ignoreMatchPrefixOnly = false;
			goto CON_MATCH_PREFIX_ONLY;
		}

		*game::native::extvar_con_ignoreMatchPrefixOnly = true;
		game::native::Dvar_ForEachName(game::native::ConDrawInput_IncrMatchCounter);

		if (!cmd_or_dvar)
		{
			game::native::Cmd_ForEachXO(game::native::ConDrawInput_IncrMatchCounter);
		}

		if (game::native::conDrawInputGlob->matchCount > dvars::yacc_con_maxMatches->current.integer) // autocomplete avail.
		{
			game::native::conDrawInputGlob->hasExactMatch = false;
			game::native::conDrawInputGlob->matchCount = 0;
			*game::native::extvar_con_ignoreMatchPrefixOnly = false;
			game::native::Dvar_ForEachName(game::native::ConDrawInput_IncrMatchCounter);

			game::native::Cmd_ForEachXO(game::native::ConDrawInput_IncrMatchCounter);

			if (!game::native::conDrawInputGlob->matchCount)
			{
				game::native::conDrawInputGlob->hasExactMatch = false;
				game::native::conDrawInputGlob->matchCount = 0;
				*game::native::extvar_con_ignoreMatchPrefixOnly = true;


			CON_MATCH_PREFIX_ONLY:
				game::native::Dvar_ForEachName(game::native::ConDrawInput_IncrMatchCounter);

				if (!cmd_or_dvar)
				{
					game::native::Cmd_ForEachXO(game::native::ConDrawInput_IncrMatchCounter);
				}
			}
		}

		var_con_current_match_count = game::native::conDrawInputGlob->matchCount;

		if (!game::native::conDrawInputGlob->matchCount)
		{
			game::native::Con_DrawInputPrompt();
			game::native::Cmd_EndTokenizedString();

			goto REDRAW_CURSOR_RETURN;
		}

		var_con_is_dvar_command = game::native::conDrawInputGlob->matchIndex;

		if (game::native::conDrawInputGlob->matchIndex < game::native::conDrawInputGlob->matchCount && game::native::conDrawInputGlob->autoCompleteChoice[0])
		{
			if (game::native::conDrawInputGlob->matchIndex >= 0)
			{
				var_con_is_dvar_command = cmd_or_dvar;
				game::native::Con_DrawAutoCompleteChoice(var_con_is_dvar_command, var_con_original_command); // color AutoComplete String

				goto CON_SKIP_INPUT_PROMPT;
			}
		}

		else
		{
			game::native::conDrawInputGlob->matchIndex = -1;
		}

		game::native::Con_DrawInputPrompt();

	CON_SKIP_INPUT_PROMPT:
		game::native::conDrawInputGlob->y = game::native::conDrawInputGlob->y + (2 * game::native::conDrawInputGlob->fontHeight + 15.0f);
		game::native::conDrawInputGlob->x = game::native::conDrawInputGlob->leftX;
#pragma endregion

		// --------------------------------------------------------------------------- 
		// :: HINTBOX

		// copy "con_inputHintBoxColor" to dynamically change its alpha without changing the dvar
		float loc_inputHintBoxColor[4];
		memcpy(&loc_inputHintBoxColor, game::native::Dvar_FindVar("con_inputHintBoxColor")->current.vector, sizeof(float[4]));

		// reduce alpha on fullscreen to make text more readable - will reset itself when small again
		if (loc_inputHintBoxColor[3] < 1.0f && game::native::con->outputVisible)
		{
			loc_inputHintBoxColor[3] = 0.95f; // alpha
		}

		// --------------------------------------------------------------------------- 
		// :: Too many matches to show ( 1 line )

		output_slider_width = 16.0f;

		if (game::native::conDrawInputGlob->matchCount > dvars::yacc_con_maxMatches->current.integer)
		{
			// create the text and get its width in pixels so we can center it
			const auto font_handle = game::native::R_RegisterFont(FONT_CONSOLE, sizeof(FONT_CONSOLE));
			const char* too_much_text = utils::string::va("%i matches (too many to show here, press TAB to print all, press SHIFT + TILDE to open full console)", var_con_current_match_count);

			const int txt_length = game::native::R_TextWidth(too_much_text, 0, font_handle);

			// small console --------------------------------------------------------

			// hint box
			console_addon_.items.s_con.hint_box_upper.x = x;
			console_addon_.items.s_con.hint_box_upper.y = game::native::conDrawInputGlob->y - game::native::conDrawInputGlob->fontHeight - 7.0f;
			console_addon_.items.s_con.hint_box_upper.w = w;
			console_addon_.items.s_con.hint_box_upper.h = game::native::conDrawInputGlob->fontHeight + 17.0f;

			// hint box text (centered)
			console_addon_.items.s_con.hint_box_upper_text.x = floorf((console_addon_.items.s_con.hint_box_upper.w * 0.5f) - ((float)txt_length * 0.5f) + console_addon_.items.s_con.input_box.x);
			//conAddon.conItems.s_con.hintBoxUpperText.x = x + 18.0f; // left aligned
			console_addon_.items.s_con.hint_box_upper_text.y = game::native::conDrawInputGlob->y;

			// full console --------------------------------------------------------

			// hint box
			console_addon_.items.f_con.hint_box_upper.x = x;
			console_addon_.items.f_con.hint_box_upper.y = game::native::conDrawInputGlob->y - game::native::conDrawInputGlob->fontHeight - 5.0f;
			console_addon_.items.f_con.hint_box_upper.w = w - output_slider_width;
			console_addon_.items.f_con.hint_box_upper.h = game::native::conDrawInputGlob->fontHeight + 17.0f;

			// hint box text (centered)
			console_addon_.items.f_con.hint_box_upper_text.x = (console_addon_.items.f_con.hint_box_upper.w * 0.5f) - ((float)txt_length * 0.5f) + console_addon_.items.f_con.input_box.x;
			//conAddon.conItems.fullCon.hintBoxUpperText.x = x + 10.0f + 34.0f; // left aligned
			console_addon_.items.f_con.hint_box_upper_text.y = game::native::conDrawInputGlob->y + 2.0f;

			// ---------------------------------------------------------------------

			// if small console is near the bottom or clipped at the bottom, put hintboxes above the output window
			if (!game::native::con->outputVisible && console_addon_.floating.enabled && (console_addon_.floating.clipped_at_border.bottom || console_addon_.floating.anker.bottom + console_addon_.items.s_con.hint_box_upper.h > console_addon_.viewport_res.height))
			{
				// calculate the new y
				console_addon_.items.s_con.hint_box_upper.y = console_addon_.floating.anker.bottom - console_addon_.items.s_con.input_box.h - console_addon_.items.s_con.output_box.h - console_addon_.items.s_con.hint_box_upper.h;
				console_addon_.items.s_con.hint_box_upper_text.y = console_addon_.items.s_con.hint_box_upper.y + ((console_addon_.items.s_con.hint_box_upper.h * 0.5f) + (game::native::conDrawInputGlob->fontHeight * 0.5f) + 0.5f); // offset by half a pixel (blurriness)
			}

			draw_rect(
				/* col */ loc_inputHintBoxColor,
				/*  x  */ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.x : console_addon_.items.s_con.hint_box_upper.x,
				/*  y  */ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.y : console_addon_.items.s_con.hint_box_upper.y,
				/*  w  */ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.w : console_addon_.items.s_con.hint_box_upper.w,
				/*  h  */ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.h : console_addon_.items.s_con.hint_box_upper.h,
				/* dep */ false,
				/* iFS */ true);

			// hintbox shadow --------------------------------------------------------
			float shadow_color[4] = { 0.0f, 0.0f, 0.0f, 0.125f };

			draw_rect(
				/*    c		*/ shadow_color,
				/*	  x		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.x : console_addon_.items.s_con.hint_box_upper.x,
				/*	  y		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.y : console_addon_.items.s_con.hint_box_upper.y,
				/*	  w		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.w + output_slider_width : console_addon_.items.s_con.hint_box_upper.w,
				/*	  h		*/ 3.0f,
				/*  depth	*/ false,
				/* ignoreFS	*/ true);

			draw_input_text(
				/*  x  */ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper_text.x : console_addon_.items.s_con.hint_box_upper_text.x,
				/*  y  */ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper_text.y : console_addon_.items.s_con.hint_box_upper_text.y,
				/* scX */ 1.0f,
				/* scY */ 1.0f,
				/* fon */ font_handle,
				/* col */ dvars::yacc_con_hintBoxTxtColor_currentDvar->current.vector,
				/* txt */ too_much_text);

			game::native::Cmd_EndTokenizedString();
			goto REDRAW_CURSOR_RETURN;
		}

		// --------------------------------------------------------------------------- 
		// :: Exact Match - Draw Details

		if (game::native::conDrawInputGlob->matchCount == 1 || game::native::conDrawInputGlob->hasExactMatch && game::native::Con_AnySpaceAfterCommand())
		{
			console_glob_color_ = loc_inputHintBoxColor;

			// small console --------------------------------------------------------

			// hint box -- we calculate y & h within DetailedMatch_UpperBox && DetailedMatch_LowerBox
			console_addon_.items.s_con.hint_box_upper.x = x;
			console_addon_.items.s_con.hint_box_upper.w = w;

			// hint box text
			// we do not handle text for dvar matches

			// full console --------------------------------------------------------

			// hint box -- we calculate y & h within DetailedMatch_UpperBox && DetailedMatch_LowerBox
			console_addon_.items.f_con.hint_box_upper.x = x;
			console_addon_.items.f_con.hint_box_upper.w = w - output_slider_width;

			// hint box text
			// we do not handle text for dvar matches

			// ---------------------------------------------------------------------

			// if small console is near the bottom or clipped at the bottom, put hintboxes above the output window
			if (!game::native::con->outputVisible && (console_addon_.floating.clipped_at_border.bottom || console_addon_.floating.anker.bottom + console_addon_.items.s_con.hint_box_total.h > console_addon_.viewport_res.height))
			{
				// we calculate the box offsets in DetailedMatch_UpperBox && DetailedMatch_LowerBox
				// but we have to offset Game::conDrawInputGlob->y here. Unfortunately, we do not know the height, so we have to "think" a frame later 
				//Game::conDrawInputGlob->y = conAddon.fltCon.fltAnker.bottom - conAddon.conItems.s_con.inputBox.h - conAddon.conItems.s_con.outputBox.h - conAddon.conItems.s_con.hintBoxTotal.h + conDrawInputGlobY_fuckery + 8.0f;

				const float tmp_top_space = console_addon_.floating.anker.top;
				const float tmp_bottom_space = console_addon_.viewport_res.height - console_addon_.floating.anker.bottom;

				// compare top with bottom
				if (tmp_top_space > tmp_bottom_space)
				{
					// top now offers more space, so move the hintbox above the output box
					game::native::conDrawInputGlob->y = console_addon_.floating.anker.bottom - console_addon_.items.s_con.input_box.h - console_addon_.items.s_con.output_box.h - console_addon_.items.s_con.hint_box_total.h + console_input_glob_y_fuckery_ + 7.0f;

					// calculate the new y
					console_addon_.items.s_con.hint_box_upper.y = console_addon_.floating.anker.bottom - console_addon_.items.s_con.input_box.h - console_addon_.items.s_con.output_box.h - console_addon_.items.s_con.hint_box_upper.h;

					// lets offset text in the to the left if we display hintboxes over the console ;)
					game::native::conDrawInputGlob->x -= 36.0f;
					game::native::conDrawInputGlob->leftX = game::native::conDrawInputGlob->x;
				}
			}

			// we have to subtract from Game::conDrawInputGlob->y because we want to offset the match text in "ConDrawInput_Detailed..." (we add it back when drawing the box)
			// this is because we only hook the box drawing part of "ConDrawInput_Detailed..." and not the text drawing
			game::native::conDrawInputGlob->y -= console_input_glob_y_fuckery_;

			// mid-hook to change box drawing
			// -> ConDrawInput_Box_DetailedMatch_UpperBox && ConDrawInput_Box_DetailedMatch_LowerBox
			game::native::Dvar_ForEachName(game::native::ConDrawInput_DetailedDvarMatch);

			if (!cmd_or_dvar)
			{
				game::native::Cmd_ForEach_PassCmd(draw_detailed_cmd_match);
			}
		}

		// --------------------------------------------------------------------------- 
		// :: Multiple Matches :: Draw with Values

		else
		{
			// y = top of outputbox
			// h = height of outputbox

			// small console --------------------------------------------------------

			// hint box --  we calculate y & h within DetailedMatch_UpperBox && DetailedMatch_LowerBox
			console_addon_.items.s_con.hint_box_upper.x = x;
			console_addon_.items.s_con.hint_box_upper.y = y + h; // bad
			console_addon_.items.s_con.hint_box_upper.w = w;
			console_addon_.items.s_con.hint_box_upper.h = static_cast<float>(game::native::conDrawInputGlob->matchCount) * game::native::conDrawInputGlob->fontHeight + 20.0f;//var_con_currentMatchCount * Game::conDrawInputGlob->fontHeight + 20.0f; // bad

			// hint box text
			// we do not handle text for dvar matches

			// full console --------------------------------------------------------

			// hint box -- we calculate y & h within DetailedMatch_UpperBox && DetailedMatch_LowerBox
			console_addon_.items.f_con.hint_box_upper.x = x;
			console_addon_.items.f_con.hint_box_upper.y = y + h; // bad
			console_addon_.items.f_con.hint_box_upper.w = w - output_slider_width;
			console_addon_.items.f_con.hint_box_upper.h = static_cast<float>(game::native::conDrawInputGlob->matchCount) * game::native::conDrawInputGlob->fontHeight + 20.0f;//var_con_currentMatchCount * Game::conDrawInputGlob->fontHeight + 20.0f; // bad

			// hint box text
			// we do not handle text for dvar matches

			// ---------------------------------------------------------------------

			// check if hintbox goes out of bounds
			if (!game::native::con->outputVisible && (console_addon_.floating.clipped_at_border.bottom || console_addon_.floating.anker.bottom + console_addon_.items.s_con.hint_box_upper.h > console_addon_.viewport_res.height))
			{
				// check if we have more "space" above the console, if not, allow a part of the hintbox to move out of bounds till the top has more space then the bottom
				const float tmp_top_space = console_addon_.floating.anker.top;
				const float tmp_bottom_space = console_addon_.viewport_res.height - console_addon_.floating.anker.bottom;

				// compare top with bottom
				if (tmp_top_space > tmp_bottom_space)
				{
					// top now offers more space, so move the hintbox above the output box
					game::native::conDrawInputGlob->y = console_addon_.floating.anker.bottom - console_addon_.items.s_con.input_box.h - console_addon_.items.s_con.output_box.h - console_addon_.items.s_con.hint_box_upper.h + console_input_glob_y_fuckery_ + 8.0f;

					// calculate the new y
					console_addon_.items.s_con.hint_box_upper.y = console_addon_.floating.anker.bottom - console_addon_.items.s_con.input_box.h - console_addon_.items.s_con.output_box.h - console_addon_.items.s_con.hint_box_upper.h;

					// lets offset text in the to the left if we display hintboxes over the console ;)
					game::native::conDrawInputGlob->x -= 36.0f;
					game::native::conDrawInputGlob->leftX = game::native::conDrawInputGlob->x;
				}
			}

			// we have to subtract from Game::conDrawInputGlob->y because we want to offset the match text in "ConDrawInput_Detailed..." (we add it back when drawing the box)
			// this is because we only hook the box drawing part of "ConDrawInput_Detailed..." and not the text drawing
			game::native::conDrawInputGlob->y -= console_input_glob_y_fuckery_;

			draw_rect(
				/* col */ loc_inputHintBoxColor,
				/*  x  */ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.x : console_addon_.items.s_con.hint_box_upper.x,//x,
				/*  y  */ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.y : console_addon_.items.s_con.hint_box_upper.y,//y + h,
				/*  w  */ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.w : console_addon_.items.s_con.hint_box_upper.w,//w,
				/*  h  */ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.h : console_addon_.items.s_con.hint_box_upper.h,//var_con_currentMatchCount * Game::conDrawInputGlob->fontHeight + 20.0f,
				/* dep */ false,
				/* iFS */ true);

			// hintbox shadow --------------------------------------------------------
			float shadow_color[4] = { 0.0f, 0.0f, 0.0f, 0.125f };

			draw_rect(
				/*    c		*/ shadow_color,
				/*	  x		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.x : console_addon_.items.s_con.hint_box_upper.x,
				/*	  y		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.y : console_addon_.items.s_con.hint_box_upper.y,
				/*	  w		*/ game::native::con->outputVisible ? console_addon_.items.f_con.hint_box_upper.w + output_slider_width : console_addon_.items.s_con.hint_box_upper.w,
				/*	  h		*/ 3.0f,
				/*  depth	*/ false,
				/* ignoreFS	*/ true);

			// crashes...
			//game::native::Dvar_ForEachName(game::native::ConDrawInput_DvarMatch);

			if (!cmd_or_dvar)
			{
				game::native::Cmd_ForEachXO(game::native::ConDrawInput_CmdMatch);
				game::native::Cmd_EndTokenizedString();

				goto REDRAW_CURSOR_RETURN;
			}
		}

		game::native::Cmd_EndTokenizedString();

		// *
		// -----------------
	REDRAW_CURSOR_RETURN:

		// Overdraw the cursor so its above the console so we obv. can not return early ;)
		if ((game::native::ui_context->openMenuCount || dvars::yacc_con_cursorState->current.enabled) && dvars::yacc_con_cursorOverdraw->current.enabled)
		{
			components::_ui::redraw_cursor();
		}
	}

	#pragma endregion

#pragma region CON-ASM // ++++++

	// Replace ConDrawInput_Box in ConDrawInput_DetailedDvarMatch ( mid-hook ) -- upper box
	__declspec(naked) void detailed_dvar_match_stub_01()
	{
		const static uint32_t retn_addr = 0x45C2E6;
		__asm
		{
			push	eax; // overwritten op
			push	esi;
			call	draw_rect_detailed_match_upper;
			pop		esi;
			add		esp, 4h;

			jmp		retn_addr;
		}
	}

	// Replace ConDrawInput_Box in ConDrawInput_DetailedDvarMatch ( mid-hook ) -- lower box
	__declspec(naked) void detailed_dvar_match_stub_02()
	{
		const static uint32_t retn_addr = 0x45C4BF;
		__asm
		{
			push	eax; // overwritten op
			push	esi;
			call	draw_rect_detailed_match_lower;
			pop		esi;
			add		esp, 4h;

			jmp		retn_addr;
		}
	}

	// Replace ConDrawInput_Box in ConDrawInput_DetailedCmdMatch
	__declspec(naked) void detailed_cmd_match_stub()
	{
		const static uint32_t retn_addr = 0x45C68F;
		__asm
		{
			push	1;		// overwritten op
			push	esi;
			call	draw_rect_detailed_match_upper;
			pop		esi;
			add		esp, 4h;

			jmp		retn_addr;
		}
	}

	// helper func to offset matchbox values
	void offset_match_values()
	{
		game::native::conDrawInputGlob->x += 500.0f;
	}

	// offset values for multiple matches, as we increased the maximum amount of dvar chars displayed 
	__declspec(naked) void matchbox_offset_values_stub_01()
	{
		const static uint32_t retn_addr = 0x45BDDB;
		__asm
		{
			pushad;
			call	offset_match_values;
			popad;

			add     esp, 4;		// stock
			jmp		retn_addr;
		}
	}

	// offset values for detailed matches, as we increased the maximum amount of dvar chars displayed 
	__declspec(naked) void matchbox_offset_values_stub_02()
	{
		const static uint32_t retn_addr = 0x45C318;
		__asm
		{
			pushad;
			call	offset_match_values;
			popad;

			add     esp, 4;		// stock
			push    0x6B5F34;	// stock
			sub     esp, 10h;	// stock

			jmp		retn_addr;
		}
	}

	// ^
	__declspec(naked) void matchbox_offset_values_stub_03()
	{
		const static uint32_t retn_addr = 0x45C398;
		__asm
		{
			pushad;
			call	offset_match_values;
			popad;

			add     esp, 4;		// stock
			push    0x6B5F44;	// stock
			sub     esp, 10h;	// stock

			jmp		retn_addr;
		}
	}

	// ^
	__declspec(naked) void matchbox_offset_values_stub_04()
	{
		const static uint32_t retn_addr = 0x45C414;
		__asm
		{
			pushad;
			call	offset_match_values;
			popad;

			add     esp, 4;		// stock
			push    0x6B5F44;	// stock
			sub     esp, 10h;	// stock

			jmp		retn_addr;
		}
	}

	// fully disable cmd autocompletion box drawing
	__declspec(naked) void disable_autocomplete_box_stub()
	{
		const static uint32_t retn_addr = 0x45C703;
		__asm
		{
			jmp		retn_addr;
		}
	}

	// change devmap autocomplete extension to "autocomplete" :: files are in main/maps/mp
	__declspec(naked) void con_devmap_autocompl_stub()
	{
		const static uint32_t retn_addr = 0x523B06;
		__asm
		{
			push	ecx;
			mov		ecx, console_mapdir_;
			mov[eax + 8], ecx;

			mov		ecx, console_mapext_;
			mov[eax + 0Ch], ecx;

			pop		ecx;
			jmp		retn_addr;
		}
	}

	// change map autocomplete extension to "autocomplete" :: files are in main/maps/mp
	__declspec(naked) void con_map_autocompl_stub()
	{
		const static uint32_t retn_addr = 0x5239CD;
		__asm
		{
			push	ecx;

			mov		ecx, console_mapext_;
			mov		ebx, ecx;

			mov     esi, edi;	// stock op inbetween

			mov		ecx, console_mapdir_;
			mov[eax + 8], ecx;

			pop		ecx;
			jmp		retn_addr;
		}
	}

	const char* get_font_for_style(int font_style)
	{
		return	font_style == 0 ? FONT_SMALL_DEV : font_style == 1 ? FONT_BIG_DEV : font_style == 2 ? FONT_CONSOLE :
			font_style == 3 ? FONT_BIG : font_style == 4 ? FONT_SMALL : font_style == 5 ? FONT_BOLD :
			font_style == 6 ? FONT_NORMAL : font_style == 7 ? FONT_EXTRA_BIG : FONT_OBJECTIVE;
	}

	// *
	// minicon drawing (Con_DrawMessageWindowOldToNew)
	void draw_message_window_old_to_new(DWORD* msgWnd, int s_xPos, int s_yPos, int s_charHeight, int s_horzAlign, int s_vertAlign, int s_mode, game::native::Font_s* s_font, const float* s_color, int s_textStyle, float s_msgwndScale, int s_textAlignMode)
	{
		// get font handle
		const auto font_str = get_font_for_style(dvars::con_minicon_font->current.integer);
		const auto font_handle = game::native::R_RegisterFont(font_str, sizeof(font_str));

		game::native::Con_DrawMessageWindowOldToNew(
			msgWnd,
			0,
			static_cast<int>(dvars::con_minicon_position->current.vector[0]),
			static_cast<int>(dvars::con_minicon_position->current.vector[1]),
			dvars::con_minicon_fontHeight->current.integer,
			s_horzAlign,
			s_vertAlign,
			s_mode,
			font_handle,
			dvars::con_minicon_fontColor->current.vector,
			dvars::con_minicon_fontStyle->current.integer,
			s_msgwndScale,
			s_textAlignMode
		);
	}

	// wrapper (the game did not push any args besides esi)
	__declspec(naked) void con_minicon_stub()
	{
		const static uint32_t retn_addr = 0x45D8DA;
		__asm
		{
			push	4;					// textAlignMode
			fstp	dword ptr[esp];		// msgwndScale
			push	3;					// textStyle
			lea     eax, [esp + 1Ch];	// color
			push    eax;				// color
			push    edi;				// font
			push    edx;				// mode
			push    1;					// vert
			push    1;					// horz
			push    12;					// charHeight
			push    4;					// yPos
			push    2;					// xPos
			push	esi;				// esi holds msgWnd

			call	draw_message_window_old_to_new;
			add     esp, 2Ch;

			jmp		retn_addr;				// after <add esp, 30h>
		}
	}
	#pragma endregion

	// called from scheduler as we need 2 hooks at this place
	void new_console::check_resize()
	{
		// ------------------------------------------------------------
		// Cursor toggle key

		// settings
		const int CURSOR_TOGGLE_DELAY = 100;

		// only count till 300, start again when conAddon.cursorToggleTimeout was reset
		if (console_addon_.cursor_toggle_timeout < CURSOR_TOGGLE_DELAY)
		{
			console_addon_.cursor_toggle_timeout++;
		}


		// if console is open
		if (game::native::clientUI->keyCatchers & 1)
		{
			// if F1 and not showing cursor -> activate
			if (game::native::playerKeys->keys[CON_KEY_TOGGLE_CURSOR].down && !dvars::yacc_con_cursorState->current.enabled && console_addon_.cursor_toggle_timeout >= CURSOR_TOGGLE_DELAY)
			{
				console_addon_.cursor_toggle_timeout = 0;
				game::native::dvar_set_value_dirty(dvars::yacc_con_cursorState, true);
				game::native::UI_SetActiveMenu(0, 5);
			}

			// if F1 and showing cursor -> deactivate
			else if (game::native::playerKeys->keys[CON_KEY_TOGGLE_CURSOR].down && dvars::yacc_con_cursorState->current.enabled && console_addon_.cursor_toggle_timeout >= CURSOR_TOGGLE_DELAY)
			{
				goto DISABLE_CURSOR;
			}
		}
		else
		{
			// if console is not open but we still have the cursor menu open, close it
			if (dvars::yacc_con_cursorState->current.enabled)
			{
			DISABLE_CURSOR:
				console_addon_.cursor_toggle_timeout = 0;
				game::native::dvar_set_value_dirty(dvars::yacc_con_cursorState, false);

				game::native::UiContext* ui = &game::native::ui_context[0];
				game::native::Menus_CloseByName("pregame_loaderror_mp", ui);
			}
		}

		// ------------------------------------------------------------

		// one time init on start / vid_restart / cgame changes or w/e
		if (!console_addon_.floating.initialized)
		{
			console_addon_.viewport_res.width = floorf(components::_ui::scrplace_apply_x(HORIZONTAL_APPLY_RIGHT, 0.0f, 0.0f));
			console_addon_.viewport_res.height = floorf(components::_ui::scrplace_apply_y(VERTICAL_APPLY_BOTTOM, 0.0f, 0.0f));

			// min width / height of the floating console
			console_addon_.floating.min_dimensions.width = 840.0f;
			console_addon_.floating.min_dimensions.height = 470.0f;
		}


		// get scaled mouse cursor (640/480 <-> Game Resolution)
		console_addon_.cursor_pos.x = utils::_utils::float_to_range(0.0f, 640.0f, 0.0f, (float)game::native::ui_context->screenWidth, game::native::ui_context->cursor.x);
		console_addon_.cursor_pos.y = utils::_utils::float_to_range(0.0f, 480.0f, 0.0f, (float)game::native::ui_context->screenHeight, game::native::ui_context->cursor.y);

		// enter if floating console state was saved to the config
		// otherwise skip the first frame to fully init the console
		// or skip if the fullscreen console is active
		if ((console_addon_.floating.initialized && !game::native::con->outputVisible) || (dvars::yacc_con_fltCon->current.enabled && !game::native::con->outputVisible))
		{
			// if mouse down && console is actually open -- else use the last floating position
			if (game::native::playerKeys->keys[KEYCATCHER_MOUSE1].down && game::native::clientUI->keyCatchers & 1)
			{
				// ----------------
				// Console Resizing

				// check if the cursor is on the resize button -- we have to check resize first as the button is ontop of the grab bar
				if ((console_addon_.floating.is_resizing || is_cursor_within_item_box_bounds(&console_addon_.items.s_con.resize_button_trigger, &console_addon_.cursor_pos)) && !console_addon_.floating.is_moving)
				{
					// trigger on first click only 
					if (!console_addon_.floating.is_resizing)
					{
						// save the current cursor position for the next frame
						console_addon_.cursor_pos_saved.x = console_addon_.cursor_pos.x;
						console_addon_.cursor_pos_saved.y = console_addon_.cursor_pos.y;

						// the position where we hit the resize button
						console_addon_.cursor_pos_saved_on_click.x = console_addon_.cursor_pos.x;
						console_addon_.cursor_pos_saved_on_click.y = console_addon_.cursor_pos.y;
					}

					if (console_addon_.floating.is_resizing) {
						// can be used to skip something in the "first click frame"
					}

					// resizing console loop
					// ---------------------

					// update width / height
					console_addon_.floating.dimensions.width = console_addon_.floating.anker.right - console_addon_.floating.anker.left;
					console_addon_.floating.dimensions.height = console_addon_.floating.anker.bottom - console_addon_.floating.anker.top;

					// get difference between the cursor of the prior frame and the current pos and resize the console with that
					console_addon_.floating.anker.right -= console_addon_.cursor_pos_saved.x - console_addon_.cursor_pos.x;

					// change outputHeight downwards
					if (console_addon_.cursor_pos.y - console_addon_.cursor_pos_saved_on_click.y >= game::native::con->fontHeight * 0.85f)
					{
						console_addon_.cursor_pos_saved_on_click.y = console_addon_.cursor_pos.y;

						if (dvars::yacc_con_outputHeight->current.integer + 1 <= dvars::yacc_con_outputHeight->domain.integer.max)
						{
							game::native::dvar_set_value_dirty(dvars::yacc_con_outputHeight, dvars::yacc_con_outputHeight->current.integer + 1);
						}
					}

					// change outputHeight upwards
					if (console_addon_.cursor_pos_saved_on_click.y - console_addon_.cursor_pos.y >= game::native::con->fontHeight * 0.85f)
					{
						console_addon_.cursor_pos_saved_on_click.y = console_addon_.cursor_pos.y;

						if (dvars::yacc_con_outputHeight->current.integer - 1 >= dvars::yacc_con_outputHeight->domain.integer.min)
						{
							game::native::dvar_set_value_dirty(dvars::yacc_con_outputHeight, dvars::yacc_con_outputHeight->current.integer - 1);
						}
					}

					// update bottom anker
					console_addon_.floating.anker.bottom = console_addon_.floating.anker.top + console_addon_.items.s_con.output_box.h + console_addon_.items.s_con.input_box.h;

					// ---------------------------------------------------

					// save the current cursor position for the next frame
					console_addon_.cursor_pos_saved.x = console_addon_.cursor_pos.x;
					console_addon_.cursor_pos_saved.y = console_addon_.cursor_pos.y;

					console_addon_.floating.enabled = true;
					console_addon_.floating.is_resizing = true;
				}

				// -------------------
				// Console Translation

				// check if cursor is within the "grab" bar (input text box)
				// skip if the console is being moved
				else if ((console_addon_.floating.is_moving || is_cursor_within_item_box_bounds(&console_addon_.items.s_con.input_box, &console_addon_.cursor_pos)) && !console_addon_.floating.is_resizing)
				{
					// trigger on first click only 
					if (!console_addon_.floating.is_moving)
					{
						// console is now floating
						console_addon_.floating.enabled = true;

						// get initial console position and dimensions if the floating console wasnt in use yet / was reset
						if (!console_addon_.floating.was_in_use || console_addon_.floating.was_reset)
						{
							console_addon_.floating.anker.left = game::native::con->screenMin[0];
							console_addon_.floating.anker.top = game::native::con->screenMin[1];
							console_addon_.floating.anker.right = game::native::con->screenMax[0];
							console_addon_.floating.anker.bottom = game::native::con->screenMax[1];
						}

						// now the floating console will retain its last position even if the fullscreen console was active
						console_addon_.floating.was_in_use = true;
						console_addon_.floating.was_reset = false;

						// calculate width / height
						console_addon_.floating.dimensions.width = console_addon_.floating.anker.right - console_addon_.floating.anker.left;
						console_addon_.floating.dimensions.height = console_addon_.floating.anker.bottom - console_addon_.floating.anker.top;

						// save the current cursor position for the next frame
						console_addon_.cursor_pos_saved.x = console_addon_.cursor_pos.x;
						console_addon_.cursor_pos_saved.y = console_addon_.cursor_pos.y;
					}

					if (console_addon_.floating.is_moving) {
						// can be used to skip something in the "first click frame"
					}

					// draggin console loop
					// ---------------------------------------------------

					// update width / height
					console_addon_.floating.dimensions.width = console_addon_.floating.anker.right - console_addon_.floating.anker.left;
					console_addon_.floating.dimensions.height = console_addon_.floating.anker.bottom - console_addon_.floating.anker.top;

					// update floating console position every frame / resize it by clipping at borders
					// get difference between the cursor of the prior frame and the current pos and translate the console with that

					console_addon_.floating.anker.left -= console_addon_.cursor_pos_saved.x - console_addon_.cursor_pos.x;
					console_addon_.floating.anker.top -= console_addon_.cursor_pos_saved.y - console_addon_.cursor_pos.y;
					console_addon_.floating.anker.right -= console_addon_.cursor_pos_saved.x - console_addon_.cursor_pos.x;

					// ---------------------------------------------------

					// save the current cursor position for the next frame
					console_addon_.cursor_pos_saved.x = console_addon_.cursor_pos.x;
					console_addon_.cursor_pos_saved.y = console_addon_.cursor_pos.y;

					// clip the console against screen bounds
					clip_console_against_borders();

					// ---------------------------------------------------

					// update bottom anker
					console_addon_.floating.anker.bottom = console_addon_.floating.anker.top + console_addon_.items.s_con.output_box.h + console_addon_.items.s_con.input_box.h;

					// console grabbed with cursor (setting this here allows us to skip something in the "first click frame" )
					console_addon_.floating.is_moving = true;
				}
			}

			// non moving floating console loop
			// ---------------------------------------------------

			// if mouse is not hold down anymore after the console was dragged / resized -- save current position to config
			else if (!game::native::playerKeys->keys[KEYCATCHER_MOUSE1].down && (console_addon_.floating.is_moving || console_addon_.floating.is_resizing))
			{
				console_addon_.floating.is_moving = false;
				console_addon_.floating.is_resizing = false;

				// set value doesnt really work here
				game::native::Dvar_SetFloat(dvars::yacc_con_fltConLeft, console_addon_.floating.anker.left, 0);
				game::native::Dvar_SetFloat(dvars::yacc_con_fltConTop, console_addon_.floating.anker.top, 0);
				game::native::Dvar_SetFloat(dvars::yacc_con_fltConRight, console_addon_.floating.anker.right, 0);
				game::native::Dvar_SetFloat(dvars::yacc_con_fltConBottom, console_addon_.floating.anker.bottom, 0);

				game::native::Cmd_ExecuteSingleCommand(game::native::LOCAL_CLIENT_0, 0, "yacc_con_fltCon 1\n");
			}

			// clip the console against screen bounds when not moving the console or when we are resizing it
			if (!console_addon_.floating.is_moving || console_addon_.floating.is_resizing)
			{
				clip_console_against_borders();
			}

			// if floating console was active and did not get reset, activate it to retain its last position
			if ((console_addon_.floating.was_in_use && !console_addon_.floating.was_reset))
			{
				console_addon_.floating.enabled = true;
			}

			// if there is a saved state in the config file -- only on init
			if (dvars::yacc_con_fltCon->current.enabled && !console_addon_.floating.initialized)
			{
				console_addon_.floating.anker.left = dvars::yacc_con_fltConLeft->current.value;
				console_addon_.floating.anker.top = dvars::yacc_con_fltConTop->current.value;
				console_addon_.floating.anker.right = dvars::yacc_con_fltConRight->current.value;
				console_addon_.floating.anker.bottom = dvars::yacc_con_fltConBottom->current.value;

				// update width / height
				console_addon_.floating.dimensions.width = console_addon_.floating.anker.right - console_addon_.floating.anker.left;
				console_addon_.floating.dimensions.height = console_addon_.floating.anker.bottom - console_addon_.floating.anker.top;

				// update conAddon.fltCon.fltAnker.bottom
				//conAddon.fltCon.fltAnker.bottom = conAddon.fltCon.fltAnker.top + conAddon.conItems.s_con.outputBox.h + conAddon.conItems.s_con.inputBox.h;

				console_addon_.floating.enabled = true;
				console_addon_.floating.was_in_use = true;
				console_addon_.floating.was_reset = false;
			}

			// if the console is floating, update x/y and width/height here
			game::native::con->screenMin[0] = console_addon_.floating.anker.left;
			game::native::con->screenMin[1] = console_addon_.floating.anker.top;
			game::native::con->screenMax[0] = console_addon_.floating.anker.right;
			game::native::con->screenMax[1] = console_addon_.floating.anker.bottom;

			// fix for small hintbox (too many to display) getting stuck at the center after fullcon was used
			if (console_addon_.floating.enabled && !console_addon_.floating.is_moving)
			{
				// update bottom anker
				console_addon_.floating.anker.bottom = console_addon_.floating.anker.top + console_addon_.items.s_con.output_box.h + console_addon_.items.s_con.input_box.h;
			}
		}

		// reset floating console to aligned console with the END key while mouse 1 is not pressed
		if (game::native::playerKeys->keys[CON_KEY_RESET_FLTCON].down && !game::native::playerKeys->keys[KEYCATCHER_MOUSE1].down && console_addon_.floating.enabled || game::native::con->outputVisible)
		{
			// disable floating console when the fullscreen console is active
			console_addon_.floating.enabled = false;
			console_addon_.floating.is_moving = false;
			console_addon_.floating.is_resizing = false;

			game::native::Cmd_ExecuteSingleCommand(game::native::LOCAL_CLIENT_0, 0, "yacc_con_fltCon 0\n");

			// only reset the floating console with the reset key and not when the fullscreen console is active
			if (!game::native::con->outputVisible)
			{
				console_addon_.floating.was_reset = true;
			}
		}


		// do not set defaults when we load a saved floating console state on initialization
		// non floating console / fullscreen console position and dimensions
		if ((!console_addon_.floating.initialized && !dvars::yacc_con_fltCon->current.enabled) || !console_addon_.floating.enabled || game::native::con->outputVisible)
		{
			// left HORIZONTAL_APPLY_LEFT
			game::native::con->screenMin[0] = floorf(components::_ui::scrplace_apply_x(HORIZONTAL_APPLY_LEFT, dvars::yacc_con_padding->current.value, 0.0f));

			// top
			game::native::con->screenMin[1] = floorf(components::_ui::scrplace_apply_y(VERTICAL_APPLY_TOP, dvars::yacc_con_padding->current.value, 0.0f));

			// right
			game::native::con->screenMax[0] = floorf(components::_ui::scrplace_apply_x(HORIZONTAL_APPLY_RIGHT, -dvars::yacc_con_padding->current.value, 0.0f));

			// bottom
			game::native::con->screenMax[1] = floorf(components::_ui::scrplace_apply_y(VERTICAL_APPLY_BOTTOM, -dvars::yacc_con_padding->current.value, 0.0f));
		}


		// mouse unrelated stuff ---------------------------------------

		if (const auto	consoleFont = GET_CONSOLEFONT;
			consoleFont)
		{
			if (consoleFont->pixelHeight != 16)
			{
				game::native::Com_PrintMessage(0, utils::string::va("Console :: consoleFont was %d", consoleFont->pixelHeight), 0);
			}

			game::native::con->fontHeight = static_cast<int>(static_cast<float>(consoleFont->pixelHeight) * dvars::yacc_con_fontSpacing->current.value);

			// adjust visibleLineCount for output text so we do not draw more lines then our rect can hold
			if (game::native::con->fontHeight)
			{
				if (game::native::con->outputVisible)
				{
					// full console
					game::native::con->visibleLineCount = static_cast<int>((console_addon_.items.f_con.output_box.h - static_cast<float>(game::native::con->fontHeight * 2)) / static_cast<float>(game::native::con->fontHeight));
				}
				else
				{
					// small console
					game::native::con->visibleLineCount = static_cast<int>((console_addon_.items.s_con.output_box.h - static_cast<float>(game::native::con->fontHeight)) / static_cast<float>(game::native::con->fontHeight));
				}
			}
			else
			{
				#if DEBUG
					game::native::Com_PrintMessage(0, utils::string::va("^1Con_CheckResize L#%d ^7:: con->fontHeight was NULL \n", __LINE__), 0);
				#endif
			}

			game::native::con->visiblePixelWidth = static_cast<int>(game::native::con->screenMax[0] - game::native::con->screenMin[0] - -28.0f);
		}

		else
		{
			game::native::con->fontHeight = 0;
			game::native::con->visibleLineCount = 0;
			game::native::con->visiblePixelWidth = 0;
		}

		if (!console_addon_.floating.initialized && !console_addon_.floating.enabled)
		{
			// print help text on init
			game::native::Com_PrintMessage(0, "\n", 0);
			game::native::Com_PrintMessage(0, utils::string::va("%s", CON_HELP_PRINT), 0);
		}

		// floating console initialized
		console_addon_.floating.initialized = true;
	}
	
	void new_console::post_load()
	{

		new_console::using_custom_console = true;
		// -----
		// DVARS

		dvars::con_minicon_position = game::native::Dvar_RegisterVec2(
			/* name		*/ "con_minicon_position",
			/* desc		*/ "minicon position (int)",
			/* x		*/ 115.0f,
			/* y		*/ 17.0f,
			/* minValue	*/ 0.0f,
			/* maxValue	*/ 640.0f,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::con_minicon_font = game::native::Dvar_RegisterInt(
			/* name		*/ "con_minicon_font",
			/* desc		*/ "minicon font",
			/* default	*/ 8,
			/* minVal	*/ 0,
			/* maxVal	*/ 8,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::con_minicon_fontHeight = game::native::Dvar_RegisterInt(
			/* name		*/ "con_minicon_fontHeight",
			/* desc		*/ "minicon char height",
			/* default	*/ 11,
			/* minVal	*/ 0,
			/* maxVal	*/ 64,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::con_minicon_fontColor = game::native::Dvar_RegisterVec4(
			/* name		*/ "con_minicon_fontColor",
			/* desc		*/ "minicon font color",
			/* x		*/ 1.0f,
			/* y		*/ 1.0f,
			/* z		*/ 1.0f,
			/* w		*/ 1.0f,
			/* minValue	*/ 0.0f,
			/* maxValue	*/ 1.0f,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::con_minicon_fontStyle = game::native::Dvar_RegisterInt(
			/* name		*/ "con_minicon_fontStyle",
			/* desc		*/ "3 = <renderFlags 4>; 6 = <renderFlags 12>; 128 = <renderFlags 1>",
			/* default	*/ 3,
			/* minVal	*/ 0,
			/* maxVal	*/ 128,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::yacc_con_fltCon = game::native::Dvar_RegisterBool(
			/* name		*/ "yacc_con_fltCon",
			/* desc		*/ "console :: floating console state",
			/* default	*/ false,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::yacc_con_fltConLeft = game::native::Dvar_RegisterFloat(
			/* name		*/ "yacc_con_fltConLeft",
			/* desc		*/ "console :: floating console left anker",
			/* default	*/ 0.0f,
			/* minVal	*/ -7680.0f,
			/* maxVal	*/ 7680.0f,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::yacc_con_fltConTop = game::native::Dvar_RegisterFloat(
			/* name		*/ "yacc_con_fltConTop",
			/* desc		*/ "console :: floating console top anker",
			/* default	*/ 0.0f,
			/* minVal	*/ -4320.0f,
			/* maxVal	*/ 4320.0f,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::yacc_con_fltConRight = game::native::Dvar_RegisterFloat(
			/* name		*/ "yacc_con_fltConRight",
			/* desc		*/ "console :: floating console right anker",
			/* default	*/ 0.0f,
			/* minVal	*/ -7680.0f,
			/* maxVal	*/ 7680.0f,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::yacc_con_fltConBottom = game::native::Dvar_RegisterFloat(
			/* name		*/ "yacc_con_fltConBottom",
			/* desc		*/ "console :: floating console bottom anker",
			/* default	*/ 0.0f,
			/* minVal	*/ -4320.0f,
			/* maxVal	*/ 4320.0f,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::yacc_con_outputHeight = game::native::Dvar_RegisterInt(
			/* name		*/ "yacc_con_outputHeight",
			/* desc		*/ "console :: height / lines of console output.",
			/* default	*/ 8,
			/* minVal	*/ 1,
			/* maxVal	*/ 30,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::yacc_con_maxMatches = game::native::Dvar_RegisterInt(
			/* name		*/ "yacc_con_maxMatches",
			/* desc		*/ "console :: maximum amout of matches to show till \"too many to show here\" is drawn.",
			/* default	*/ 24,
			/* minVal	*/ 0,
			/* maxVal	*/ 50,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::yacc_con_useDepth = game::native::Dvar_RegisterBool(
			/* name		*/ "yacc_con_useDepth",
			/* desc		*/ "console :: use scene depth as inputBox background.",
			/* default	*/ false,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::yacc_con_padding = game::native::Dvar_RegisterFloat(
			/* name		*/ "yacc_con_padding",
			/* desc		*/ "console :: distance screen border <-> console (all sides)",
			/* default	*/ 0.0f,
			/* minVal	*/ -2000.0f,
			/* maxVal	*/ 2000.0f,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::yacc_con_fontSpacing = game::native::Dvar_RegisterFloat(
			/* name		*/ "yacc_con_fontSpacing",
			/* desc		*/ "console output :: space between lines",
			/* default	*/ 0.8f,
			/* minVal	*/ 0.8f,
			/* maxVal	*/ 1.0f,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::yacc_con_cursorOverdraw = game::native::Dvar_RegisterBool(
			/* name		*/ "yacc_con_cursorOverdraw",
			/* desc		*/ "console :: redraw the menu cursor so its above the console.",
			/* default	*/ true,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::yacc_con_cursorState = game::native::Dvar_RegisterBool(
			/* name		*/ "yacc_con_cursorState",
			/* desc		*/ "console :: current state of the cursor.",
			/* default	*/ false,
			/* flags	*/ game::native::dvar_flags::dvar_none);

		dvars::yacc_con_hintBoxTxtColor_currentDvar = game::native::Dvar_RegisterVec4(
			/* name		*/ "yacc_con_hintBoxTxtColor_currentDvar",
			/* desc		*/ "console :: color of dvar names in the hintbox",
			/* x		*/ 0.8f,
			/* y		*/ 0.8f,
			/* z		*/ 1.0f,
			/* w		*/ 1.0f,
			/* minValue	*/ 0.0f,
			/* maxValue	*/ 1.0f,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::yacc_con_hintBoxTxtColor_currentValue = game::native::Dvar_RegisterVec4(
			/* name		*/ "yacc_con_hintBoxTxtColor_currentValue",
			/* desc		*/ "console :: color of dvar values in the hintbox",
			/* x		*/ 0.8f,
			/* y		*/ 0.8f,
			/* z		*/ 1.0f,
			/* w		*/ 1.0f,
			/* minValue	*/ 0.0f,
			/* maxValue	*/ 1.0f,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::yacc_con_hintBoxTxtColor_defaultValue = game::native::Dvar_RegisterVec4(
			/* name		*/ "yacc_con_hintBoxTxtColor_defaultValue",
			/* desc		*/ "console :: color of default dvar value in the hintbox",
			/* x		*/ 0.8f,
			/* y		*/ 0.8f,
			/* z		*/ 1.0f,
			/* w		*/ 1.0f,
			/* minValue	*/ 0.0f,
			/* maxValue	*/ 1.0f,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::yacc_con_hintBoxTxtColor_dvarDescription = game::native::Dvar_RegisterVec4(
			/* name		*/ "yacc_con_hintBoxTxtColor_dvarDescription",
			/* desc		*/ "console :: color of dvar description in the hintbox",
			/* x		*/ 0.8f,
			/* y		*/ 0.8f,
			/* z		*/ 1.0f,
			/* w		*/ 1.0f,
			/* minValue	*/ 0.0f,
			/* maxValue	*/ 1.0f,
			/* flags	*/ game::native::dvar_flags::saved);

		dvars::yacc_con_hintBoxTxtColor_domainDescription = game::native::Dvar_RegisterVec4(
			/* name		*/ "yacc_con_hintBoxTxtColor_domainDescription",
			/* desc		*/ "console :: color of dvar domain in the hintbox",
			/* x		*/ 0.8f,
			/* y		*/ 0.8f,
			/* z		*/ 1.0f,
			/* w		*/ 1.0f,
			/* minValue	*/ 0.0f,
			/* maxValue	*/ 1.0f,
			/* flags	*/ game::native::dvar_flags::saved);

		// -----
		// Hooks


		// Minicon drawing - wrapper to proxy
		utils::hook::nop(0x45D8B8, 6);
		utils::hook(0x45D8B8, con_minicon_stub, HOOK_JUMP).install()->quick(); // in Con_DrawMiniConsole

		// Con_CheckResize - main position
		utils::hook(0x45AE5F, check_resize_on_init, HOOK_CALL).install()->quick(); // in Con_OneTimeInit
		utils::hook(0x46907B, check_resize_on_init, HOOK_CALL).install()->quick(); // in CL_InitRenderer


		// Con_DrawOuputWindow -- in "Con_DrawSolidConsole" before "Con_DrawInput"
		utils::hook(0x45E058, draw_output_window, HOOK_CALL).install()->quick(); // no flicker but latched value bug on vec4

		// Con_DrawInput ( 2x IfElse )
		utils::hook(0x45E045, draw_input, HOOK_CALL).install()->quick();
		utils::hook(0x45E05E, draw_input, HOOK_CALL).install()->quick(); // our active func

		// Disable the need for Forward/Backslash for console cmds
		utils::hook::nop(0x46391F, 5);

		// Replace ConDrawInput_Box in ConDrawInput_DetailedDvarMatch ( mid-hook ) -- upper box
		utils::hook(0x45C2E0, detailed_dvar_match_stub_01, HOOK_CALL).install()->quick();
		utils::hook::nop(0x45C2E5, 1);

		// Replace ConDrawInput_Box in ConDrawInput_DetailedDvarMatch ( mid-hook ) -- lower box
		utils::hook(0x45C4B9, detailed_dvar_match_stub_02, HOOK_CALL).install()->quick();
		utils::hook::nop(0x45C4BE, 1);

		// Replace ConDrawInput_Box in ConDrawInput_DetailedCmdMatch
		utils::hook(0x45C688, detailed_cmd_match_stub, HOOK_CALL).install()->quick();
		utils::hook::nop(0x45C68D, 2);

		// Increase max chars to draw for dvar matches
		utils::hook::set<BYTE>(0x45BDBA, 0x40); // Stock 0x18

		// Increase draw width for matched dvar - dvar string
		utils::hook::set<BYTE>(0x45C2F2, 0x40); // Stock 0x18

		// Increase distance to matched dvars as we increased the maximum amount of chars for found matches
		utils::hook::nop(0x45BDC5, 6); // nop the instruction first, then install our jmp
		utils::hook(0x45BDC5, matchbox_offset_values_stub_01, HOOK_JUMP).install()->quick();

		// same for detailed match (3 times for current, latched, default)
		utils::hook::nop(0x45C2FB, 6); // nop the instruction first, then install our jmp
		utils::hook(0x45C2FB, matchbox_offset_values_stub_02, HOOK_JUMP).install()->quick();

		utils::hook::nop(0x45C37B, 6); // nop the instruction first, then install our jmp
		utils::hook(0x45C37B, matchbox_offset_values_stub_03, HOOK_JUMP).install()->quick();

		utils::hook::nop(0x45C3F7, 6); // nop the instruction first, then install our jmp
		utils::hook(0x45C3F7, matchbox_offset_values_stub_04, HOOK_JUMP).install()->quick();

		// fully disable cmd autocomplete box with dir searching
		utils::hook::nop(0x45C6CC, 6); utils::hook(0x45C6CC, disable_autocomplete_box_stub, HOOK_JUMP).install()->quick();
	}

REGISTER_MODULE(new_console);