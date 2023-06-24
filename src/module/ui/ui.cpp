#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include "game/game.hpp"
#include <module/setup/console.hpp>
#include <module/setup/scheduler.hpp>
#include "version.hpp"

#include <utils/concurrency.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <game/dvars.hpp>
#include <module/gameplay/command.hpp>
#include <utils/io.hpp>
#include <module/assets/localization.hpp>
#include <module/assets/asset_handler.hpp>

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

		dvars::ui_ultrawide = game::native::Dvar_RegisterBool(
			/* name		*/ "ui_ultrawide",
			/* desc		*/ "menu helper",
			/* default	*/ false,
			/* flags	*/ game::native::dvar_flags::read_only);

		command::add("ui_debugmode", "<boolean>", "Draw ui debug info on the screen", [](command::params params)
		{
			if (params.length() < 2)
			{
				game::native::Com_PrintMessage(0, "Usage :: ui_debugmode <boolean>\n", 0);
				return;
			}

			bool uiDebugValue;

			std::istringstream(params[1]) >> uiDebugValue;

			utils::hook::set<bool>(0xD5E44CC, uiDebugValue);
		});
	}

	void post_unpack() override
	{
		localization::Set("MPUI_WELCOME", "Welcome to the YACC COD4 ^3Alpha");
		localization::Set("MPUI_CHOOSE_LOADOUT", "CHOOSE LOADOUT");
	}
};

REGISTER_MODULE(ui)