#include <std_include.hpp>
#include "steam/steam.hpp"

namespace steam
{
	void master_server_updater::SetActive(bool bActive)
	{
	}

	void master_server_updater::SetHeartbeatInterval(int iHeartbeatInterval)
	{
	}

	bool master_server_updater::HandleIncomingPacket(const void* pData, int cbData, unsigned int srcIP,
	                                                 unsigned short srcPort)
	{
		return true;
	}

	int master_server_updater::GetNextOutgoingPacket(void* pOut, int cbMaxOut, unsigned int* pNetAdr,
	                                                 unsigned short* pPort)
	{
		return 0;
	}

	void master_server_updater::SetBasicServerData(unsigned short nProtocolVersion, bool bDedicatedServer,
	                                               const char* pRegionName, const char* pProductName,
	                                               unsigned short nMaxReportedClients, bool bPasswordProtected,
	                                               const char* pGameDescription)
	{
	}

	void master_server_updater::ClearAllKeyValues()
	{
	}

	void master_server_updater::SetKeyValue(const char* pKey, const char* pValue)
	{
	}

	void master_server_updater::NotifyShutdown()
	{
	}

	bool master_server_updater::WasRestartRequested()
	{
		return false;
	}

	void master_server_updater::ForceHeartbeat()
	{
	}

	bool master_server_updater::AddMasterServer(const char* pServerAddress)
	{
		return true;
	}

	bool master_server_updater::RemoveMasterServer(const char* pServerAddress)
	{
		return true;
	}

	int master_server_updater::GetNumMasterServers()
	{
		return 0;
	}

	int master_server_updater::GetMasterServerAddress(int iServer, char* pOut, int outBufferSize)
	{
		return 0;
	}
}
