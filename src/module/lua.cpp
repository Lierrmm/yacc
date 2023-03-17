#include <std_include.hpp>
#include "loader/module_loader.hpp"
#include "game/game.hpp"
#include "command.hpp"
#include "lua.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include <utils/io.hpp>
#include "game_module.hpp"
#include "console.hpp"

struct globals_t
{
	std::string in_require_script;
	std::unordered_map<std::string, std::string> loaded_scripts;
	bool load_raw_script{};
	std::string raw_script_name{};
};

globals_t globals;

lua_State* lua_state;
sol::state global_lua;

void lua_engine::post_load()
{
	lua_state = global_lua.lua_state();
	global_lua.open_libraries(sol::lib::base);
	global_lua.new_usertype<Engine>("_engine", sol::constructors<void()>(),
		"Conbuf_AppendText", &Engine::Conbuf_AppendText,
		"mapname", &Engine::mapname,
		"gametype", &Engine::gametype,
		"GetDvarFloat", &Engine::GetDvarFloat
		);
	global_lua.script("Engine = _engine.new()");
}


void print_error(const std::string& error)
{
	console::error("************** LUI script execution error **************\n");
	console::error("%s\n", error.data());
	console::error("********************************************************\n");
}

void print_loading_script(const std::string& name)
{
	console::info("Loading LUI script '%s'\n", name.data());
}

void lua_engine::load_script(const std::string& name)
{
	globals.loaded_scripts[name] = name;

	try
	{
		const auto load_results = global_lua.script_file(name, &sol::script_pass_on_error);

		if (!load_results.valid())
		{
			sol::error error = load_results;
			print_error(utils::string::va("Failed to load %s", name.data()));
			print_error(error.what());
		}

	}
	catch (std::exception ex)
	{
		print_error(utils::string::va("SOL2 Error: %s", ex.what()));
	}
}

void lua_engine::load_scripts(const std::string& script_dir)
{
	if (!utils::io::directory_exists(script_dir))
	{
		return;
	}

	const auto scripts = utils::io::list_files(script_dir);

	for (const auto& script : scripts)
	{
		std::string data;
		if (std::filesystem::is_directory(script))
		{
			print_loading_script(script);
			load_script(script + "/__init__.lua");
		}
	}
}

void lua_engine::loadLuaScripts()
{
	load_scripts(game_module::get_host_module().get_folder() + "/data/ui_scripts/");
	load_scripts("yacc/ui_scripts/");
	load_scripts("data/ui_scripts/");
}

void lua_engine::post_unpack()
{
	loadLuaScripts();
	command::add("reloadlua", [this](command::params)
	{
		loadLuaScripts();
	});
}


REGISTER_MODULE(lua_engine);