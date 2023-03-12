#include <std_include.hpp>
#include "steam/steam.hpp"

namespace steam
{
	bool apps::BIsSubscribed()
	{
		return true;
	}

	bool apps::BIsLowViolence()
	{
		return false;
	}

	bool apps::BIsCybercafe()
	{
		return false;
	}

	bool apps::BIsVACBanned()
	{
		return false;
	}

	const char* apps::GetCurrentGameLanguage()
	{
		return "english";
	}

	const char* apps::GetAvailableGameLanguages()
	{
		return "english";
	}

	bool apps::BIsSubscribedApp(unsigned int appID)
	{
		return true;
	}

	bool apps::BIsDlcInstalled(unsigned int appID)
	{
		return true;
	}

	unsigned int apps::GetEarliestPurchaseUnixTime(unsigned int nAppID)
	{
		return 0;
	}

	bool apps::BIsSubscribedFromFreeWeekend()
	{
		return false;
	}

	int apps::GetDLCCount()
	{
		return 0;
	}

	bool apps::BGetDLCDataByIndex(int iDLC, unsigned int* pAppID, bool* pbAvailable, char* pchName,
	                              int cchNameBufferSize)
	{
		return false;
	}

	void apps::InstallDLC(unsigned int nAppID)
	{
	}

	void apps::UninstallDLC(unsigned int nAppID)
	{
	}
}
