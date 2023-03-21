#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include "game/game.hpp"

#include "news.hpp"
#include "console.hpp"
#include "localization.hpp"

#define NEWS_MOTD_DEFAULT "Welcome to YACC!"

bool news::Terminate;
std::thread news::Thread;

const char* news::GetNewsText()
{
	return NEWS_MOTD_DEFAULT;//Localization::Get("MPUI_MOTD_TEXT");
}

void news::post_load()
{
	game::native::Dvar_RegisterBool("g_firstLaunch", "", true, game::native::DVAR_FLAG_SAVED);

	localization::Set("MPUI_CHANGELOG_TEXT", "Loading...");
	localization::Set("MPUI_MOTD_TEXT", NEWS_MOTD_DEFAULT);

	// make newsfeed (ticker) menu items not cut off based on safe area
	//Utils::Hook::Nop(0x63892D, 5);

	// hook for getting the news ticker string
	//Utils::Hook::Nop(0x6388BB, 2); // skip the "if (item->text[0] == '@')" localize check
	//Utils::Hook(0x6388C1, News::GetNewsText, HOOK_CALL).install()->quick();

	news::Terminate = false;
	news::Thread = std::thread([]()
	{
		if (news::Terminate) return;

		std::string data = "^7This is a test ^5MOTD"s;
		if (!data.empty())
		{
			//Localization::Set("MPUI_MOTD_TEXT", data);
		}

		if (!news::Terminate)
		{
			while (!news::Terminate)
			{
				// Sleep for 3 minutes
				for (int i = 0; i < 180 && !news::Terminate; ++i)
				{
					std::this_thread::sleep_for(1s);
				}
			}
		}
	});
}

void news::pre_destroy()
{
	news::Terminate = true;

	if (news::Thread.joinable())
	{
		news::Thread.join();
	}

	news::Thread = std::thread();
}

REGISTER_MODULE(news);