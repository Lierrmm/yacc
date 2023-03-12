#pragma once

#include "game/game.hpp"

namespace game_console
{
	void print(int type, const std::string& data);

	bool console_char_event(int local_client_num, int key);
	bool console_key_event(game::native::LocalClientNum_t localClientNum, int key, int down);

	bool match_compare(const std::string& input, const std::string& text, const bool exact);
	void find_matches(std::string input, std::vector<std::string>& suggestions, const bool exact);
}
