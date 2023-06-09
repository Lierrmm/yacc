#include <std_include.hpp>
#include "string.hpp"
#define VA_BUFFER_COUNT		64
#define VA_BUFFER_SIZE		65536
namespace utils::string
{
	void InfoString::set(const std::string& key, const std::string& value)
	{
		this->keyValuePairs[key] = value;
	}

	std::string InfoString::get(const std::string& key)
	{
		const auto value = this->keyValuePairs.find(key);
		if (value != this->keyValuePairs.end())
		{
			return value->second;
		}

		return "";
	}

	void InfoString::parse(std::string buffer)
	{
		if (buffer[0] == '\\')
		{
			buffer = buffer.substr(1);
		}

		auto KeyValues = split(buffer, '\\');

		for (size_t i = 0; !KeyValues.empty() && i < (KeyValues.size() - 1); i += 2)
		{
			const auto& key = KeyValues[i];
			const auto& value = KeyValues[i + 1];
			this->keyValuePairs[key] = value;
		}
	}

	std::string InfoString::build()
	{
		std::string infoString;

		auto first = true;

		for (const auto& [key, value] : this->keyValuePairs)
		{
			if (first) first = false;
			else infoString.append("\\");

			infoString.append(key);
			infoString.append("\\");
			infoString.append(value);
		}

		return infoString;
	}

	void InfoString::dump()
	{
		for (const auto& [key, value] : this->keyValuePairs)
		{
			OutputDebugStringA(va("%s: %s\n", key.data(), value.data()));
		}
	}

	json11::Json InfoString::to_json()
	{
		return this->keyValuePairs;
	}

	const char* va(const char* fmt, ...)
	{
		static thread_local va_provider<8, 256> provider;

		va_list ap;
		va_start(ap, fmt);

		const char* result;
		result = provider.get(fmt, ap);

		va_end(ap);
		return result;
	}

	std::vector<std::string> split(const std::string& s, const char delim)
	{
		std::stringstream ss(s);
		std::string item;
		std::vector<std::string> elems;

		while (std::getline(ss, item, delim))
		{
			elems.push_back(item); // elems.push_back(std::move(item)); // if C++11 (based on comment from @mchiasson)
		}

		return elems;
	}

	std::string to_lower(std::string text)
	{
		std::transform(text.begin(), text.end(), text.begin(), [](const unsigned char input)
		{
			return static_cast<char>(std::tolower(input));
		});

		return text;
	}

	std::string to_upper(std::string text)
	{
		std::transform(text.begin(), text.end(), text.begin(), [](const unsigned char input)
		{
			return static_cast<char>(std::toupper(input));
		});

		return text;
	}

	bool match_compare(const std::string& input, const std::string& text, const bool exact)
	{
		if (exact && text == input) return true;
		if (!exact && text.find(input) != std::string::npos) return true;
		return false;
	}

	bool starts_with(const std::string& text, const std::string& substring)
	{
		return text.find(substring) == 0;
	}

	bool ends_with(const std::string& haystack, const std::string& needle)
	{
		if (needle.size() > haystack.size()) return false;
		return std::equal(needle.rbegin(), needle.rend(), haystack.rbegin());
	}

	void strip(const char* in, char* out, size_t max)
	{
		if (!in || !out) return;

		max--;
		auto current = 0u;
		while (*in != 0 && current < max)
		{
			const auto color_index = (*(in + 1) - 48) >= 0xC ? 7 : (*(in + 1) - 48);

			if (*in == '^' && (color_index != 7 || *(in + 1) == '7'))
			{
				++in;
			}
			else
			{
				*out = *in;
				++out;
				++current;
			}

			++in;
		}

		*out = '\0';
	}

	int IsSpace(int c)
	{
		if (c < -1) return 0;
		return _isspace_l(c, nullptr);
	}

	std::string& LTrim(std::string& str)
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int val)
			{
				return !IsSpace(val);
			}));

		return str;
	}

	// Trim from end
	std::string& RTrim(std::string& str)
	{
		str.erase(std::find_if(str.rbegin(), str.rend(), [](int val)
			{
				return !IsSpace(val);
			}).base(), str.end());

		return str;
	}

	std::string& trim(std::string& str)
	{
		return LTrim(RTrim(str));
	}

	std::string convert(const std::wstring& wstr)
	{
		std::string result;
		result.reserve(wstr.size());

		for (const auto& chr : wstr)
		{
			result.push_back(static_cast<char>(chr));
		}

		return result;
	}

	std::wstring convert(const std::string& str)
	{
		std::wstring result;
		result.reserve(str.size());

		for (const auto& chr : str)
		{
			result.push_back(static_cast<wchar_t>(chr));
		}

		return result;
	}

	std::string replace(std::string str, const std::string& from, const std::string& to)
	{
		if (from.empty())
		{
			return str;
		}

		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos)
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}

		return str;
	}

	std::string dump_hex(const std::string& data, const std::string& separator)
	{
		std::string result;

		for (unsigned int i = 0; i < data.size(); ++i)
		{
			if (i > 0)
			{
				result.append(separator);
			}

			result.append(va("%02X", data[i] & 0xFF));
		}

		return result;
	}

	std::string get_clipboard_data()
	{
		if (OpenClipboard(nullptr))
		{
			std::string data;

			auto* const clipboard_data = GetClipboardData(1u);
			if (clipboard_data)
			{
				auto* const cliptext = static_cast<char*>(GlobalLock(clipboard_data));
				if (cliptext)
				{
					data.append(cliptext);
					GlobalUnlock(clipboard_data);
				}
			}

			CloseClipboard();

			return data;
		}
		return {};
	}

	std::string XOR(std::string str, char value)
	{
		for (unsigned int i = 0; i < str.size(); ++i)
		{
			str[i] ^= value;
		}

		return str;
	}
}
