#pragma once
#include "nt.hpp"

#define HOOK_JUMP true
#define HOOK_CALL false

namespace utils
{
	class hook final
	{
	public:
		class signature final
		{
		public:
			struct container final
			{
				const char* signature;
				const char* mask;
				std::function<void(char*)> callback;
			};

			signature(void* start, const size_t length) : start_(start), length_(length)
			{
			}

			signature(const DWORD start, const size_t length) : signature(reinterpret_cast<void*>(start), length)
			{
			}

			signature() : signature(0x400000, 0x800000)
			{
			}

			void process();
			void add(const container& container);

		private:
			void* start_;
			size_t length_;
			std::vector<container> signatures_;
		};

		class detour
		{
		public:
			detour() = default;
			detour(void* place, void* target);
			detour(size_t place, void* target);
			~detour();

			detour(detour&& other) noexcept
			{
				this->operator=(std::move(other));
			}

			detour& operator=(detour&& other) noexcept
			{
				if (this != &other)
				{
					this->~detour();

					this->place_ = other.place_;
					this->original_ = other.original_;

					other.place_ = nullptr;
					other.original_ = nullptr;
				}

				return *this;
			}

			detour(const detour&) = delete;
			detour& operator=(const detour&) = delete;

			void enable() const;
			void disable() const;

			void create(void* place, void* target);
			void create(size_t place, void* target);
			void clear();

			template <typename T>
			T* get() const
			{
				return static_cast<T*>(this->get_original());
			}

			template <typename T = void, typename... Args>
			T invoke(Args ... args)
			{
				return static_cast<T(*)(Args ...)>(this->get_original())(args...);
			}

			[[nodiscard]] void* get_original() const;

		private:
			void* place_{};
			void* original_{};
		};

		hook() : initialized_(false), installed_(false), place_(nullptr), stub_(nullptr), original_(nullptr),
		         use_jump_(false), protection_(0)
		{
			ZeroMemory(this->buffer_, sizeof(this->buffer_));
		}

		hook(void* place, void* stub, const bool use_jump = true) : hook() { this->initialize(place, stub, use_jump); }

		hook(void* place, void (*stub)(), const bool use_jump = true) : hook(
			place, reinterpret_cast<void*>(stub), use_jump)
		{
		}

		hook(const DWORD place, void* stub, const bool use_jump = true) : hook(
			reinterpret_cast<void*>(place), stub, use_jump)
		{
		}

		hook(const DWORD place, const DWORD stub, const bool use_jump = true) : hook(
			reinterpret_cast<void*>(place), reinterpret_cast<void*>(stub), use_jump)
		{
		}

		hook(const DWORD place, void (*stub)(), const bool use_jump = true) : hook(
			reinterpret_cast<void*>(place), reinterpret_cast<void*>(stub), use_jump)
		{
		}

		hook(const hook&) = delete;
		hook(const hook&&) = delete;

		~hook();

		hook* initialize(void* place, void* stub, bool use_jump = true);
		hook* initialize(DWORD place, void* stub, bool use_jump = true);
		hook* initialize(DWORD place, void (*stub)(), bool use_jump = true); // For lambdas
		hook* install(bool unprotect = true, bool keep_unprotected = false);
		hook* uninstall(bool unprotect = true);

		void* get_address() const;
		void* get_original() const;
		void quick();

		static bool iat(nt::library module, const std::string& target_module, const std::string& process, void* stub);

		static void nop(void* place, size_t length);
		static void nop(DWORD place, size_t length);

		template <typename T> static std::function<T> Call(DWORD function)
		{
			return std::function<T>(reinterpret_cast<T*>(function));
		}

		template <typename T> static std::function<T> Call(FARPROC function)
		{
			return Call<T>(reinterpret_cast<DWORD>(function));
		}

		template <typename T> static std::function<T> Call(void* function)
		{
			return Call<T>(reinterpret_cast<DWORD>(function));
		}

		template <typename T>
		static void set(void* place, T value)
		{
			DWORD old_protect;
			VirtualProtect(place, sizeof(T), PAGE_EXECUTE_READWRITE, &old_protect);

			*static_cast<T*>(place) = value;

			VirtualProtect(place, sizeof(T), old_protect, &old_protect);
			FlushInstructionCache(GetCurrentProcess(), place, sizeof(T));
		}

		template <typename T>
		static void set(const DWORD place, T value)
		{
			return set<T>(reinterpret_cast<void*>(place), value);
		}

		static bool is_relatively_far(const void* pointer, const void* data, const int offset)
		{
			const int64_t diff = size_t(data) - (size_t(pointer) + offset);
			const auto small_diff = int32_t(diff);
			return diff != int64_t(small_diff);
		}

		static void inject(void* pointer, const void* data)
		{
			if (is_relatively_far(pointer, data, 4))
			{
				throw std::runtime_error("Too far away to create 32bit relative branch");
			}

			set<int32_t>(pointer, int32_t(size_t(data) - (size_t(pointer) + 4)));
		}

		static void inject(const size_t pointer, const void* data)
		{
			return inject(reinterpret_cast<void*>(pointer), data);
		}

		template <typename T, typename... Args>
		static T invoke(size_t func, Args ... args)
		{
			return reinterpret_cast<T(*)(Args ...)>(func)(args...);
		}

		template <typename T, typename... Args>
		static T invoke(void* func, Args ... args)
		{
			return static_cast<T(*)(Args ...)>(func)(args...);
		}

	private:
		bool initialized_;
		bool installed_;

		void* place_;
		void* stub_;
		void* original_;
		char buffer_[5]{};
		bool use_jump_;

		DWORD protection_;

		std::mutex state_mutex_;
	};
}
