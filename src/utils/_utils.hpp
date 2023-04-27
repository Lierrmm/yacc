#pragma once

namespace utils::_utils
{
	void OpenUrl(const std::string& url);
	std::string ParseChallenge(const std::string& data);
	float float_to_range(float original_start, float original_end, float new_start, float new_end, float value);
	int int_to_range(int original_start, int original_end, int new_start, int new_end, int value);
	float fmaxf3(float input, float arg1, float arg2);
}