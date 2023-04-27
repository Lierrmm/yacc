#include <std_include.hpp>
#include "module_loader.hpp"
#include <utils/memory.hpp>

//std::vector<std::unique_ptr<module>>* module_loader::modules_ = nullptr;

//void module_loader::register_module(std::unique_ptr<module>&& module_)
//{
//	if (!modules_)
//	{
//		modules_ = new std::vector<std::unique_ptr<module>>();
//		atexit(destroy_modules);
//	}
//
//	modules_->push_back(std::move(module_));
//}

bool module_loader::uninitializing = false;

void module_loader::register_module(std::unique_ptr<module>&& component_)
{
	get_components().push_back(std::move(component_));
}

std::vector<std::unique_ptr<module>>& module_loader::get_components()
{
	using component_vector = std::vector<std::unique_ptr<module>>;
	using component_vector_container = std::unique_ptr<component_vector, std::function<void(component_vector*)>>;

	static component_vector_container components(new component_vector, [](component_vector* component_vector)
	{
		pre_destroy();
		delete component_vector;
	});

	return *components;
}

void module_loader::sort()
{
	auto& components = get_components();

	std::ranges::stable_sort(components, [](const std::unique_ptr<module>& a, const std::unique_ptr<module>& b)
	{
		return a->priority() > b->priority();
	});
}

bool module_loader::post_start()
{
	static auto handled = false;
	if (handled) return true;
	handled = true;

	try
	{
		for (const auto& module_ : get_components())
		{
			module_->post_start();
		}
	}
	catch (premature_shutdown_trigger&)
	{
		return false;
	}

	return true;
}

bool module_loader::post_load()
{
	static auto handled = false;
	if (handled) return true;
	handled = true;

	try
	{
		for (const auto& module_ : get_components())
		{
			module_->post_load();
		}
	}
	catch (premature_shutdown_trigger&)
	{
		return false;
	}

	return true;
}

void module_loader::post_unpack()
{
	static auto handled = false;
	if (handled) return;
	handled = true;

	for (const auto& module_ : get_components())
	{
		module_->post_unpack();
	}
}

void module_loader::pre_destroy()
{
	static auto handled = false;
	if (handled) return;
	handled = true;

	for (const auto& module_ : get_components())
	{
		module_->pre_destroy();
	}
}

void* module_loader::load_import(const std::string& module, const std::string& function)
{
	void* function_ptr = nullptr;

	for (const auto& module_ : get_components())
	{
		const auto module_function_ptr = module_->load_import(module, function);
		if (module_function_ptr)
		{
			function_ptr = module_function_ptr;
		}
	}

	return function_ptr;
}

bool module_loader::is_uninitializing()
{
	return module_loader::uninitializing;
}

void module_loader::uninitialize()
{
	uninitializing = true;
	auto& components = get_components();
	std::reverse(components.begin(), components.end());

	for (const auto& module_ : components)
	{
		delete &module_;
	}

	components.clear();
	utils::memory::get_allocator()->clear();
	uninitializing = false;
}

void module_loader::trigger_premature_shutdown()
{
	throw premature_shutdown_trigger();
}

