#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include "game/game.hpp"

#include "console.hpp"
#include "network.hpp"
#include "scheduler.hpp"

#include <utils/string.hpp>
#include <utils/cryptography.hpp>

#include <discord_rpc.h>

namespace 
{
	DiscordRichPresence discord_presence;

	void join_game(const char* join_secret)
	{
		game::native::Cbuf_AddText(utils::string::va("connect %s\n", join_secret), 0);
	}

	void join_request(const DiscordUser* request)
	{
#ifdef _DEBUG
		console::info("Discord: Join request from %s (%s)\n", request->username, request->userId);
#endif
		Discord_Respond(request->userId, DISCORD_REPLY_IGNORE);
	}

	void update_discord()
	{
		Discord_RunCallbacks();

		if (!*game::native::CL_IsCgameInitialized)
		{
			discord_presence.details = "Multiplayer";
			discord_presence.state = "Main Menu";

			discord_presence.partySize = 0;
			discord_presence.partyMax = 0;

			discord_presence.startTimestamp = 0;

			discord_presence.largeImageKey = "menu_multiplayer";
		}
		else
		{			
			discord_presence.details = utils::string::va("%s on %s", game::native::UI_GetCurrentGameType(), game::native::UI_GetCurrentMapName());

			discord_presence.partySize = game::native::cgs->snap != nullptr
				? game::native::cgs->snap->numClients
				: 1;

			auto* host_name = reinterpret_cast<char*>(0x742948);
			utils::string::strip(host_name, host_name, std::strlen(host_name) + 1);

			discord_presence.state = host_name;
			discord_presence.partyMax = game::native::Dvar_FindVar("sv_maxclients")->current.integer;

			/*std::hash<game::native::netadr_t> hash_fn;
			static const auto nonce = utils::cryptography::random::get_integer();
			
			game::native::netadr_t address{};
			address = *game::native::serverAddress;
			if (address.type != game::native::NA_BAD || address.type != game::native::NA_LOOPBACK)
			{
				auto _address = network::Address(address);
				discord_presence.partyId = utils::string::va("%zu", hash_fn(address) ^ nonce);
				discord_presence.joinSecret = _address.getCString();
				discord_presence.partyPrivacy = DISCORD_PARTY_PUBLIC;
			}*/
			

			if (!discord_presence.startTimestamp)
			{
				discord_presence.startTimestamp = std::chrono::duration_cast<std::chrono::seconds>(
					std::chrono::system_clock::now().time_since_epoch()).count();
			}

			discord_presence.largeImageKey = utils::string::va("loadscreen_%s", game::native::Dvar_FindVar("mapname")->current.string);
			discord_presence.smallImageKey = "menu_multiplayer";
		}

		Discord_UpdatePresence(&discord_presence);
	}
}

class discord final : public module
{
public:
	void post_load() override
	{
		DiscordEventHandlers handlers;
		ZeroMemory(&handlers, sizeof(handlers));
		handlers.ready = ready;
		handlers.errored = errored;
		handlers.disconnected = errored;
		handlers.joinGame = join_game;
		handlers.spectateGame = nullptr;
		handlers.joinRequest = join_request;

		Discord_Initialize("1084807026217078855", &handlers, 1, nullptr);

		scheduler::on_frame(update_discord, scheduler::async);

		initialized_ = true;
	}

	void pre_destroy() override
	{
		if (!initialized_)
		{
			return;
		}

		Discord_Shutdown();
	}


private:
	bool initialized_ = false;

	static void ready(const DiscordUser* /*request*/)
	{
		ZeroMemory(&discord_presence, sizeof(discord_presence));

		discord_presence.instance = 1;

		Discord_UpdatePresence(&discord_presence);
	}

	static void errored(const int error_code, const char* message)
	{
		console::error("Discord: (%i) %s", error_code, message);
	}
};

REGISTER_MODULE(discord)