#include <std_include.hpp>
#include "loader/module_loader.hpp"
#include "game/game.hpp"

#include <module/setup/console.hpp>
#include "network.hpp"
std::string network::SelectedPacket;
utils::string::Signal<network::CallbackRaw> network::StartupSignal;
std::map<std::string, utils::string::Slot<network::Callback>> network::PacketHandlers;

network::Address::Address(const std::string& addrString)
{
	game::native::NET_StringToAdr(addrString.data(), &this->address);
}

network::Address::Address(sockaddr* addr)
{
	game::native::network::SockadrToNetadr(addr, &this->address);
}

bool network::Address::operator==(const network::Address& obj) const
{
	return game::native::network::NET_CompareAdr(this->address, obj.address);
}

void network::Address::setPort(unsigned short port)
{
	this->address.port = htons(port);
}

unsigned short network::Address::getPort()
{
	return ntohs(this->address.port);
}

void network::Address::setIP(DWORD ip)
{
	this->address.ip.full = ip;
}

void network::Address::setIP(game::native::netIP_t ip)
{
	this->address.ip = ip;
}

game::native::netIP_t network::Address::getIP()
{
	return this->address.ip;
}

void network::Address::setType(game::native::netadrtype_t type)
{
	this->address.type = type;
}

game::native::netadrtype_t network::Address::getType()
{
	return this->address.type;
}

sockaddr network::Address::getSockAddr()
{
	sockaddr addr;
	this->toSockAddr(&addr);
	return addr;
}

void network::Address::toSockAddr(sockaddr* addr)
{
	if (addr)
	{
		game::native::network::NetadrToSockadr(&this->address, addr);
	}
}

void network::Address::toSockAddr(sockaddr_in* addr)
{
	this->toSockAddr(reinterpret_cast<sockaddr*>(addr));
}

game::native::netadr_t* network::Address::get()
{
	return &this->address;
}

const char* network::Address::getCString() const
{
	return game::native::network::NET_AdrToString(this->address);
}

std::string network::Address::getString() const
{
	return this->getCString();
}

bool network::Address::isLocal()
{
	// According to: https://en.wikipedia.org/wiki/Private_network

	// 10.X.X.X
	if (this->getIP().bytes[0] == 10) return true;

	// 192.168.X.X
	if (this->getIP().bytes[0] == 192 && this->getIP().bytes[1] == 168) return true;

	// 172.16.X.X - 172.31.X.X
	if (this->getIP().bytes[0] == 172 && (this->getIP().bytes[1] >= 16) && (this->getIP().bytes[1] < 32)) return true;

	// 127.0.0.1
	if (this->getIP().full == 0x0100007F) return true;

	// TODO: Maybe check for matching localIPs and subnet mask

	return false;
}

bool network::Address::isSelf()
{
	if (game::native::network::NET_IsLocalAddress(this->address)) return true; // Loopback
	if (this->getPort() != network::GetPort()) return false; // Port not equal

	for (int i = 0; i < *game::native::network::numIP; ++i)
	{
		if (this->getIP().full == game::native::network::localIP[i].full)
		{
			return true;
		}
	}

	return false;
}

bool network::Address::isLoopback()
{
	if (this->getIP().full == 0x100007f) // 127.0.0.1
	{
		return true;
	}

	return game::native::network::NET_IsLocalAddress(this->address);
}

bool network::Address::isValid()
{
	return (this->getType() != game::native::netadrtype_t::NA_BAD && this->getType() >= game::native::netadrtype_t::NA_BOT && this->getType() <= game::native::netadrtype_t::NA_IP);
}

void network::Handle(const std::string& packet, utils::string::Slot<network::Callback> callback)
{
	network::PacketHandlers[utils::string::to_lower(packet)] = callback;
}

void network::OnStart(utils::string::Slot<network::CallbackRaw> callback)
{
	network::StartupSignal.connect(callback);
}

void network::Send(game::native::netsrc_t type, network::Address target, const std::string& data)
{
	std::string rawData;
	rawData.append("\xFF\xFF\xFF\xFF", 4);
	rawData.append(data);
	//rawData.append("\0", 1);

	network::SendRaw(type, target, rawData);
}

void network::Send(network::Address target, const std::string& data)
{
	network::Send(game::native::netsrc_t::NS_CLIENT1, target, data);
}

void network::SendRaw(game::native::netsrc_t type, network::Address target, const std::string& data)
{
	if (!target.isValid()) return;
	game::native::network::Sys_SendPacket(type, data.size(), data.data(), *target.get());
}

void network::SendRaw(network::Address target, const std::string& data)
{
	network::SendRaw(game::native::netsrc_t::NS_CLIENT1, target, data);
}

void network::SendCommand(game::native::netsrc_t type, network::Address target, const std::string& command, const std::string& data)
{
	// Use space as separator (possible separators are '\n', ' ').
	// Though, our handler only needs exactly 1 char as separator and doesn't care which char it is.
	// EDIT: Most 3rd party tools expect a line break, so let's use that instead!
	std::string packet;
	packet.append(command);
	packet.append("\n", 1);
	packet.append(data);

	network::Send(type, target, packet);
}

void network::SendCommand(network::Address target, const std::string& command, const std::string& data)
{
	network::SendCommand(game::native::netsrc_t::NS_CLIENT1, target, command, data);
}

void network::Broadcast(unsigned short port, const std::string& data)
{
	Address target;

	target.setPort(port);
	target.setIP(INADDR_BROADCAST);
	target.setType(game::native::netadrtype_t::NA_BROADCAST);

	network::Send(game::native::netsrc_t::NS_CLIENT1, target, data);
}

void network::BroadcastRange(unsigned int min, unsigned int max, const std::string& data)
{
	for (unsigned int i = min; i < max; ++i)
	{
		network::Broadcast(static_cast<unsigned short>(i & 0xFFFF), data);
	}
}

void network::BroadcastAll(const std::string& data)
{
	network::BroadcastRange(100, 65536, data);
}

int network::PacketInterceptionHandler(const char* packet)
{
	// Packet rate limit.
	static uint32_t packets = 0;
	static int lastClean = 0;

	if ((game::native::Sys_Milliseconds() - lastClean) > 1'000)
	{
		packets = 0;
		lastClean = game::native::Sys_Milliseconds();
	}

	if ((++packets) > NETWORK_MAX_PACKETS_PER_SECOND)
	{
		return 1;
	}

	std::string packetCommand = packet;
	auto pos = packetCommand.find_first_of("\\\n ");
	if (pos != std::string::npos)
	{
		packetCommand = packetCommand.substr(0, pos);
	}

	packetCommand = utils::string::to_lower(packetCommand);

	// Check if custom handler exists
	for (auto i = network::PacketHandlers.begin(); i != network::PacketHandlers.end(); ++i)
	{
		if (utils::string::to_lower(i->first) == packetCommand)
		{
			network::SelectedPacket = i->first;
			return 0;
		}
	}

	// No interception
	return 1;
}

void network::DeployPacket(game::native::netadr_t* from, game::native::msg_t* msg)
{
	if (network::PacketHandlers.find(network::SelectedPacket) != network::PacketHandlers.end())
	{
		std::string data;

		size_t offset = network::SelectedPacket.size() + 4 + 1;

		if (static_cast<size_t>(msg->cursize) > offset)
		{
			data.append(msg->data + offset, msg->cursize - offset);
		}

		// Remove trailing 0x00 byte
		// Actually, don't remove it, it might be part of the packet. Send correctly formatted packets instead!
		//if (data.size() && !data[data.size() - 1]) data.pop_back();

		network::PacketHandlers[network::SelectedPacket](from, data);
	}
	else
	{
		console::info("Error: Network packet intercepted, but handler is missing!\n");
	}
}

void network::NetworkStart()
{
	network::StartupSignal();
}

unsigned short network::GetPort()
{
	return game::native::Dvar_FindVar("net_port")->current.integer;
}

__declspec(naked) void network::NetworkStartStub()
{
	__asm
	{
		mov eax, 572F50h
		call eax
		pop     edi
		pop     esi
		pop     ebp
		pop     ebx
		mov     esp, ebp
		pop     ebp
		jmp network::NetworkStart
	}
}

__declspec(naked) void network::DeployPacketStub()
{
	__asm
	{
		lea eax, [esp + 054h]

		pushad

		push ebp // Command
		push eax // Address pointer
		call network::DeployPacket
		add esp, 8h

		popad

		mov al, 1
		pop edi
		pop esi
		pop ebp
		pop ebx
		add esp, 040h
		retn
	}
}

__declspec(naked) void network::PacketErrorCheck()
{
	__asm
	{
		cmp eax, 2746h
		jz returnIgnore

		cmp eax, WSAENETRESET
		jz returnIgnore

		push 57244Fh
		retn

		returnIgnore :
		push 57261Eh
			retn
	}
}

void network::post_load()
{
	//AssertSize(game::native::netadr_t, 20);

	// increase max port binding attempts from 10 to 100
	utils::hook::set<BYTE>(0x57311F, 100);

	// increase cl_maxpackets limit
	utils::hook::set<BYTE>(0x469E31, 125);

	// default maxpackets and snaps
	utils::hook::set<BYTE>(0x46A3DC, 30);

	// Parse port as short in Net_AddrToString
	utils::hook::set<const char*>(0x5025A1, "%u.%u.%u.%u:%hu");

	// Install startup handler
	utils::hook(0x5731A9, network::NetworkStartStub, HOOK_JUMP).install()->quick();

	// Install interception handler
	utils::hook(0x468018, network::PacketInterceptionHandler, HOOK_CALL).install()->quick();

	// Prevent recvfrom error spam
	utils::hook(0x572444, network::PacketErrorCheck, HOOK_JUMP).install()->quick();

	// Install packet deploy hook
	utils::hook::RedirectJump(0x468022, network::DeployPacketStub);

	network::Handle("resolveAddress", [](Address address, const std::string& /*data*/)
	{
		network::SendRaw(address, address.getString());
	});
}

REGISTER_MODULE(network);