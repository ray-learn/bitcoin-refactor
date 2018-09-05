#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64  int64;
typedef unsigned __int64  uint64;
#else
typedef long long  int64;
typedef unsigned long long  uint64;
#endif

#define loop for (;;)
#define ARRAYLEN(array) (sizeof(array)/sizeof((array)[0]))

int64 GetTime();
int64 GetAdjustedTime();

/**
 * Used to bypass the rule against non-const reference to temporary
 * where it makes sense with wrappers such as CFlatData or CTxDB
 **/
template<typename T>
inline T& REF(const T& val)
{
	return (T&)val;
}

string strprintf(const char* format, ...);