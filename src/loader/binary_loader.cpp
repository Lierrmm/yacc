#include <std_include.hpp>
#include <utils/nt.hpp>
#include <utils/io.hpp>
#include <utils/cryptography.hpp>
#include <utils/string.hpp>
#include <utils/compression.hpp>

#include "binary_loader.hpp"

namespace binary_loader
{
	std::string load_resource(const int id)
	{
		const auto res = FindResource(::utils::nt::library(), MAKEINTRESOURCE(id), RT_RCDATA);
		if (!res) return {};

		const auto handle = LoadResource(nullptr, res);
		if (!handle) return {};

		return std::string(LPSTR(LockResource(handle)), SizeofResource(nullptr, res));
	}

	std::string load_delta(const launcher::mode mode)
	{
		if (mode == launcher::mode::singleplayer)
		{
			return load_resource(BINARY_SP);
		}

		if (mode == launcher::mode::multiplayer)
		{
			return load_resource(BINARY_MP);
		}

		return {};
	}

	std::string load_base(launcher::mode mode)
	{
		std::string data;
		std::string executable;

		switch (mode)
		{
		case launcher::mode::multiplayer:
			executable = "iw3mp.exe"s;
			break;
		case launcher::mode::singleplayer:
			executable = "iw3sp.exe"s;
			break;
		default:
			throw std::runtime_error("Invalid mode specified.");
			break;
		}

		if (!utils::io::read_file(executable, &data))
		{
			throw std::runtime_error("Unable to load iw5mp_server.exe");
		}

		return data;
	}

	std::string compress_with_match_score(const std::string& data, const std::string& base, int match_score)
	{
		const auto new_data = reinterpret_cast<const unsigned char*>(data.data());
		const auto old_data = reinterpret_cast<const unsigned char*>(base.data());

		std::vector<unsigned char> diff;
		create_diff(new_data, new_data + data.size(), old_data, old_data + base.size(), diff, match_score);

		const unsigned long long size = data.size();

		std::string result(reinterpret_cast<char*>(diff.data()), diff.size());
		result.append(reinterpret_cast<const char*>(&size), sizeof(size));
		result = utils::compression::zstd::compress(result);

		return result;
	}

	void create_for_file(const std::string& file, const std::string& base)
	{
		std::string data;
		std::string result;
		std::vector<std::string> results;

		if (!utils::io::read_file(file, &data))
		{
			throw std::runtime_error(utils::string::va("Unable to load file %s!", file.data()));
		}

		{
			std::mutex mutex;
			std::vector<std::thread> threads;

			for (auto i = 0; i <= 9; ++i)
			{
				threads.emplace_back([&mutex, &results, &data, &base, i]()
				{
					const auto result = compress_with_match_score(data, base, i);

					std::lock_guard _(mutex);
					results.push_back(result);
				});
			}

			for (auto& t : threads)
			{
				if (t.joinable())
				{
					t.join();
				}
			}
		}

		for (const auto& current_result : results)
		{
			if (result.empty() || current_result.size() < result.size())
			{
				result = current_result;
			}

			printf("%s\n", result.data());
		}

		utils::io::write_file(file + ".diff", result);
	}

	void create()
	{
		create_for_file("iw3sp.exe", load_base(launcher::mode::singleplayer));
		create_for_file("iw3mp.exe", load_base(launcher::mode::multiplayer));
	}

	std::string build_binary(const std::string& base, const std::string& diff)
	{
		const auto* size = reinterpret_cast<const unsigned long long*>(diff.data() + diff.size() - sizeof(unsigned long
			long));

		std::string binary;
		binary.resize(static_cast<size_t>(*size));

		const auto new_data = reinterpret_cast<unsigned char*>(binary.data());
		const auto old_data = reinterpret_cast<const unsigned char*>(base.data());
		const auto diff_data = reinterpret_cast<const unsigned char*>(diff.data());

		if (patch(new_data, new_data + binary.size(), old_data, old_data + base.size(), diff_data,
		          diff_data + diff.size() - sizeof(*size)) == hpatch_FALSE || binary.empty())
		{
			throw std::runtime_error("Unable to create binary from patch!");
		}

		return binary;
	}

	std::string load(const launcher::mode mode)
	{
		const auto base = load_base(mode);
		auto delta = load_delta(mode);
		delta = utils::compression::zstd::decompress(delta);
		return build_binary(base, delta);
	}
}
