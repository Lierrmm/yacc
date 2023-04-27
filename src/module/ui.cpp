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
#include <utils/io.hpp>
#include "localization.hpp"
#include "asset_handler.hpp"

game::native::dvar_t* g_dump_scripts;
game::native::dvar_t* g_dump_images;

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

game::native::GfxImage* findImage(game::native::Material* material, const std::string& type)
{
	game::native::GfxImage* image = nullptr;

	const auto hash = game::native::R_HashString(type.data());

	for (char l = 0; l < material->textureCount; ++l)
	{
		if (material->textureTable[l].nameHash == hash)
		{
			image = material->textureTable[l].u.image;
			break;
		}
	}

	return image;
}

game::native::GfxImage* extractImage(game::native::Material* material, const std::string& type)
{
	auto* image = findImage(material, type);

	if (!image)
	{
		return image;
	}

	if (image && std::string(image->name).length() > 0)
	{
		std::string _name = utils::string::va("raw/imagedump/%s.png", image->name);
		D3DXSaveTextureToFileA(_name.data(), D3DXIFF_PNG, image->texture.map, nullptr);
	}

	return image;
}

void dump_images_from_material(const std::string& matName, game::native::XAssetHeader header)
{
	if (!g_dump_images->current.enabled)
	{
		return;
	}

	auto material = header.material;

	extractImage(material, "colorMap");
	extractImage(material, "normalMap");
	extractImage(material, "specularMap");

	console::info("Dumped Image %s\n", matName.data());
}

void dump_raw_image(const std::string& imageName, game::native::XAssetHeader header)
{
	if (!g_dump_images->current.enabled)
	{
		return;
	}

	auto image = header.image;

	if (image && std::string(image->name).length() > 0)
	{
		std::string _name = utils::string::va("raw/imagedump/%s.png", image->name);
		D3DXSaveTextureToFileA(_name.data(), D3DXIFF_PNG, image->texture.map, nullptr);
		console::info("Dumped Raw Image %s\n", imageName.data());
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
		utils::hook::set<const char*>(0x53DBBC, "");
		utils::hook::set<const char*>(0x45C8F1, utils::string::va("YACC %s > ", VERSION));
		utils::hook::set<DWORD>(0x45DE94, (DWORD)(YACC_BUILDSTRING));

		g_dump_images = game::native::Dvar_RegisterBool("g_dump_images", "Dump images to raw/imagedump", false, game::native::dvar_flags::dvar_none);
		AssetHandler::OnFind(game::native::XAssetType::ASSET_TYPE_IMAGE, [](game::native::XAssetType, const std::string& _filename)
		{
			game::native::XAssetHeader header = AssetHandler::FindOriginalAsset(game::native::XAssetType::ASSET_TYPE_IMAGE, _filename.data());
			if (g_dump_images->current.enabled)
			{
				dump_raw_image(_filename, header);
			}

			return header;
		});

		AssetHandler::OnFind(game::native::XAssetType::ASSET_TYPE_MATERIAL, [](game::native::XAssetType, const std::string& _filename)
			{
				game::native::XAssetHeader header = AssetHandler::FindOriginalAsset(game::native::XAssetType::ASSET_TYPE_MATERIAL, _filename.data());
				if (g_dump_images->current.enabled)
				{
					dump_images_from_material(_filename, header);
				}

				return header;
			});

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
	}

	void post_unpack() override
	{
		localization::Set("MPUI_WELCOME", "Welcome to the YACC COD4 ^3Alpha");
		localization::Set("MPUI_CHOOSE_LOADOUT", "CHOOSE LOADOUT");
	}
};

REGISTER_MODULE(ui)