#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include "game/game.hpp"
#include <module/addons/game_console.hpp>

#include "scheduler.hpp"

std::mutex scheduler::mutex_;
std::queue<std::pair<std::string, int>> scheduler::errors_;
utils::concurrent_list<std::pair<std::function<void()>, scheduler::thread>> scheduler::callbacks_;
utils::concurrent_list<std::pair<std::function<void()>, scheduler::thread>> scheduler::single_callbacks_;
utils::concurrent_list<std::pair<std::function<scheduler::evaluation()>, scheduler::thread>> scheduler::condition_callbacks_;

volatile bool kill = false;
std::thread _thread;

void scheduler::on_frame(const std::function<void()> &callback, const thread thread)
{
	callbacks_.add({ callback, thread });
}

void scheduler::delay(const std::function<void()>& callback, const std::chrono::milliseconds delay, const thread thread)
{
	const auto begin = std::chrono::high_resolution_clock::now();

	until([=]()
		{
			if ((std::chrono::high_resolution_clock::now() - begin) > delay)
			{
				callback();
				return evaluation::remove;
			}

			return evaluation::reschedule; },
		thread);
}

void scheduler::once(const std::function<void()>& callback, const thread thread)
{
	single_callbacks_.add({ callback, thread });
}

void scheduler::until(const std::function<evaluation()>& callback, thread thread)
{
	condition_callbacks_.add({ callback, thread });
}

void scheduler::error(const std::string& message, int level)
{
	std::lock_guard _(mutex_);
	errors_.emplace(message, level);
}

__declspec(naked) void scheduler::main_frame_stub()
{
	static const int execution_thread = thread::main;
	const static uint32_t retn_addr = 0x4FA8F0;
	__asm
	{
		pushad;
		push	execution_thread;
		call	execute;
		pop		eax;
		popad;

		jmp		retn_addr;
	}
}

__declspec(naked) void scheduler::renderer_frame_stub_stock()
{
	static const int execution_thread = thread::renderer;
	const static uint32_t retn_addr = 0x4597C0;
	__asm
	{
		pushad;
		push	execution_thread;
		call	execute;
		pop		eax;
		popad;

		jmp		retn_addr;
	}
}

__declspec(naked) void scheduler::renderer_frame_stub_con_addon()
{
	static const int execution_thread = thread::renderer;

	__asm
	{
		pushad;
		push	execution_thread;
		call	execute;
		pop		eax;
		popad;

		call	new_console::check_resize;
		retn;
	}
}

__declspec(naked) void scheduler::execute(thread)
{
	__asm
	{
		push[esp + 4h];

		call	execute_error;
		call	execute_safe;

		add		esp, 4h;
		retn;
	}
}

void scheduler::execute_safe(const thread thread)
{
	for (const auto callback : callbacks_)
	{
		if (callback->second == thread)
		{
			callback->first();
		}
	}

	for (auto callback : single_callbacks_)
	{
		if (callback->second == thread)
		{
			single_callbacks_.remove(callback);
			callback->first();
		}
	}

	for (auto callback : condition_callbacks_)
	{
		if (callback->second == thread)
		{
			if (callback->first() == evaluation::remove)
			{
				condition_callbacks_.remove(callback);
			}
		}
	}
}

void scheduler::execute_error(thread thread)
{
	const char* message;
	int level;

	if (get_next_error(&message, &level) && message)
	{
		game::native::Com_Error(level, "%s", message);
	}
}

bool scheduler::get_next_error(const char** error_message, int* error_level)
{
	std::lock_guard _(mutex_);
	if (errors_.empty())
	{
		*error_message = nullptr;
		return false;
	}

	const auto error = errors_.front();
	errors_.pop();

	*error_level = error.second;
	*error_message = utils::string::va("%s", error.first.data());

	return true;
}

void scheduler::post_start()
{
	_thread = utils::thread::create_named_thread("Async Scheduler", []()
	{
		while (!kill)
		{
			execute(thread::async);
			std::this_thread::sleep_for(10ms);
		}
	});
}

void scheduler::post_load()
{
	utils::hook(0x4FACE8, main_frame_stub, HOOK_CALL).install()->quick();

	if (new_console::is_using_custom_console())
	{
		utils::hook(0x4705EE, renderer_frame_stub_con_addon, HOOK_CALL).install()->quick(); // call yacc_con_CheckResize if console addon is loaded
	}
	else
	{
		utils::hook(0x4705EE, renderer_frame_stub_stock, HOOK_CALL).install()->quick(); // call stock Con_CheckResize otherwise
	}
}

void scheduler::pre_destroy()
{
	kill = true;
	if (_thread.joinable())
	{
		_thread.join();
	}
}

REGISTER_MODULE(scheduler);