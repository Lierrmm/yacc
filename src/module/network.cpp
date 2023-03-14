#include <std_include.hpp>
#include <game/game.hpp>

#include "network.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace network
{
	namespace
	{
		int net_compare_base_address(const game::native::netadr_t* a1, const game::native::netadr_t* a2)
		{
			if (a1->type == a2->type)
			{
				switch (a1->type)
				{
				case game::native::NA_BOT:
				case game::native::NA_LOOPBACK:
					return a1->port == a2->port;

				case game::native::NA_IP:
					return !memcmp(a1->ip, a2->ip, 4);
				case game::native::NA_BROADCAST:
					return true;
				default:
					break;
				}
			}

			return false;
		}

		int net_compare_address(const game::native::netadr_t* a1, const game::native::netadr_t* a2)
		{
			return net_compare_base_address(a1, a2) && a1->port == a2->port;
		}
	}

	bool are_addresses_equal(const game::native::netadr_t& a, const game::native::netadr_t& b)
	{
		return net_compare_address(&a, &b);
	}

	const char* net_adr_to_string(const game::native::netadr_t& a)
	{
		if (a.type == game::native::NA_LOOPBACK)
		{
			return "loopback";
		}

		if (a.type == game::native::NA_BOT)
		{
			return "bot";
		}

		if (a.type == game::native::NA_IP || a.type == game::native::NA_BROADCAST)
		{
			if (a.port)
			{
				return utils::string::va("%u.%u.%u.%u:%u", a.ip[0], a.ip[1], a.ip[2], a.ip[3], htons(a.port));
			}

			return utils::string::va("%u.%u.%u.%u", a.ip[0], a.ip[1], a.ip[2], a.ip[3]);
		}

		return "bad";
	}

}