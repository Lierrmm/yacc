#pragma once

namespace steam
{
	class apps
	{
	protected:
		~apps() = default;

	public:
		virtual bool BIsSubscribed();
		virtual bool BIsLowViolence();
		virtual bool BIsCybercafe();
		virtual bool BIsVACBanned();
		virtual const char* GetCurrentGameLanguage();
		virtual const char* GetAvailableGameLanguages();
		virtual bool BIsSubscribedApp(unsigned int appID);
		virtual bool BIsDlcInstalled(unsigned int appID);
		virtual unsigned int GetEarliestPurchaseUnixTime(unsigned int nAppID);
		virtual bool BIsSubscribedFromFreeWeekend();
		virtual int GetDLCCount();
		virtual bool BGetDLCDataByIndex(int iDLC, unsigned int* pAppID, bool* pbAvailable, char* pchName,
		                                int cchNameBufferSize);
		virtual void InstallDLC(unsigned int nAppID);
		virtual void UninstallDLC(unsigned int nAppID);
	};
}
