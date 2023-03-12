#pragma once

class scheduler final : public module
{
public:
	enum pipeline
	{
		// Asynchronuous pipeline, disconnected from the game
		async = 0,

		// The game's rendering pipeline
		renderer,

		// The game's server thread
		server,

		// The game's main thread
		main,

		count,
	};

	void post_start() override;
	void post_unpack() override;
	void pre_destroy() override;

	static void schedule(const std::function<bool()>& callback, pipeline type = pipeline::async,
		std::chrono::milliseconds delay = 0ms);
	static void loop(const std::function<void()>& callback, pipeline type = pipeline::async,
		std::chrono::milliseconds delay = 0ms);
	static void once(const std::function<void()>& callback, pipeline type = pipeline::async,
		std::chrono::milliseconds delay = 0ms);

private:
	static void execute(const pipeline type);

	static void r_end_frame_stub();
	static void g_glass_update_stub();
	static void main_frame_stub();
};
