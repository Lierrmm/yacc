#pragma once

#include <utils/hook.hpp>
#include <utils/string.hpp>

//namespace network
//{
//	using callback = std::function<void(const game::native::native::netadr_t&, const std::string&)>;
//	bool are_addresses_equal(const game::native::native::netadr_t& a, const game::native::native::netadr_t& b);
//	const char* net_adr_to_string(const game::native::native::netadr_t& a);
//}
//
//inline bool operator==(const game::native::native::netadr_t& a, const game::native::native::netadr_t& b)
//{
//	return network::are_addresses_equal(a, b); //
//}
//
//inline bool operator!=(const game::native::native::netadr_t& a, const game::native::native::netadr_t& b)
//{
//	return !(a == b); //
//}
//
//namespace std
//{
//	template <>
//	struct equal_to<game::native::native::netadr_t>
//	{
//		using result_type = bool;
//
//		bool operator()(const game::native::native::netadr_t& lhs, const game::native::native::netadr_t& rhs) const
//		{
//			return network::are_addresses_equal(lhs, rhs);
//		}
//	};
//
//	template <>
//	struct hash<game::native::native::netadr_t>
//	{
//		size_t operator()(const game::native::native::netadr_t& x) const noexcept
//		{
//			return hash<uint32_t>()(*reinterpret_cast<const uint32_t*>(&x.ip[0])) ^ hash<uint16_t>()(x.port);
//		}
//	};
//}
#define NETWORK_MAX_PACKETS_PER_SECOND 100'000

class network final : public module
{
public:
	class Address
	{
	public:
		Address() { setType(game::native::netadrtype_t::NA_BAD); };
		Address(const std::string& addrString);
		Address(sockaddr* addr);
		Address(sockaddr addr) : Address(&addr) {}
		Address(sockaddr_in addr) : Address(&addr) {}
		Address(sockaddr_in* addr) : Address(reinterpret_cast<sockaddr*>(addr)) {}
		Address(game::native::netadr_t addr) : address(addr) {}
		Address(game::native::netadr_t* addr) : Address(*addr) {}
		Address(const Address& obj) : address(obj.address) {};
		bool operator!=(const Address& obj) const { return !(*this == obj); };
		bool operator==(const Address& obj) const;

		void setPort(unsigned short port);
		unsigned short getPort();

		void setIP(DWORD ip);
		void setIP(game::native::netIP_t ip);
		game::native::netIP_t getIP();

		void setType(game::native::netadrtype_t type);
		game::native::netadrtype_t getType();

		sockaddr getSockAddr();
		void toSockAddr(sockaddr* addr);
		void toSockAddr(sockaddr_in* addr);
		game::native::netadr_t* get();
		const char* getCString() const;
		std::string getString() const;

		bool isLocal();
		bool isSelf();
		bool isValid();
		bool isLoopback();

	private:
		game::native::netadr_t address;
	};

	typedef void(Callback)(Address address, const std::string& data);
	typedef void(CallbackRaw)();

	void post_load() override;

	static unsigned short GetPort();

	static void Handle(const std::string& packet, utils::string::Slot<network::Callback> callback);
	static void OnStart(utils::string::Slot<CallbackRaw> callback);

	// Send quake-styled binary data
	static void Send(Address target, const std::string& data);
	static void Send(game::native::netsrc_t type, Address target, const std::string& data);

	// Allows sending raw data without quake header
	static void SendRaw(Address target, const std::string& data);
	static void SendRaw(game::native::netsrc_t type, Address target, const std::string& data);

	// Send quake-style command using binary data
	static void SendCommand(Address target, const std::string& command, const std::string& data = "");
	static void SendCommand(game::native::netsrc_t type, Address target, const std::string& command, const std::string& data = "");

	static void Broadcast(unsigned short port, const std::string& data);
	static void BroadcastRange(unsigned int min, unsigned int max, const std::string& data);
	static void BroadcastAll(const std::string& data);

private:
	static std::string SelectedPacket;
	static utils::string::Signal<CallbackRaw> StartupSignal;
	static std::map<std::string, utils::string::Slot<network::Callback>> PacketHandlers;

	static int PacketInterceptionHandler(const char* packet);
	static void DeployPacket(game::native::netadr_t* from, game::native::msg_t* msg);
	static void DeployPacketStub();

	static void NetworkStart();
	static void NetworkStartStub();

	static void PacketErrorCheck();
};

template <>
struct std::hash<network::Address>
{
	std::size_t operator()(const network::Address& k) const
	{
		return (std::hash<std::string>()(k.getString()));
	}
};
