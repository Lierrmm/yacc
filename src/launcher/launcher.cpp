#include <std_include.hpp>
#include <utils/nt.hpp>

#include "launcher.hpp"

launcher::launcher()
{
	this->create_main_menu();
}

void launcher::create_main_menu()
{
	this->main_window_.register_callback("openUrl", [](html_frame::callback_params* params)
	{
		if (params->arguments.empty()) return;

		const auto param = params->arguments[0];
		if (!param.is_string()) return;

		const auto url = param.get_string();
		ShellExecuteA(nullptr, "open", url.data(), nullptr, nullptr, SW_SHOWNORMAL);
	});

	this->main_window_.register_callback("selectMode", [this](html_frame::callback_params* params)
	{
		if (params->arguments.empty()) return;

		const auto param = params->arguments[0];
		if (!param.is_number()) return;

		const auto number = param.get_number();
		if (number == singleplayer || number == multiplayer)
		{
			this->select_mode(static_cast<mode>(number));
		}
	});

	this->main_window_.set_callback(
		[](window* window, const UINT message, const WPARAM w_param, const LPARAM l_param) -> LRESULT
		{
			if (message == WM_CLOSE)
			{
				window::close_all();
			}

			return DefWindowProcA(*window, message, w_param, l_param);
		});

	this->main_window_.create("YACC", 750, 430);
	this->main_window_.load_html(load_content(MENU_MAIN));
	this->main_window_.show();
}

launcher::mode launcher::run() const
{
	window::run();
	return this->mode_;
}

void launcher::select_mode(const mode mode)
{
	this->mode_ = mode;
	this->main_window_.close();
}

std::string launcher::load_content(const int res)
{
	const auto resource = FindResource(utils::nt::library(), MAKEINTRESOURCE(res), RT_RCDATA);
	if (!resource) return {};

	const auto handle = LoadResource(nullptr, resource);
	if (!handle) return {};

	return std::string(LPSTR(LockResource(handle)), SizeofResource(nullptr, resource));
}
