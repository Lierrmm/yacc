#include <std_include.hpp>
#include "steam/steam.hpp"

namespace steam
{
	void game_server::LogOn()
	{
	}

	void game_server::LogOff()
	{
	}

	bool game_server::LoggedOn()
	{
		return true;
	}

	bool game_server::Secure()
	{
		return false;
	}

	steam_id game_server::GetSteamID()
	{
		return steam_id();
	}

	bool game_server::SendUserConnectAndAuthenticate(unsigned int unIPClient, const void* pvAuthBlob,
	                                                 unsigned int cubAuthBlobSize, steam_id* pSteamIDUser)
	{
		return true;
	}

	steam_id game_server::CreateUnauthenticatedUserConnection()
	{
		return steam_id();
	}

	void game_server::SendUserDisconnect(steam_id steamIDUser)
	{
	}

	bool game_server::UpdateUserData(steam_id steamIDUser, const char* pchPlayerName, unsigned int uScore)
	{
		return true;
	}

	bool game_server::SetServerType(unsigned int unServerFlags, unsigned int unGameIP, unsigned short unGamePort,
	                                unsigned short unSpectatorPort, unsigned short usQueryPort, const char* pchGameDir,
	                                const char* pchVersion, bool bLANMode)
	{
		return true;
	}

	void game_server::UpdateServerStatus(int cPlayers, int cPlayersMax, int cBotPlayers, const char* pchServerName,
	                                     const char* pSpectatorServerName, const char* pchMapName)
	{
	}

	void game_server::UpdateSpectatorPort(unsigned short unSpectatorPort)
	{
	}

	void game_server::SetGameType(const char* pchGameType)
	{
	}

	bool game_server::GetUserAchievementStatus(steam_id steamID, const char* pchAchievementName)
	{
		return false;
	}

	void game_server::GetGameplayStats()
	{
	}

	unsigned __int64 game_server::GetServerReputation()
	{
		return 0;
	}

	bool game_server::RequestUserGroupStatus(steam_id steamIDUser, steam_id steamIDGroup)
	{
		return false;
	}

	unsigned int game_server::GetPublicIP()
	{
		return 0;
	}

	void game_server::SetGameData(const char* pchGameData)
	{
	}

	int game_server::UserHasLicenseForApp(steam_id steamID, unsigned int appID)
	{
		return 0;
	}
}
