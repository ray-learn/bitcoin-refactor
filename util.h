#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64  int64;
typedef unsigned __int64  uint64;
#else
typedef long long  int64;
typedef unsigned long long  uint64;
#endif

#define foreach BOOST_FOREACH
#define loop for (;;)
#define BEGIN(a) ((char*)&(a))
#define END(a) ((char*)&((&(a))[1]))
#define ARRAYLEN(array) (sizeof(array)/sizeof((array)[0]))

uint64 GetRand(uint64 nMax);
int64 GetTime();
int64 GetAdjustedTime();

void PrintException(std::exception* pex, const char* pszThread);
void AddTimeData(unsigned int ip, int64 nTime);

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

class CTryCriticalBlock
{
protected:
	CRITICAL_SECTION* pcs;
public:
	CTryCriticalBlock(CRITICAL_SECTION& csIn) { pcs = (TryEnterCriticalSection(&csIn) ? &csIn : NULL); }
	CTryCriticalBlock(CCriticalSection& csIn) { pcs = (TryEnterCriticalSection(&csIn) ? &csIn : NULL); }
	~CTryCriticalBlock() { if (pcs) LeaveCriticalSection(pcs); }
	bool Entered() { return pcs != NULL; }
};

#define TRY_CRITICAL_BLOCK(cs)	\
	for (bool fcriticalblockonce=true;fcriticalblockonce;assert(("break caught by TRY_CRITICAL_BLOCK!",!fcriticalblockonce)), fcriticalblockonce=false) \
	for (CTryCriticalBlock criticalblock(cs);fcriticalblockonce && (fcriticalblockonce = criticalblock.Entered()) && (cs.pszFile = __FILE__, cs.nLine=__LINE__, true); fcriticalblockonce=false, cs.pszFile=NULL,cs.nLine=0)


#define CATCH_PRINT_EXCEPTION(pszFn)	\
	catch (std::exception& e) {			\
		PrintException(&e, (pszFn));	\
	} catch (...)  {					\
		PrintException(NULL, (pszFn));	\
	}	