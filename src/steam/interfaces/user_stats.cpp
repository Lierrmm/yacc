#include <std_include.hpp>
#include "steam/steam.hpp"

namespace steam
{
	bool user_stats::RequestCurrentStats()
	{
		return true;
	}

	bool user_stats::GetStat(const char* pchName, int* pData)
	{
		return false;
	}

	bool user_stats::GetStat(const char* pchName, float* pData)
	{
		return false;
	}

	bool user_stats::SetStat(const char* pchName, int nData)
	{
		return false;
	}

	bool user_stats::SetStat(const char* pchName, float fData)
	{
		return false;
	}

	bool user_stats::UpdateAvgRateStat(const char* pchName, float flCountThisSession, double dSessionLength)
	{
		return false;
	}

	bool user_stats::GetAchievement(const char* pchName, bool* pbAchieved)
	{
		return false;
	}

	bool user_stats::SetAchievement(const char* pchName)
	{
		return false;
	}

	bool user_stats::ClearAchievement(const char* pchName)
	{
		return false;
	}

	bool user_stats::GetAchievementAndUnlockTime(const char* pchName, bool* pbAchieved, unsigned int* punUnlockTime)
	{
		return false;
	}

	bool user_stats::StoreStats()
	{
		return false;
	}

	int user_stats::GetAchievementIcon(const char* pchName)
	{
		return 0;
	}

	const char* user_stats::GetAchievementDisplayAttribute(const char* pchName, const char* pchKey)
	{
		return "";
	}

	bool user_stats::IndicateAchievementProgress(const char* pchName, unsigned int nCurProgress,
	                                             unsigned int nMaxProgress)
	{
		return false;
	}

	unsigned __int64 user_stats::RequestUserStats(steam_id steamIDUser)
	{
		return 0;
	}

	bool user_stats::GetUserStat(steam_id steamIDUser, const char* pchName, int* pData)
	{
		return false;
	}

	bool user_stats::GetUserStat(steam_id steamIDUser, const char* pchName, float* pData)
	{
		return false;
	}

	bool user_stats::GetUserAchievement(steam_id steamIDUser, const char* pchName, bool* pbAchieved)
	{
		return false;
	}

	bool user_stats::GetUserAchievementAndUnlockTime(steam_id steamIDUser, const char* pchName, bool* pbAchieved,
	                                                 unsigned int* punUnlockTime)
	{
		return false;
	}

	bool user_stats::ResetAllStats(bool bAchievementsToo)
	{
		return false;
	}

	unsigned __int64 user_stats::FindOrCreateLeaderboard(const char* pchLeaderboardName, int eLeaderboardSortMethod,
	                                                     int eLeaderboardDisplayType)
	{
		return 0;
	}

	unsigned __int64 user_stats::FindLeaderboard(const char* pchLeaderboardName)
	{
		return 0;
	}

	const char* user_stats::GetLeaderboardName(unsigned __int64 hSteamLeaderboard)
	{
		return "";
	}

	int user_stats::GetLeaderboardEntryCount(unsigned __int64 hSteamLeaderboard)
	{
		return 0;
	}

	int user_stats::GetLeaderboardSortMethod(unsigned __int64 hSteamLeaderboard)
	{
		return 0;
	}

	int user_stats::GetLeaderboardDisplayType(unsigned __int64 hSteamLeaderboard)
	{
		return 0;
	}

	unsigned __int64 user_stats::DownloadLeaderboardEntries(unsigned __int64 hSteamLeaderboard,
	                                                        int eLeaderboardDataRequest, int nRangeStart, int nRangeEnd)
	{
		return 0;
	}

	unsigned __int64 user_stats::DownloadLeaderboardEntriesForUsers(unsigned __int64 hSteamLeaderboard,
	                                                                steam_id* prgUsers, int cUsers)
	{
		return 0;
	}

	bool user_stats::GetDownloadedLeaderboardEntry(unsigned __int64 hSteamLeaderboardEntries, int index,
	                                               int* pLeaderboardEntry, int* pDetails, int cDetailsMax)
	{
		return false;
	}

	unsigned __int64 user_stats::UploadLeaderboardScore(unsigned __int64 hSteamLeaderboard,
	                                                    int eLeaderboardUploadScoreMethod, int nScore,
	                                                    const int* pScoreDetails, int cScoreDetailsCount)
	{
		return 0;
	}

	unsigned __int64 user_stats::AttachLeaderboardUGC(unsigned __int64 hSteamLeaderboard, unsigned __int64 hUGC)
	{
		return 0;
	}

	unsigned __int64 user_stats::GetNumberOfCurrentPlayers()
	{
		return 0;
	}

	unsigned __int64 user_stats::RequestGlobalAchievementPercentages()
	{
		return 0;
	}

	int user_stats::GetMostAchievedAchievementInfo(char* pchName, unsigned int unNameBufLen, float* pflPercent,
	                                               bool* pbAchieved)
	{
		return 0;
	}

	int user_stats::GetNextMostAchievedAchievementInfo(int iIteratorPrevious, char* pchName, unsigned int unNameBufLen,
	                                                   float* pflPercent, bool* pbAchieved)
	{
		return 0;
	}

	bool user_stats::GetAchievementAchievedPercent(const char* pchName, float* pflPercent)
	{
		return false;
	}

	unsigned __int64 user_stats::RequestGlobalStats(int nHistoryDays)
	{
		return 0;
	}

	bool user_stats::GetGlobalStat(const char* pchStatName, __int64* pData)
	{
		return false;
	}

	bool user_stats::GetGlobalStat(const char* pchStatName, double* pData)
	{
		return false;
	}

	int user_stats::GetGlobalStatHistory(const char* pchStatName, __int64* pData, unsigned int cubData)
	{
		return 0;
	}

	int user_stats::GetGlobalStatHistory(const char* pchStatName, double* pData, unsigned int cubData)
	{
		return 0;
	}
}
