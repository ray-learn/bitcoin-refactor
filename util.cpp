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