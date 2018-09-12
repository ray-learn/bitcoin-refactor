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

template<typename T1>
inline uint256 Hash(const T1 pbegin, const T1 pend)
{
	uint256 hash1;
	SHA256((unsigned char*)&pbegin[0], (pend - pbegin) * sizeof(pbegin[0]), (unsigned char*)&hash1);
	uint256 hash2;
	SHA256((unsigned char*)&hash1, sizeof(hash1), (unsigned char*)&hash2);
	return hash2;
}

template<typename T1, typename T2>
inline uint256 Hash(const T1 p1begin, const T1 p1end,
	const T2 p2begin, const T2 p2end)
{
	uint256 hash1;
	SHA256_CTX ctx;
	SHA256_Init(&ctx);
	SHA256_Update(&ctx, (unsigned char*)&p1begin[0], (p1end - p1begin) * sizeof(p1begin[0]));
	SHA256_Update(&ctx, (unsigned char*)&p2begin[0], (p2end - p2begin) * sizeof(p2begin[0]));
	SHA256_Final((unsigned char*)&hash1, &ctx);
	uint256 hash2;
	SHA256((unsigend char*)&hash1, sizeof(hash1), (unsigned char*)&hash2);
	return hash2;
}

template<typename T1, typename T2, typename T3>
inline uint256 Hash(const T1 p1begin, const T1 p1end,
	const T2 p2begin, const T2 p2end,
	const T3 p3begin, const T3 p3end)
{
	uint256 hash1;
	SHA256_CTX ctx;
	SHA256_Init(&ctx);
	SHA256_Update(&ctx, (unsigned char*)&p1begin[0], (p1end - p1begin) * sizeof(p1begin[0]));
	SHA256_Update(&ctx, (unsigned char*)&p2begin[0], (p2end - p2begin) * sizeof(p2begin[0]));
	SHA256_Update(&ctx, (unsigned char*)&p3begin[0], (p3end - p3begin) * sizeof(p3begin[0]));
	SHA256_Final((unsigned char*)&hash1, &ctx);
	uint256 hash2;
	SHA256((unsigned char*)&hash1, sizeof(hash1), (unsigned char*)&hash2);
	return hash2;
}