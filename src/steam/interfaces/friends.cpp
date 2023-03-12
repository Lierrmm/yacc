#include <std_include.hpp>
#include "steam/steam.hpp"

namespace steam
{
	const char* friends::GetPersonaName()
	{
		return "GlaDos";
	}

	void friends::SetPersonaName(const char* pchPersonaName)
	{
	}

	int friends::GetPersonaState()
	{
		return 1;
	}

	int friends::GetFriendCount(int eFriendFlags)
	{
		return 0;
	}

	steam_id friends::GetFriendByIndex(int iFriend, int iFriendFlags)
	{
		return steam_id();
	}

	int friends::GetFriendRelationship(steam_id steamIDFriend)
	{
		return 0;
	}

	int friends::GetFriendPersonaState(steam_id steamIDFriend)
	{
		return 0;
	}

	const char* friends::GetFriendPersonaName(steam_id steamIDFriend)
	{
		return "";
	}

	bool friends::GetFriendGamePlayed(steam_id steamIDFriend, void* pFriendGameInfo)
	{
		return false;
	}

	const char* friends::GetFriendPersonaNameHistory(steam_id steamIDFriend, int iPersonaName)
	{
		return "";
	}

	bool friends::HasFriend(steam_id steamIDFriend, int eFriendFlags)
	{
		return false;
	}

	int friends::GetClanCount()
	{
		return 0;
	}

	steam_id friends::GetClanByIndex(int iClan)
	{
		return steam_id();
	}

	const char* friends::GetClanName(steam_id steamIDClan)
	{
		return "3arc";
	}

	const char* friends::GetClanTag(steam_id steamIDClan)
	{
		return this->GetClanName(steamIDClan);
	}

	int friends::GetFriendCountFromSource(steam_id steamIDSource)
	{
		return 0;
	}

	steam_id friends::GetFriendFromSourceByIndex(steam_id steamIDSource, int iFriend)
	{
		return steam_id();
	}

	bool friends::IsUserInSource(steam_id steamIDUser, steam_id steamIDSource)
	{
		return false;
	}

	void friends::SetInGameVoiceSpeaking(steam_id steamIDUser, bool bSpeaking)
	{
	}

	void friends::ActivateGameOverlay(const char* pchDialog)
	{
	}

	void friends::ActivateGameOverlayToUser(const char* pchDialog, steam_id steamID)
	{
	}

	void friends::ActivateGameOverlayToWebPage(const char* pchURL)
	{
	}

	void friends::ActivateGameOverlayToStore(unsigned int nAppID)
	{
		OutputDebugStringA("Store requested!");
	}

	void friends::SetPlayedWith(steam_id steamIDUserPlayedWith)
	{
	}

	void friends::ActivateGameOverlayInviteDialog(steam_id steamIDLobby)
	{
	}

	int friends::GetSmallFriendAvatar(steam_id steamIDFriend)
	{
		return 0;
	}

	int friends::GetMediumFriendAvatar(steam_id steamIDFriend)
	{
		return 0;
	}

	int friends::GetLargeFriendAvatar(steam_id steamIDFriend)
	{
		return 0;
	}

	bool friends::RequestUserInformation(steam_id steamIDUser, bool bRequireNameOnly)
	{
		return false;
	}

	unsigned __int64 friends::RequestClanOfficerList(steam_id steamIDClan)
	{
		return 0;
	}

	steam_id friends::GetClanOwner(steam_id steamIDClan)
	{
		return steam_id();
	}

	int friends::GetClanOfficerCount(steam_id steamIDClan)
	{
		return 0;
	}

	steam_id friends::GetClanOfficerByIndex(steam_id steamIDClan, int iOfficer)
	{
		return steam_id();
	}

	int friends::GetUserRestrictions()
	{
		return 0;
	}

	bool friends::SetRichPresence(const char* pchKey, const char* pchValue)
	{
		return true;
	}

	void friends::ClearRichPresence()
	{
	}

	const char* friends::GetFriendRichPresence(steam_id steamIDFriend, const char* pchKey)
	{
		return "";
	}

	int friends::GetFriendRichPresenceKeyCount(steam_id steamIDFriend)
	{
		return 0;
	}

	const char* friends::GetFriendRichPresenceKeyByIndex(steam_id steamIDFriend, int iKey)
	{
		return "a";
	}

	bool friends::InviteUserToGame(steam_id steamIDFriend, const char* pchConnectString)
	{
		return false;
	}

	int friends::GetCoplayFriendCount()
	{
		return 0;
	}

	steam_id friends::GetCoplayFriend(int iCoplayFriend)
	{
		return steam_id();
	}

	int friends::GetFriendCoplayTime(steam_id steamIDFriend)
	{
		return 0;
	}

	unsigned int friends::GetFriendCoplayGame(steam_id steamIDFriend)
	{
		return 0;
	}
}
