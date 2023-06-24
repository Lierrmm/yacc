#include <std_include.hpp>

#include "memory.hpp"

#include <game/game.hpp>

#include "Stream.hpp"
#include "string.hpp"

namespace utils
{
	std::string Stream::Reader::readString()
	{
		std::string str;

		while (char byte = this->readByte())
		{
			str.push_back(byte);
		}

		return str;
	}

	const char* Stream::Reader::readCString()
	{
		return this->allocator->duplicate_string(this->readString());
	}

	char Stream::Reader::readByte()
	{
		if ((this->position + 1) <= this->buffer.size())
		{
			return this->buffer[this->position++];
		}

		throw std::runtime_error("Reading past the buffer");
	}

	void* Stream::Reader::read(size_t size, size_t count)
	{
		size_t bytes = size * count;

		if ((this->position + bytes) <= this->buffer.size())
		{
			void* _buffer = this->allocator->allocate(bytes);

			std::memcpy(_buffer, this->buffer.data() + this->position, bytes);
			this->position += bytes;

			return _buffer;
		}

		throw std::runtime_error("Reading past the buffer");
	}

	bool Stream::Reader::end()
	{
		return (this->buffer.size() == this->position);
	}

	void Stream::Reader::seek(unsigned int _position)
	{
		if (this->buffer.size() >= _position)
		{
			this->position = _position;
		}
	}

	void Stream::Reader::seekRelative(unsigned int _position)
	{
		return this->seek(_position + this->position);
	}

	void* Stream::Reader::readPointer()
	{
		void* pointer = this->read<void*>();
		if (!this->hasPointer(pointer))
		{
			this->pointerMap[pointer] = nullptr;
		}
		return pointer;
	}

	void Stream::Reader::mapPointer(void* oldPointer, void* newPointer)
	{
		if (this->hasPointer(oldPointer))
		{
			this->pointerMap[oldPointer] = newPointer;
		}
	}

	bool Stream::Reader::hasPointer(void* pointer)
	{
		return this->pointerMap.find(pointer) != this->pointerMap.end();
	}

	Stream::Stream() : ptrAssertion(false), criticalSectionState(0)
	{
		memset(this->blockSize, 0, sizeof(this->blockSize));

#ifdef WRITE_LOGS
		this->structLevel = 0;
		Utils::IO::WriteFile("userraw/logs/zb_writes.log", "", false);
#endif
	}

	Stream::Stream(size_t size) : Stream()
	{
		this->buffer.reserve(size);
	}

	Stream::~Stream()
	{
		this->buffer.clear();

		if (this->criticalSectionState != 0)
		{
			MessageBoxA(nullptr, utils::string::va("Invalid critical section state '%i' for stream destruction!", this->criticalSectionState), "WARNING", MB_ICONEXCLAMATION);
		}
	};

	size_t Stream::length()
	{
		return this->buffer.length();
	}

	size_t Stream::capacity()
	{
		return this->buffer.capacity();
	}

	void Stream::assertPointer(const void* pointer, size_t length)
	{
		if (!this->ptrAssertion) return;

		for (auto& entry : this->ptrList)
		{
			unsigned int ePtr = reinterpret_cast<unsigned int>(entry.first);
			unsigned int tPtr = reinterpret_cast<unsigned int>(pointer);

			if (utils::HasIntercection(ePtr, entry.second, tPtr, length))
			{
				MessageBoxA(nullptr, "Duplicate data written!", "ERROR", MB_ICONERROR);
				__debugbreak();
			}
		}

		this->ptrList.push_back({ pointer, length });
	}

	char* Stream::save(const void* _str, size_t size, size_t count)
	{
		return this->save(this->getCurrentBlock(), _str, size, count);
	}

	char* Stream::save(game::native::XFILE_BLOCK_TYPES stream, const void * _str, size_t size, size_t count)
	{
		// Only those seem to actually write data.
		// everything else is allocated at runtime but XFILE_BLOCK_RUNTIME is the only one that actually allocates anything
		// clearly half of this stuff is unused
		if (stream == game::native::XFILE_BLOCK_RUNTIME)
		{
			this->increaseBlockSize(stream, size * count);
			return this->at();
		}

		auto data = this->data();

		if (this->isCriticalSection() && this->length() + (size * count) > this->capacity())
		{
			MessageBoxA(nullptr, utils::string::va("Potential stream reallocation during critical operation detected! Writing data of the length 0x%X exceeds the allocated stream size of 0x%X\n", (size * count), this->capacity()), "ERROR", MB_ICONERROR);
			__debugbreak();
		}

		this->buffer.append(static_cast<const char*>(_str), size * count);

		if (this->data() != data && this->isCriticalSection())
		{
			MessageBoxA(nullptr, "Stream reallocation during critical operations not permitted!\nPlease increase the initial memory size or reallocate memory during non-critical sections!", "ERROR", MB_ICONERROR);
			__debugbreak();
		}

		this->increaseBlockSize(stream, size * count);
		this->assertPointer(_str, size * count);

		return this->at() - (size * count);
	}

	char* Stream::save(game::native::XFILE_BLOCK_TYPES stream, int value, size_t count)
	{
		auto ret = this->length();

		for (size_t i = 0; i < count; ++i)
		{
			this->save(stream, &value, 4, 1);
		}

		return this->data() + ret;
	}

	char* Stream::saveString(const std::string& string)
	{
		return this->saveString(string.data()/*, string.size()*/);
	}

	char* Stream::saveString(const char* string)
	{
		return this->saveString(string, strlen(string));
	}

	char* Stream::saveString(const char* string, size_t len)
	{
		auto ret = this->length();

		if (string)
		{
			this->save(string, len);
		}

		this->saveNull();

		return this->data() + ret;
	}

	char* Stream::saveText(const std::string& string)
	{
		return this->save(string.data(), string.length());
	}

	char* Stream::saveByte(unsigned char byte, size_t count)
	{
		auto ret = this->length();

		for (size_t i = 0; i < count; ++i)
		{
			this->save(&byte, 1);
		}

		return this->data() + ret;
	}

	char* Stream::saveNull(size_t count)
	{
		return this->saveByte(0, count);
	}

	char* Stream::saveMax(size_t count)
	{
		return this->saveByte(static_cast<unsigned char>(-1), count);
	}

	void Stream::align(Stream::Alignment align)
	{
		uint32_t size = 2 << align;

		// Not power of 2!
		if (!size || (size & (size - 1))) return;
		--size;

		game::native::XFILE_BLOCK_TYPES stream = this->getCurrentBlock();
		this->blockSize[stream] = ~size & (this->getBlockSize(stream) + size);
	}

	bool Stream::pushBlock(game::native::XFILE_BLOCK_TYPES stream)
	{
		this->streamStack.push_back(stream);
		return this->isValidBlock(stream);
	}

	bool Stream::popBlock()
	{
		if (!this->streamStack.empty())
		{
			this->streamStack.pop_back();
			return true;
		}

		return false;
	}

	bool Stream::hasBlock()
	{
		return !this->streamStack.empty();
	}

	bool Stream::isValidBlock(game::native::XFILE_BLOCK_TYPES stream)
	{
		return (stream < game::native::MAX_XFILE_COUNT && stream >= game::native::XFILE_BLOCK_TEMP);
	}

	void Stream::increaseBlockSize(game::native::XFILE_BLOCK_TYPES stream, unsigned int size)
	{
		if (this->isValidBlock(stream))
		{
			this->blockSize[stream] += size;
		}

#ifdef WRITE_LOGS
		std::string data = Utils::String::VA("%*s%d\n", this->structLevel, "", size);
		if (stream == Game::XFILE_BLOCK_RUNTIME) data = Utils::String::VA("%*s(%d)\n", this->structLevel, "", size);
		Utils::IO::WriteFile("userraw/logs/zb_writes.log", data, true);
#endif
	}

	void Stream::increaseBlockSize(unsigned int size)
	{
		return this->increaseBlockSize(this->getCurrentBlock(), size);
	}

	game::native::XFILE_BLOCK_TYPES Stream::getCurrentBlock()
	{
		if (!this->streamStack.empty())
		{
			return this->streamStack.back();
		}

		return game::native::XFILE_BLOCK_INVALID;
	}

	char* Stream::at()
	{
		return reinterpret_cast<char*>(this->data() + this->length());
	}

	char* Stream::data()
	{
		return const_cast<char*>(this->buffer.data());
	}

	unsigned int Stream::getBlockSize(game::native::XFILE_BLOCK_TYPES stream)
	{
		if (this->isValidBlock(stream))
		{
			return this->blockSize[stream];
		}

		return 0;
	}

	DWORD Stream::getPackedOffset()
	{
		game::native::XFILE_BLOCK_TYPES block = this->getCurrentBlock();

		Stream::Offset offset;
		offset.block = block;
		offset.offset = this->getBlockSize(block);
		return offset.getPackedOffset();
	}

	void Stream::toBuffer(std::string& outBuffer)
	{
		outBuffer.clear();
		outBuffer.append(this->data(), this->length());
	}

	std::string Stream::toBuffer()
	{
		std::string _buffer;
		this->toBuffer(_buffer);
		return _buffer;
	}

	void Stream::enterCriticalSection()
	{
		++this->criticalSectionState;
	}

	void Stream::leaveCriticalSection()
	{
		--this->criticalSectionState;
	}

	bool Stream::isCriticalSection()
	{
		if (this->criticalSectionState < 0)
		{
			MessageBoxA(nullptr, "CriticalSectionState in stream has been overrun!", "ERROR", MB_ICONERROR);
			__debugbreak();
		}

		return (this->criticalSectionState != 0);
	}
}
