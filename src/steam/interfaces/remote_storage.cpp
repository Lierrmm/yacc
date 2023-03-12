#include <std_include.hpp>
#include "steam/steam.hpp"

namespace steam
{
	bool remote_storage::FileWrite(const char* pchFile, const void* pvData, int cubData)
	{
		return true;
	}

	int remote_storage::GetFileSize(const char* pchFile)
	{
		return 0;
	}

	int remote_storage::FileRead(const char* pchFile, void* pvData, int cubDataToRead)
	{
		OutputDebugStringA(pchFile);
		return 0;
	}

	bool remote_storage::FileExists(const char* pchFile)
	{
		return false;
	}

	int remote_storage::GetFileCount()
	{
		return 0;
	}

	const char* remote_storage::GetFileNameAndSize(int iFile, int* pnFileSizeInBytes)
	{
		*pnFileSizeInBytes = 0;
		return "";
	}

	bool remote_storage::GetQuota(int* pnTotalBytes, int* puAvailableBytes)
	{
		*pnTotalBytes = 0x10000000;
		*puAvailableBytes = 0x10000000;
		return false;
	}
}
