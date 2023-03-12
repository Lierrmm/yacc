#pragma once

class console final : public module
{
public:
	enum
	{
		con_type_error = 1,
		con_type_warning = 3,
		con_type_info = 7,
	};

	console();

	void post_start() override;
	void pre_destroy() override;
	void post_unpack() override;

	static void print(int type, const char* fmt, ...);

	template <typename... Args>
	static void error(const char* fmt, Args&&... args)
	{
		print(con_type_error, fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void warn(const char* fmt, Args&&... args)
	{
		print(con_type_warning, fmt, std::forward<Args>(args)...);
	}

	template <typename... Args>
	static void info(const char* fmt, Args&&... args)
	{
		print(con_type_info, fmt, std::forward<Args>(args)...);
	}

private:
	bool console_initialized_ = false;
	bool terminate_runner_ = false;

	std::thread console_runner_;

	int handles_[2]{};

	void log_messages() const;
	static void log_message(const std::string& message);
	static void dispatch_message(const int type, const std::string& message);

	void runner() const;

	static std::string format(va_list* ap, const char* message);
};
