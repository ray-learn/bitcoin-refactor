#include "winsock2.h"



static const unsigned short DEFAULT_PORT = htons(8333);
static const unsigned char pchIPv4[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff };


enum
{
	NODE_NETWORK = (1 << 0),
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


	IMPLEMENT_SERIALIZE( READWRITE(ip);)

};