#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include "game/game.hpp"

#include <utils/hook.hpp>
#include <utils/thread.hpp>
#include <utils/concurrency.hpp>

#include "scheduler.hpp"

namespace
{
	constexpr bool cond_continue = false;
	constexpr bool cond_end = true;

	struct task
	{
		std::function<bool()> handler{};
		std::chrono::milliseconds interval{};
		std::chrono::high_resolution_clock::time_point last_call{};
	};

	using task_list = std::vector<task>;

	class task_pipeline
	{
	public:
		void add(task&& task)
		{
			new_callbacks_.access([&task](task_list& tasks)
			{
				tasks.emplace_back(std::move(task));
			});
		}

		void execute()
		{
			callbacks_.access([&](task_list& tasks)
			{
				this->merge_callbacks();

				for (auto i = tasks.begin(); i != tasks.end();)
				{
					const auto now = std::chrono::high_resolution_clock::now();
					const auto diff = now - i->last_call;

					if (diff < i->interval)
					{
						++i;
						continue;
					}

					i->last_call = now;

					const auto res = i->handler();
					if (res == cond_end)
					{
						i = tasks.erase(i);
					}
					else
					{
						++i;
					}
				}
			});
		}

	private:
		utils::concurrency::container<task_list> new_callbacks_;
		utils::concurrency::container<task_list, std::recursive_mutex> callbacks_;

		void merge_callbacks()
		{
			callbacks_.access([&](task_list& tasks)
			{
				new_callbacks_.access([&](task_list& new_tasks)
				{
					tasks.insert(tasks.end(), std::move_iterator<task_list::iterator>(new_tasks.begin()), std::move_iterator<task_list::iterator>(new_tasks.end()));
					new_tasks = {};
				});
			});
		}
	};

	volatile bool kill = false;
	std::thread thread;
	task_pipeline pipelines[scheduler::pipeline::count];
}

void scheduler::execute(const pipeline type)
{
	assert(type >= 0 && type < pipeline::count);
	pipelines[type].execute();
}

void scheduler::r_end_frame_stub()
{
	utils::hook::invoke<void>(SELECT_VALUE(0x4193D0, 0x67F840));
	execute(pipeline::renderer);
}

void scheduler::g_glass_update_stub()
{
	utils::hook::invoke<void>(SELECT_VALUE(0x4E3730, 0x505BB0));
	execute(pipeline::server);
}

void scheduler::main_frame_stub()
{
	utils::hook::invoke<void>(SELECT_VALUE(0x458600, 0x556470));
	execute(pipeline::main);
}

void scheduler::schedule(const std::function<bool()>& callback, const pipeline type,
	const std::chrono::milliseconds delay)
{
	assert(type >= 0 && type < pipeline::count);

	task task;
	task.handler = callback;
	task.interval = delay;
	task.last_call = std::chrono::high_resolution_clock::now();

	pipelines[type].add(std::move(task));
}

void scheduler::loop(const std::function<void()>& callback, const pipeline type,
	const std::chrono::milliseconds delay)
{
	schedule([callback]()
	{
		callback();
		return cond_continue;
	}, type, delay);
}

void scheduler::once(const std::function<void()>& callback, const pipeline type,
	const std::chrono::milliseconds delay)
{
	schedule([callback]
	{
		callback();
		return cond_end;
	}, type, delay);
}

void scheduler::post_start()
{
	thread = utils::thread::create_named_thread("Async Scheduler", []()
	{
		while (!kill)
		{
			execute(pipeline::async);
			std::this_thread::sleep_for(10ms);
		}
	});
}

void scheduler::post_unpack()
{
	//utils::hook(SELECT_VALUE(0x44C7DB, 0x55688E), main_frame_stub, HOOK_CALL).install()->quick();

	//utils::hook(SELECT_VALUE(0x57F7F8, 0x4978E2), r_end_frame_stub, HOOK_CALL).install()->quick();

	//// Hook a function inside G_RunFrame. Fixes TLS issues
	//utils::hook(SELECT_VALUE(0x52EFBC, 0x50CEC6), g_glass_update_stub, HOOK_CALL).install()->quick();
}

void scheduler::pre_destroy()
{
	kill = true;
	if (thread.joinable())
	{
		thread.join();
	}
}

//REGISTER_MODULE(scheduler);
