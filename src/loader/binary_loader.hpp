#pragma once
#include "launcher/launcher.hpp"

namespace binary_loader
{
	void create();
	std::string load(launcher::mode mode);
}
