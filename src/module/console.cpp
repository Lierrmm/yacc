#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include "game/game.hpp"

#include "console.hpp"
#include "scheduler.hpp"
//#include "log_file.hpp"

#include <utils/concurrency.hpp>
#include <utils/hook.hpp>

namespace
{
	using message_queue = std::queue<std::string>;
	utils::concurrency::container<message_queue> message_queue_;
}

console::console()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	(void)_pipe(this->handles_, 1024, _O_TEXT);
	(void)_dup2(this->handles_[1], 1);
	(void)_dup2(this->handles_[1], 2);
}

void console::post_load()
{
	scheduler::loop(std::bind(&console::log_messages, this), scheduler::pipeline::main);
	this->console_runner_ = std::thread(std::bind(&console::runner, this));
}

void console::post_unpack()
{
	game::native::Sys_ShowConsole();

	message_queue_.access([this]([[maybe_unused]] message_queue& queue)
	{
		this->console_initialized_ = true;
	});
}

void console::pre_destroy()
{
	this->terminate_runner_ = true;

	printf("\r\n");
	_flushall();

	_close(this->handles_[0]);
	_close(this->handles_[1]);

	if (this->console_runner_.joinable())
	{
		this->console_runner_.join();
	}
}

void console::print(const int type, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	const auto result = format(&ap, fmt);
	va_end(ap);

	dispatch_message(type, result);
}

void console::log_messages() const
{
	while (this->console_initialized_ && !message_queue_.get_raw().empty())
	{
		std::queue<std::string> message_queue_copy;

		{
			message_queue_.access([&](message_queue& queue)
			{
				message_queue_copy = std::move(queue);
				queue = std::queue<std::string>();
			});
		}

		while (!message_queue_copy.empty())
		{
			log_message(message_queue_copy.front());
			message_queue_copy.pop();
		}
	}

	fflush(stdout);
	fflush(stderr);
}

void console::log_message(const std::string& message)
{
#ifdef _DEBUG
	OutputDebugStringA(message.data());
#endif
	game::native::Conbuf_AppendText_ASM(message.data());
}

void console::dispatch_message([[maybe_unused]] const int type, const std::string& message)
{
	//log_file::info(message);
	message_queue_.access([&message](message_queue& queue)
	{
		queue.emplace(message);
	});
}

void console::runner() const
{
	while (!this->terminate_runner_ && this->handles_[0])
	{
		char buffer[1024];
		const auto len = _read(this->handles_[0], buffer, sizeof(buffer));
		if (len > 0)
		{
			dispatch_message(con_type_info, std::string(buffer, len));
		}
		else
		{
			std::this_thread::sleep_for(10ms);
		}
	}

	std::this_thread::yield();
}

std::string console::format(va_list* ap, const char* message)
{
	static thread_local char buffer[0x1000];

	const auto count = vsnprintf_s(buffer, _TRUNCATE, message, *ap);

	if (count < 0) return {};
	return {buffer, static_cast<std::size_t>(count)};
}

REGISTER_MODULE(console)
