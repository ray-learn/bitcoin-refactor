#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64  int64;
typedef unsigned __int64  uint64;
#else
typedef long long  int64;
typedef unsigned long long  uint64;
#endif

#define loop for (;;)
#define ARRAYLEN(array) (sizeof(array)/sizeof((array)[0]))

uint64 GetRand(uint64 nMax);
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

class CCriticalSection
{
protected:
	CRITICAL_SECTION cs;
public:
	char* pszFile;
	int nLine;
	explicit CCriticalSection() { InitializeCriticalSection(&cs); }
	~CCriticalSection() { DeleteCriticalSection(&cs); }
	void Enter() { EnterCriticalSection(&cs); }
	void Leave() { LeaveCriticalSection(&cs); }
	bool TryEnter() { return TryEnterCriticalSection(&cs); }
	CRITICAL_SECTION* operator&() { return &cs; }
};

// Automatically leave critical section when leaving block, needed for exception safety
class CCriticalBlock
{
protected:
	CRITICAL_SECTION* pcs;
public:
	CCriticalBlock(CRITICAL_SECTION& csIn) { pcs = &csIn; EnterCriticalSection(pcs); }
	CCriticalBlock(CCriticalSection& csIn) { pcs = &csIn; EnterCriticalSection(pcs); }
	~CCriticalBlock() { LeaveCriticalSection(pcs); }
};

#define CRITICAL_BLOCK(cs)	\
	for (bool fcriticalblockonce=true;fcriticalblockonce;assert(("break caught by CRITICAL_BLOCK!",!fcriticalblockonce)), fcriticalblockonce=false)	\
	for (CCriticalBlock criticalblock(cs);fcriticalblockonce && (cs.pszFile=__FILE__,cs.nLine=__LINE__,true);fcriticalblockonce=false,cs.pszFile=NULL,cs.nLine=0)
