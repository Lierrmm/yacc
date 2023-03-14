#pragma once

namespace network
{
	using callback = std::function<void(const game::native::netadr_t&, const std::string&)>;
	bool are_addresses_equal(const game::native::netadr_t& a, const game::native::netadr_t& b);
	const char* net_adr_to_string(const game::native::netadr_t& a);
}

inline bool operator==(const game::native::netadr_t& a, const game::native::netadr_t& b)
{
	return network::are_addresses_equal(a, b); //
}

inline bool operator!=(const game::native::netadr_t& a, const game::native::netadr_t& b)
{
	return !(a == b); //
}

namespace std
{
	template <>
	struct equal_to<game::native::netadr_t>
	{
		using result_type = bool;

		bool operator()(const game::native::netadr_t& lhs, const game::native::netadr_t& rhs) const
		{
			return network::are_addresses_equal(lhs, rhs);
		}
	};

	template <>
	struct hash<game::native::netadr_t>
	{
		size_t operator()(const game::native::netadr_t& x) const noexcept
		{
			return hash<uint32_t>()(*reinterpret_cast<const uint32_t*>(&x.ip[0])) ^ hash<uint16_t>()(x.port);
		}
	};
}
