#pragma once
#include <utils/string.hpp>

class command final : public module
{
public:
	class params
	{
	public:
		params(DWORD id) : command_id(id) {};
		params(const params& obj) { this->command_id = obj.command_id; };
		params() : params(game::native::cmd_args->nesting) {};
		virtual std::string join(int index);

		const char* operator[](size_t index);
		size_t length();

	private:
		DWORD command_id;
	};

	void pre_destroy() override;
	typedef void(callback)(command::params params);

	static void add(const char* name, utils::string::Slot<callback> callback);
	static void add(const char* name, const char* args, const char* description, utils::string::Slot<command::callback> callback);
	static void execute(std::string command, bool sync = true);

private:
	static game::native::cmd_function_s* allocate();
	static std::vector<game::native::cmd_function_s*> functions;
	static std::map<std::string, utils::string::Slot<callback>> function_map;
	static void main_callback();
};