#include "std_include.hpp"

namespace utils::_utils
{
	void SafeShellExecute(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd)
	{
		[=]()
		{
			__try
			{
				ShellExecuteA(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
			}
			__finally
			{
			}
		}();

		std::this_thread::yield();
	}

	void OpenUrl(const std::string& url)
	{
		SafeShellExecute(nullptr, "open", url.data(), nullptr, nullptr, SW_SHOWNORMAL);
	}

	std::string ParseChallenge(const std::string& data)
	{
		auto pos = data.find_first_of("\n ");
		if (pos == std::string::npos) return data;
		return data.substr(0, pos).data();
	}

	//------------------------------ original range ------------------- desired range ----- value to convert
	float float_to_range(float original_start, float original_end, float new_start, float new_end, float value)
	{
		if (value == original_start)
		{
			return new_start;
		}

		if (value == original_end)
		{
			return new_end;
		}

		const float org_diff = original_end - original_start;
		const float new_diff = new_end - new_start;

		return (new_diff / org_diff) * value + new_start;
	}

	//------------ original range -------------------- desired range ----------- value to convert
	int int_to_range(int original_start, int original_end, int new_start, int new_end, int value)
	{
		const int org_diff = original_end - original_start;
		const int new_diff = new_end - new_start;

		return ((new_diff / org_diff) * value + new_start);
	}

	float fmaxf3(float input, float arg1, float arg2)
	{
		float x, output = arg1;

		if (input - arg2 < 0.0f)
		{
			x = input;
		}
		else
		{
			x = arg2;
		}

		if (0.0f > arg1 - input)
		{
			output = x;
		}

		return output;
	}
}