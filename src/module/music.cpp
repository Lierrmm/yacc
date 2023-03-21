#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include <game/game.hpp>

#include "music.hpp"
#include <utils/string.hpp>
#include "asset_handler.hpp"


std::unordered_map<std::string, const char*> music::SoundAliasList;

void music::Replace(const std::string& sound, const char* file)
{
	music::SoundAliasList[utils::string::to_lower(sound)] = file;
}

game::native::XAssetHeader music::ModifyAliases(game::native::XAssetType type, const std::string& filename)
{
	game::native::XAssetHeader header = { nullptr };

	if (music::SoundAliasList.find(utils::string::to_lower(filename)) != music::SoundAliasList.end())
	{
		game::native::snd_alias_list_t* aliases = game::native::DB_FindXAssetHeader(type, filename.data()).sound;

		if (aliases && aliases->count > 0 && aliases->head && aliases->head->soundFile)
		{
			if (aliases->head->soundFile->type == game::native::snd_alias_type_t::SAT_STREAMED)
			{
				aliases->head->soundFile->sound.streamSnd.filename.info.raw.name = music::SoundAliasList[utils::string::to_lower(filename)];
			}

			header.sound = aliases;
		}
	}

	return header;
}

void music::post_load()
{
	AssetHandler::OnFind(game::native::XAssetType::ASSET_TYPE_SOUND, music::ModifyAliases);

	music::Replace("music_mainmenu_mp", "HGW_gameshell_TMP_v9.mp3");
}

//REGISTER_MODULE(music);