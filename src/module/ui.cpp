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
#include "command.hpp"

game::native::dvar_t* g_dump_scripts;
game::native::dvar_t* g_dump_images;
game::native::dvar_t* db_print_default_assets;

utils::hook::detour db_find_xasset_header_hook;

void UI_DrawBuildString()
{
	int ctx; // client id
	float size; 
	game::native::Font_s* font;
	float fontscale;
	float ypos;
	float xpos;
	const char* buildString;

	if (game::native::ui_context->openMenuCount > 2)
	{
		return;
	}
	if (game::native::ui_context->openMenuCount == 2 && strcmp(game::native::ui_context->menuStack[1]->window.name, "main_text") != 0)
	{
		return;
	}

	auto buildSize = game::native::Dvar_FindVar("ui_buildSize");
	auto smallFont = game::native::Dvar_FindVar("ui_smallFont");
	auto extraBigFont = game::native::Dvar_FindVar("ui_extraBigFont");
	auto bigFont = game::native::Dvar_FindVar("ui_bigFont");
	fontscale = buildSize->current.value;

	ctx = 0;
	size = game::native::scrPlace[ctx].scaleVirtualToReal[1] * fontscale;

	if (smallFont->current.value >= size) {
		font = game::native::R_RegisterFont(FONT_SMALL, sizeof(FONT_SMALL));
	}
	else if (extraBigFont->current.value <= size) {
		font = game::native::R_RegisterFont(FONT_EXTRA_BIG, sizeof(FONT_EXTRA_BIG));
	}
	else if (bigFont->current.value > size) {
		font = game::native::R_RegisterFont(FONT_NORMAL, sizeof(FONT_NORMAL));
	}
	else {
		font = game::native::R_RegisterFont(FONT_BIG, sizeof(FONT_BIG));
	}

	auto buildLocation = game::native::Dvar_FindVar("ui_buildLocation");

	ypos = buildLocation->current.vector[1];
	xpos = 450;
	ypos -= 5.0;

#ifdef DEBUG
	xpos -= 20.0;
	buildString = utils::string::va("%s %s\n", "YACC Debug:", VERSION);
#else
	buildString = utils::string::va("%s %s\n", "YACC:", VERSION);
#endif

	const float color_foreground[4] = { 1.0f, 0.8f, 0.7f, 1.0f };
	//			client, string, length, font, x, y, horizontal, vertical, scale, color, style
	UI_DrawText(&game::native::scrPlace[ctx], buildString, 64, font, xpos, ypos, 3, 0, fontscale, color_foreground, 0);

	ypos += game::native::UI_TextHeight(font, fontscale);
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
		*(float*)(0xCC8F640) = dvars::r_aspectRatio_custom->current.value;
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

void __cdecl SCR_DrawSmallStringExt(signed int x, signed int y, const char* string, const float* setColor)
{
	float th;

	auto consoleFont = game::native::R_RegisterFont(FONT_CONSOLE, sizeof(FONT_CONSOLE));
	th = (float)game::native::R_TextHeight(consoleFont);
	game::native::R_AddCmdDrawText(string, 0x7FFFFFFF, consoleFont, (float)x, (float)y + th, 1.0, 1.0, 0.0, setColor, 0);
}

void Con_DrawBuildString(float x, float y, float y2)
{
	auto buildString = utils::string::va("%s %s\n", "YACC Debug:", VERSION);
	float colorYellow[4] = { 1, 1, 0, 1 };
	float ydraw = y2 - 16.0 + y;
	SCR_DrawSmallStringExt((signed int)x, (signed int)ydraw, buildString, colorYellow);
}

//void dump_gsc_script(const std::string& name, game::native::XAssetHeader header)
//{
//	if (!g_dump_scripts->current.enabled)
//	{
//		return;
//	}
//
//	std::string buffer;
//	buffer.append(header.data->name, strlen(header.scriptfile->name) + 1);
//	buffer.append(reinterpret_cast<char*>(&header.scriptfile->compressedLen), 4);
//	buffer.append(reinterpret_cast<char*>(&header.scriptfile->len), 4);
//	buffer.append(reinterpret_cast<char*>(&header.scriptfile->bytecodeLen), 4);
//	buffer.append(header.scriptfile->buffer, header.scriptfile->compressedLen);
//	buffer.append(header.scriptfile->bytecode, header.scriptfile->bytecodeLen);
//
//	const auto out_name = utils::string::va("gsc_dump/%s.gscbin", name.data());
//	utils::io::write_file(out_name, buffer);
//
//	console::info("Dumped %s\n", out_name);
//}

game::native::XAssetHeader db_find_xasset_header_stub(game::native::XAssetType type, const char* name)
{
	auto result = db_find_xasset_header_hook.invoke<game::native::XAssetHeader>(type, name);

	/*if (type == game::native::XAssetType::ASSET_TYPE_MENU)
	{
		dump_gsc_script(name, result);
	}*/

	if (type == game::native::XAssetType::ASSET_TYPE_MATERIAL)
	{
		if (!strcmp(name, "loadscreen"))
		{
			console::info("Found LOADSCREEN! %s\n", name);
		}
	}

	if (type == game::native::XAssetType::ASSET_TYPE_RAWFILE)
	{
		if (result.rawfile)
		{
			const std::string override_rawfile_name = "override/"s + name;
			const auto override_rawfile = db_find_xasset_header_hook.invoke<game::native::XAssetHeader>(type, override_rawfile_name.data());
			if (override_rawfile.rawfile)
			{
				result.rawfile = override_rawfile.rawfile;
				console::info("using override asset for rawfile: \"%s\"\n", name);
			}
		}
	}

	return result;
}

class ui final : public module
{
public:
	void post_start() override
	{

	}

	void post_load() override
	{
		utils::hook(0x53DB10, UI_DrawBuildString, HOOK_JUMP).install()->quick();

		/* Build Number */
		utils::hook(0x45DF84, Con_DrawBuildString, HOOK_JUMP).install()->quick();

		// open / re-open the specified menu from uicontext->menus
		command::add("menu_open", "<menu_name>", "open / re-open the specified menu from uicontext->menus", [](command::params params)
		{
			if (params.length() < 2)
			{
				game::native::Com_PrintMessage(0, "Usage :: menu_open <menu_name>\n", 0);
				return;
			}

			if (!game::native::ui_context || !game::native::clientUI)
			{
				game::native::Com_PrintMessage(0, "uiContext | clientUI was null\n", 0);
				return;
			}

			const char* name = params[1];
			game::native::UiContext* ui = &game::native::ui_context[0];
			game::native::clientUI->displayHUDWithKeycatchUI = true;

			game::native::Menus_CloseByName(name, ui);
			game::native::Menus_OpenByName(name, ui);
		});


		command::add("menu_open_ingame", "<menu_name>", "wip", [](command::params params)
		{
			if (const auto& cl_ingame = game::native::Dvar_FindVar("cl_ingame"); cl_ingame)
			{
				if (cl_ingame->current.enabled && game::native::ui_cg_dc)
				{
					const char* name = params[1];
					game::native::UiContext* ui = &game::native::ui_cg_dc[0];

					game::native::Key_SetCatcher();
					game::native::Menus_OpenByName(name, ui);
				}
				else
				{
					game::native::Com_PrintMessage(0, "^1Not in-game\n", 0);
				}
			}
		});

		command::add("menu_closebyname", "<menu_name>", "close the specified menu", [](command::params params)
		{
			if (params.length() < 2)
			{
				game::native::Com_PrintMessage(0, "Usage :: menu_closebyname <menu_name>\n", 0);
				return;
			}

			if (!game::native::ui_context)
			{
				game::native::Com_PrintMessage(0, "uiContext was null\n", 0);
				return;
			}

			const char* name = params[1];
			game::native::Menus_CloseByName(name, game::native::ui_context);


			if (const auto& cl_ingame = game::native::Dvar_FindVar("cl_ingame"); cl_ingame)
			{
				if (cl_ingame->current.enabled && game::native::ui_cg_dc)
				{
					game::native::Menus_CloseByName(name, game::native::ui_cg_dc);
				}
			}
		});

		// *
		// Display

		// hook R_AspectRatio to initially reset the ultrawide dvar (menu helper)
		utils::hook::nop(0x5D318E, 6);		utils::hook(0x5D318E, aspect_ratio_custom_reset_stub, HOOK_JUMP).install()->quick();

		// Set custom aspect ratio by using the default switchcase in R_AspectRatio
		utils::hook::nop(0x5D325B, 6);		utils::hook(0x5D325B, aspect_ratio_custom_stub, HOOK_JUMP).install()->quick();

		//dvars::r_aspectRatio_custom = game::native::Dvar_RegisterFloat(
		//	/* name		*/ "r_aspectRatio_custom",
		//	/* desc		*/ "description",
		//	/* default	*/ 2.3333333f,
		//	/* minVal	*/ 0.1f,
		//	/* maxVal	*/ 10.0f,
		//	/* flags	*/ game::native::dvar_flags::saved);

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

		db_find_xasset_header_hook.create(game::native::DB_FindXAssetHeader, db_find_xasset_header_stub);

		db_print_default_assets = game::native::Dvar_RegisterBool("db_printDefaultAssets", "Print default asset usage", false, game::native::DVAR_FLAG_SAVED);
	}
};



REGISTER_MODULE(ui)