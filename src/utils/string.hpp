#pragma once
#include "memory.hpp"

namespace utils::string
{
	template <typename T> using Slot = std::function<T>;

	template <size_t Buffers, size_t MinBufferSize>
	class va_provider final
	{
	public:
		static_assert(Buffers != 0 && MinBufferSize != 0, "Buffers and MinBufferSize mustn't be 0");

		va_provider() : current_buffer_(0)
		{
		}

		char* get(const char* format, const va_list ap)
		{
			++this->current_buffer_ %= ARRAYSIZE(this->string_pool_);
			auto entry = &this->string_pool_[this->current_buffer_];

			if (!entry->size || !entry->buffer)
			{
				throw std::runtime_error("String pool not initialized");
			}

			while (true)
			{
				const int res = vsnprintf_s(entry->buffer, entry->size, _TRUNCATE, format, ap);
				if (res > 0) break; // Success
				if (res == 0) return nullptr; // Error

				entry->double_size();
			}

			return entry->buffer;
		}

	private:
		class entry final
		{
		public:
			explicit entry(const size_t _size = MinBufferSize) : size(_size), buffer(nullptr)
			{
				if (this->size < MinBufferSize) this->size = MinBufferSize;
				this->allocate();
			}

			~entry()
			{
				if (this->buffer) memory::get_allocator()->free(this->buffer);
				this->size = 0;
				this->buffer = nullptr;
			}

			void allocate()
			{
				if (this->buffer) memory::get_allocator()->free(this->buffer);
				this->buffer = memory::get_allocator()->allocate_array<char>(this->size + 1);
			}

			void double_size()
			{
				this->size *= 2;
				this->allocate();
			}

			size_t size;
			char* buffer;
		};

		size_t current_buffer_;
		entry string_pool_[Buffers];
	};

	const char* va(const char* fmt, ...);
	std::vector<std::string> split(const std::string& s, const char delim);
	std::string to_lower(std::string text);
	std::string to_upper(std::string text);
	void strip(const char* in, char* out, size_t max);
	bool starts_with(const std::string& text, const std::string& substring);
	bool match_compare(const std::string& input, const std::string& text, bool exact);
	std::string replace(std::string str, const std::string& from, const std::string& to);

	std::wstring convert(const std::string& str);

	std::string dump_hex(const std::string& data, const std::string& separator = " ");

	std::string get_clipboard_data();
}
