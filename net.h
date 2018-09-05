#include "winsock2.h"



static const unsigned short DEFAULT_PORT = htons(8333);
static const unsigned char pchIPv4[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff };


enum
{
	NODE_NETWORK = (1 << 0),
};

/** 
 * Message header: (4) message start, (12) command, (4) size
 * The message start string is designed to be unlikely to occur in normal data.
 * The characters are rarely used upper ascii, not valid as UTF-8, and produce a large 4-byte int at any alignment.
 **/
static const char pchMessageStart[4] = { 0xf9, 0xbe, 0xb4, 0xd9 };

class CMessageHeader
{
public:
	enum { COMMAND_SIZE = 12 };
	char pchMessageStart[sizeof(::pchMessageStart)];
	char pchCommand[COMMAND_SIZE];
	unsigned int nMessageSize;

	CMessageHeader()
	{
		memcpy(pchMessageStart, ::pchMessageStart, sizeof(pchMessageStart));
		memset(pchCommand, 0, sizeof(pchCommand));
		pchCommand[1] = 1;
		nMessageSize = -1;
	}

	IMPLEMENT_SERIALIZE
	(
		READWRITE(FLATDATA(pchMessageStart));
		READWRITE(FLATDATA(pchCommand));
		READWRITE(nMessageSize);
	)

	string GetCommand()
	{
		if (pchCommand[COMMAND_SIZE - 1] == 0)
			return string(pchCommand, pchCommand + strlen(pchCommand));
		else
			return string(pchCommand, pchCommand + COMMAND_SIZE);
	}

	bool IsValid()
	{
		if (memcmp(pchMessageStart, ::pchMessageStart, sizeof(pchMessageStart)) != 0)
			return false;
		
		for (char* p1 = pchCommand; p1 < pchCommand + COMMAND_SIZE; p1++)
		{
			if (*p1 == 0)
			{
				for (; p1 < pchCommand + COMMAND_SIZE; p1++)
					if (*p1 != 0)
						return false;
			}
			else if (*p1 < ' ' || *p1 > 0x7E)
				return false;
		}

		if (nMessageSize > 0x10000000)
		{
			printf("CMessageHeader::IsValid() : nMessageSize too large %u\n", nMessageSize);
			return false;
		}

		return true;
	}
};

class CAddress
{
public:
    uint64 nServices;
    unsigned char pchReserved[12];
    unsigned int ip;
    unsigned short port;

    unsigned int nTime;			// disk only
    unsigned int nLastFailed;	// memory only 

    CAddress()
    {
        nServices = 0;
        memcpy(pchReserved, pchIPv4, sizeof(pchReserved));
        ip = 0;
        port = DEFAULT_PORT;
        nTime = GetAdjustedTime();
        nLastFailed = 0;
    }

	CAddress(unsigned int ipIn, unsigned short portIn, uint64 nServicesIn = 0)
	{
		nServices = nServicesIn;
		memcpy(pchReserved, pchIPv4, sizeof(pchReserved));
		ip = ipIn;
		port = portIn;
		nTime = GetAdjustedTime();
		nLastFailed = 0;
	}

	explicit CAddress(const struct sockaddr_in& sockaddr, uint64 nServicesIn = 0)
	{
		nServices = nServicesIn;
		memcpy(pchReserved, pchIPv4, sizeof(pchReserved));
		ip = sockaddr.sin_addr.s_addr;
		port = sockaddr.sin_port;
		nTime = GetAdjustedTime();
		nLastFailed = 0;
	}

	explicit CAddress(const char* pszIn, uint64 nServicesIn = 0)
	{
		nServices = nServicesIn;
		memcpy(pchReserved, pchIPv4, sizeof(pchReserved));
		ip = 0;
		port = DEFAULT_PORT;
		nTime = GetAdjustedTime();
		nLastFailed = 0;

		char psz[100];
		if (strlen(pszIn) > ARRAYLEN(psz) - 1)
			return;
		strcpy(psz, pszIn);
		unsigned int a, b, c, d, e;
		if (sscanf(psz, "%u.%u.%u.%u:%u", &a, &b, &c, &d, &e) < 4)
			return;
		char* pszPort = strchr(psz, ':');
		if (pszPort)
		{
			*pszPort++ = '\0';
			port = htons(atoi(pszPort));
		}
		ip = inet_addr(psz);
	}


	IMPLEMENT_SERIALIZE
	( 
		if (nType & SER_DISK)
		{
			READWRITE(nVersion);
			READWRITE(nTime);
		}
		READWRITE(nServices);
		READWRITE(FLATDATA(pchReserved));
		READWRITE(ip);
		READWRITE(port);
	)

	friend inline bool operator==(const CAddress& a, const CAddress& b)
	{
		return (memcmp(a.pchReserved, b.pchReserved, sizeof(a.pchReserved)) == 0 &&
			a.ip == b.ip &&
			a.port == b.port);
	}

	friend inline bool operator<(const CAddress& a, const CAddress& b)
	{
		int ret = memcmp(a.pchReserved, b.pchReserved, sizeof(a.pchReserved));
		if (ret < 0)
			return true;
		else if (ret == 0)
		{
			if (ntohl(a.ip) < ntohl(b.ip))
				return true;
			else if (a.ip == b.ip)
				return ntohs(a.port) < ntohs(b.port);
		}
		return false;
	}

	vector<unsigned char> GetKey() const
	{
		CDataStream ss;
		ss.reverse(18);
		ss << FLATDATA(pchReserved) << ip << port;

		#if defined(_MSC_VER) && _MSC_VER < 1300
		return vector<unsigned char>((unsigned char*)&ss.begin()[0], (unsigned char*)&ss.end()[0]);
		#else
		return vector<unsigned char>(ss.begin(), ss.end());
		#endif
	}

	struct sockaddr_in GetSockAddr() const
	{
		struct sockaddr_in sockaddr;
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_addr.s_addr = ip;
		sockaddr.sin_port = port;
		return sockaddr;
	}

	bool IsIPv4() const
	{
		return (memcmp(pchReserved, pchIPv4, sizeof(pchIPv4)) == 0);
	}

	bool IsRoutable() const
	{
		return !(GetByte(3) == 10 || (GetByte(3) == 192 && GetByte(2) == 168));
	}

	unsigned char GetByte(int n) const
	{
		return ((unsigned char*)&ip)[3 - n];
	}

	string ToStringIPPort() const
	{
		return strprintf("%u.%u.%u.%u:%u", GetByte(3), GetByte(2), GetByte(1), GetByte(0), ntohs(port));
	}

	string ToStringIP() const
	{
		return strprintf("%u.%u.%u.%u", GetByte(3), GetByte(2), GetByte(1), GetByte(0));
	}

	string ToString() const
	{
		return strprintf("%u.%u.%u.%u:%u", GetByte(3), GetByte(2), GetByte(1), GetByte(0), ntohs(port));
	}

	void print() const
	{
		printf("CAddress(%s)\n", ToString().c_str());
	}
};

enum
{
	MSG_TX = 1,
	MSG_BLOCK,
	MSG_REVIEW,
	MSG_PRODUCT,
	MSG_TABLE,
};