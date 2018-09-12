#include "headers.h"
#include <time.h>

int64 GetTime()
{
    return time(NULL);
}

static int64 nTimeOffset = 0;


int64 GetAdjustedTime()
{
    return GetTime() + nTimeOffset;
}

// TODO: deep analysis the implementation
string strprintf(const char* format, ...)
{
	char buffer[50000];
	char* p = buffer;
	int limit = sizeof(buffer);
	int ret;
	loop
	{
		va_list arg_ptr;
		va_start(arg_ptr, format);
		ret = _vsnprintf(p, limit, format, arg_ptr);
		va_end(arg_ptr);
		if (ret >= 0 && ret < limit)
			break;
		if (p != buffer)
			delete p;
		limit *= 2;
		p = new char[limit];
		if (p == NULL)
			throw std::bad_alloc();
	}
#ifdef _MSC_VER
		if (p == buffer)
			return string(p, p + ret);
#endif
	string str(p, p + ret);
	if (p != buffer)
		delete p;
	return str;
}

uint64 GetRand(uint64 nMax)
{
	if (nMax == 0)
		return 0;

	uint64 nRange = (_UI64_MAX / nMax) * nMax;
	uint64 nRand = 0;
	do
		RAND_bytes((unsigned char*)&nRand, sizeof(nRand));
	while (nRand >= nRange);
	return (nRand % nMax);
}

void PrintException(std::exception* pex, const char* pszThread)
{
	char pszModule[260];
	pszModule[0] = '\0';
	GetModuleFileNameA(NULL, pszModule, sizeof(pszModule));
	_strlwr(pszModule);
	char pszMessage[1000];
	if (pex)
		snprintf(pszMessage, sizeof(pszMessage),
			"EXCEPTION: %s    \n%s    \n%s in %s    \n", typeid(*pex).name(), pex->what(), pszModule, pszThread);
	else
		snprintf(pszMessage, sizeof(pszMessage),
			"UNKNOWN EXCEPTION    \n%s in %s    \n", pszModule, pszThread);
	printf("\n\n*******************************\n%s", pszMessage);
	//if (wxTheApp)
	//	wxMessageBox(pszMessage, "Error", wxOK | wxICON_ERROR);
	throw;
	// DebugBreak();

}

void AddTimeData(unsigned int ip, int64 nTime)
{
	int64 nOffsetSample = nTime - GetTime();

	// Ignore duplicates
	static set<unsigned int> setKnown;
	if (!setKnown.insert(ip).second)
		return;

	// Add data
	static vector<int64> vTimeOffsets;
	if (vTimeOffsets.empty())
		vTimeOffsets.push_back(0);
	vTimeOffsets.push_back(nOffsetSample);
	printf("Added time data, samples %d, ip %08x, offset %+I64d (%+I64d minutes)\n", vTimeOffsets.size(), ip, vTimeOffsets.back(), vTimeOffsets.back() / 60);
	if (vTimeOffsets.size() >= 5 && vTimeOffsets.size() % 2 == 1)
	{
		sort(vTimeOffsets.begin(), vTimeOffsets.end());
		int64 nMedian = vTimeOffsets[vTimeOffsets.size() / 2];
		nTimeOffset = nMedian;
		if ((nMedian > 0 ? nMedian : -nMedian) > 5 * 60)
		{
			// Only let other nodes change our clock so far before we
			// go to the NTP servers
			/// todo: Get time from NTP servers, then set a flag
			///    to make sure it doesn't get changed again
		}
		foreach(int64 n, vTimeOffsets)
			printf("%+I64d  ", n);
		printf("|  nTimeOffset = %+I64d  (%+I64d minutes)\n", nTimeOffset, nTimeOffset / 60);
	}
}