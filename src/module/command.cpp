#include <std_include.hpp>
#include <loader/module_loader.hpp>

#include <game/game.hpp>
#include <utils/string.hpp>
#include "command.hpp"

std::vector<game::native::cmd_function_s*> command::functions;
std::map<std::string, utils::string::Slot<command::callback>> command::function_map;

const char* command::params::operator[](size_t index)
{
	if (index >= this->length())
	{
		return "";
	}

	return game::native::cmd_args->argv[this->command_id][index];
}

std::string command::params::join(const int index)
{
	std::string result;

	for (auto i = index; i < this->length(); i++)
	{
		if (i > index) result.append(" ");
		result.append(game::native::cmd_args->argv[this->command_id][i]);
	}

	return result;
}

size_t command::params::length()
{
	return game::native::cmd_args->argc[this->command_id];
}

void command::add(const char* name, utils::string::Slot<command::callback> callback)
{
	command::function_map[utils::string::to_lower(name)] = callback;
	game::native::Cmd_AddCommand(name, command::main_callback, command::allocate(), 0);
}

void command::add(const char* name, const char* args, const char* description, utils::string::Slot<command::callback> callback)
{
	command::function_map[utils::string::to_lower(name)] = callback;
	game::native::Cmd_AddCommand(name, args, description, command::main_callback, command::allocate(), 0);
}

void command::execute(std::string command, bool sync)
{
	command.append("\n"); // Make sure it's terminated

	if (sync)
	{
		game::native::Cmd_ExecuteSingleCommand((game::native::LocalClientNum_t)0, 0, command.data());
	}
	else
	{
		game::native::Cbuf_AddText(command.data(), 0);
	}
}

game::native::cmd_function_s* command::allocate()
{
	const auto cmd = new game::native::cmd_function_s;
	command::functions.push_back(cmd);

	return cmd;
}

void command::main_callback()
{
	command::params params(game::native::cmd_args->nesting);
	const std::string command = utils::string::to_lower(params[0]);

	if (command::function_map.contains(command))
	{
		command::function_map[command](params);
	}
}

void command::pre_destroy()
{
	for (const auto command : command::functions)
	{
		delete command;
	}

	command::functions.clear();
}