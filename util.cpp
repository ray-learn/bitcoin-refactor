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