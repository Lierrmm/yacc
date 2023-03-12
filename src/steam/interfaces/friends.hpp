#pragma once

namespace steam
{
	class friends
	{
	protected:
		~friends() = default;

	public:
		virtual const char* GetPersonaName();
		virtual void SetPersonaName(const char* pchPersonaName);
		virtual int GetPersonaState();
		virtual int GetFriendCount(int eFriendFlags);
		virtual steam_id GetFriendByIndex(int iFriend, int iFriendFlags);
		virtual int GetFriendRelationship(steam_id steamIDFriend);
		virtual int GetFriendPersonaState(steam_id steamIDFriend);
		virtual const char* GetFriendPersonaName(steam_id steamIDFriend);
		virtual bool GetFriendGamePlayed(steam_id steamIDFriend, void* pFriendGameInfo);
		virtual const char* GetFriendPersonaNameHistory(steam_id steamIDFriend, int iPersonaName);
		virtual bool HasFriend(steam_id steamIDFriend, int eFriendFlags);
		virtual int GetClanCount();
		virtual steam_id GetClanByIndex(int iClan);
		virtual const char* GetClanName(steam_id steamIDClan);
		virtual const char* GetClanTag(steam_id steamIDClan);
		virtual int GetFriendCountFromSource(steam_id steamIDSource);
		virtual steam_id GetFriendFromSourceByIndex(steam_id steamIDSource, int iFriend);
		virtual bool IsUserInSource(steam_id steamIDUser, steam_id steamIDSource);
		virtual void SetInGameVoiceSpeaking(steam_id steamIDUser, bool bSpeaking);
		virtual void ActivateGameOverlay(const char* pchDialog);
		virtual void ActivateGameOverlayToUser(const char* pchDialog, steam_id steamID);
		virtual void ActivateGameOverlayToWebPage(const char* pchURL);
		virtual void ActivateGameOverlayToStore(unsigned int nAppID);
		virtual void SetPlayedWith(steam_id steamIDUserPlayedWith);
		virtual void ActivateGameOverlayInviteDialog(steam_id steamIDLobby);
		virtual int GetSmallFriendAvatar(steam_id steamIDFriend);
		virtual int GetMediumFriendAvatar(steam_id steamIDFriend);
		virtual int GetLargeFriendAvatar(steam_id steamIDFriend);
		virtual bool RequestUserInformation(steam_id steamIDUser, bool bRequireNameOnly);
		virtual unsigned __int64 RequestClanOfficerList(steam_id steamIDClan);
		virtual steam_id GetClanOwner(steam_id steamIDClan);
		virtual int GetClanOfficerCount(steam_id steamIDClan);
		virtual steam_id GetClanOfficerByIndex(steam_id steamIDClan, int iOfficer);
		virtual int GetUserRestrictions();
		virtual bool SetRichPresence(const char* pchKey, const char* pchValue);
		virtual void ClearRichPresence();
		virtual const char* GetFriendRichPresence(steam_id steamIDFriend, const char* pchKey);
		virtual int GetFriendRichPresenceKeyCount(steam_id steamIDFriend);
		virtual const char* GetFriendRichPresenceKeyByIndex(steam_id steamIDFriend, int iKey);
		virtual bool InviteUserToGame(steam_id steamIDFriend, const char* pchConnectString);
		virtual int GetCoplayFriendCount();
		virtual steam_id GetCoplayFriend(int iCoplayFriend);
		virtual int GetFriendCoplayTime(steam_id steamIDFriend);
		virtual unsigned int GetFriendCoplayGame(steam_id steamIDFriend);
	};
}
