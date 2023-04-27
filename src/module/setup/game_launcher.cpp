#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include <utils/nt.hpp>
#include <utils/string.hpp>

class game_launcher final : public module
{
public:
	void* load_import(const std::string& module, const std::string& function) override
	{
		if (utils::string::to_lower(module) == "shell32.dll" && function == "ShellExecuteA")
		{
			return shell_execute_a;
		}

		return nullptr;
	}

private:
	static HINSTANCE WINAPI shell_execute_a(const HWND hwnd, const LPCSTR lp_operation, const LPCSTR lp_file,
	                                           const LPCSTR lp_parameters, const LPCSTR lp_directory,
	                                           const INT n_show_cmd)
	{
		static const auto mp_url = "steam://run/7940"s;

		if (lp_file && (mp_url == lp_file))
		{
			launch_game();
			return HINSTANCE(33);
		}

		return ShellExecuteA(hwnd, lp_operation, lp_file, lp_parameters, lp_directory, n_show_cmd);
	}


	static void launch_game()
	{
		const utils::nt::library self;

		STARTUPINFOA s_info;
		PROCESS_INFORMATION p_info;

		ZeroMemory(&s_info, sizeof(s_info));
		ZeroMemory(&p_info, sizeof(p_info));
		s_info.cb = sizeof(s_info);

		const auto path = self.get_path();
		std::string cmdline = utils::string::va("\"%s\"", path.data());

		CreateProcessA(path.data(), cmdline.data(), nullptr, nullptr, false, NULL, nullptr, nullptr, &s_info, &p_info);

		if (p_info.hThread && p_info.hThread != INVALID_HANDLE_VALUE) CloseHandle(p_info.hThread);
		if (p_info.hProcess && p_info.hProcess != INVALID_HANDLE_VALUE) CloseHandle(p_info.hProcess);
	}
};

REGISTER_MODULE(game_launcher)
