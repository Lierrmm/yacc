#pragma once

namespace steam
{
	class game_server
	{
	protected:
		~game_server() = default;

	public:
		virtual void LogOn();
		virtual void LogOff();
		virtual bool LoggedOn();
		virtual bool Secure();
		virtual steam_id GetSteamID();
		virtual bool SendUserConnectAndAuthenticate(unsigned int unIPClient, const void* pvAuthBlob,
		                                            unsigned int cubAuthBlobSize, steam_id* pSteamIDUser);
		virtual steam_id CreateUnauthenticatedUserConnection();
		virtual void SendUserDisconnect(steam_id steamIDUser);
		virtual bool UpdateUserData(steam_id steamIDUser, const char* pchPlayerName, unsigned int uScore);
		virtual bool SetServerType(unsigned int unServerFlags, unsigned int unGameIP, unsigned short unGamePort,
		                           unsigned short unSpectatorPort, unsigned short usQueryPort, const char* pchGameDir,
		                           const char* pchVersion, bool bLANMode);
		virtual void UpdateServerStatus(int cPlayers, int cPlayersMax, int cBotPlayers, const char* pchServerName,
		                                const char* pSpectatorServerName, const char* pchMapName);
		virtual void UpdateSpectatorPort(unsigned short unSpectatorPort);
		virtual void SetGameType(const char* pchGameType);
		virtual bool GetUserAchievementStatus(steam_id steamID, const char* pchAchievementName);
		virtual void GetGameplayStats();
		virtual unsigned __int64 GetServerReputation();
		virtual bool RequestUserGroupStatus(steam_id steamIDUser, steam_id steamIDGroup);
		virtual unsigned int GetPublicIP();
		virtual void SetGameData(const char* pchGameData);
		virtual int UserHasLicenseForApp(steam_id steamID, unsigned int appID);
	};
}
