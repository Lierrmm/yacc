#pragma once

class new_console final : public module
{
public:
	void post_load() override;
	static bool is_using_custom_console();
	static void check_resize();
	static int cmd_argc();

private:
	static bool using_custom_console;
};