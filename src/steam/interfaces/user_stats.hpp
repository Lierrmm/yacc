#pragma once

namespace steam
{
	class user_stats
	{
	protected:
		~user_stats() = default;

	public:
		virtual bool RequestCurrentStats();
		virtual bool GetStat(const char* pchName, int* pData);
		virtual bool GetStat(const char* pchName, float* pData);
		virtual bool SetStat(const char* pchName, int nData);
		virtual bool SetStat(const char* pchName, float fData);
		virtual bool UpdateAvgRateStat(const char* pchName, float flCountThisSession, double dSessionLength);
		virtual bool GetAchievement(const char* pchName, bool* pbAchieved);
		virtual bool SetAchievement(const char* pchName);
		virtual bool ClearAchievement(const char* pchName);
		virtual bool GetAchievementAndUnlockTime(const char* pchName, bool* pbAchieved, unsigned int* punUnlockTime);
		virtual bool StoreStats();
		virtual int GetAchievementIcon(const char* pchName);
		virtual const char* GetAchievementDisplayAttribute(const char* pchName, const char* pchKey);
		virtual bool IndicateAchievementProgress(const char* pchName, unsigned int nCurProgress,
		                                         unsigned int nMaxProgress);
		virtual unsigned __int64 RequestUserStats(steam_id steamIDUser);
		virtual bool GetUserStat(steam_id steamIDUser, const char* pchName, int* pData);
		virtual bool GetUserStat(steam_id steamIDUser, const char* pchName, float* pData);
		virtual bool GetUserAchievement(steam_id steamIDUser, const char* pchName, bool* pbAchieved);
		virtual bool GetUserAchievementAndUnlockTime(steam_id steamIDUser, const char* pchName, bool* pbAchieved,
		                                             unsigned int* punUnlockTime);
		virtual bool ResetAllStats(bool bAchievementsToo);
		virtual unsigned __int64 FindOrCreateLeaderboard(const char* pchLeaderboardName, int eLeaderboardSortMethod,
		                                                 int eLeaderboardDisplayType);
		virtual unsigned __int64 FindLeaderboard(const char* pchLeaderboardName);
		virtual const char* GetLeaderboardName(unsigned __int64 hSteamLeaderboard);
		virtual int GetLeaderboardEntryCount(unsigned __int64 hSteamLeaderboard);
		virtual int GetLeaderboardSortMethod(unsigned __int64 hSteamLeaderboard);
		virtual int GetLeaderboardDisplayType(unsigned __int64 hSteamLeaderboard);
		virtual unsigned __int64 DownloadLeaderboardEntries(unsigned __int64 hSteamLeaderboard,
		                                                    int eLeaderboardDataRequest, int nRangeStart,
		                                                    int nRangeEnd);
		virtual unsigned __int64 DownloadLeaderboardEntriesForUsers(unsigned __int64 hSteamLeaderboard,
		                                                            steam_id* prgUsers, int cUsers);
		virtual bool GetDownloadedLeaderboardEntry(unsigned __int64 hSteamLeaderboardEntries, int index,
		                                           int* pLeaderboardEntry, int* pDetails, int cDetailsMax);
		virtual unsigned __int64 UploadLeaderboardScore(unsigned __int64 hSteamLeaderboard,
		                                                int eLeaderboardUploadScoreMethod, int nScore,
		                                                const int* pScoreDetails, int cScoreDetailsCount);
		virtual unsigned __int64 AttachLeaderboardUGC(unsigned __int64 hSteamLeaderboard, unsigned __int64 hUGC);
		virtual unsigned __int64 GetNumberOfCurrentPlayers();
		virtual unsigned __int64 RequestGlobalAchievementPercentages();
		virtual int GetMostAchievedAchievementInfo(char* pchName, unsigned int unNameBufLen, float* pflPercent,
		                                           bool* pbAchieved);
		virtual int GetNextMostAchievedAchievementInfo(int iIteratorPrevious, char* pchName, unsigned int unNameBufLen,
		                                               float* pflPercent, bool* pbAchieved);
		virtual bool GetAchievementAchievedPercent(const char* pchName, float* pflPercent);
		virtual unsigned __int64 RequestGlobalStats(int nHistoryDays);
		virtual bool GetGlobalStat(const char* pchStatName, __int64* pData);
		virtual bool GetGlobalStat(const char* pchStatName, double* pData);
		virtual int GetGlobalStatHistory(const char* pchStatName, __int64* pData, unsigned int cubData);
		virtual int GetGlobalStatHistory(const char* pchStatName, double* pData, unsigned int cubData);
	};
}
