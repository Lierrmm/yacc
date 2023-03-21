#pragma once

enum class module_priority
{
	min = 0,
	// must run after the updater
	steam_proxy,
	updater,
	patches,
	// must have the highest priority
	filesystem,
};

class module
{
public:
	virtual ~module()
	{
	}

	virtual void post_start()
	{
	}

	virtual void post_load()
	{
	}

	virtual void post_unpack()
	{
	}

	virtual void pre_destroy()
	{
	}

	virtual module_priority priority() const
	{
		return module_priority::min;
	}

	virtual void* load_import(const std::string& module, const std::string& function)
	{
		return nullptr;
	}
};
