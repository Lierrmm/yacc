#pragma once

class news final : public module
{
public:
	void post_load() override;
	void pre_destroy() override;

private:
	static std::thread Thread;

	static bool Terminate;
	static const char* GetNewsText();
};