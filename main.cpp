/**
 * This the main framework cpp file
 **/
#include "headers.h"

int nDropMessagesTest = 0;
CCriticalSection cs_main;
map<uint256, CBlockIndex*> mapBlockIndex;

bool ProcessMessages(CNode* pfrom)
{
	CDataStream& vRecv = pfrom->vRecv;
	if (vRecv.empty())
		return true;
	printf("ProcessMessages(%d bytes)\n", vRecv.size());

	// Message format
	// (4) message start
	// (12) command
	// (4) size
	// (x) data

	loop
	{
		// Scan for message start
		CDataStream::iterator pstart = search(vRecv.begin(), vRecv.end(), BEGIN(pchMessageStart), END(pchMessageStart));
		if (vRecv.end() - pstart < sizeof(CMessageHeader))
		{
			if (vRecv.size() > sizeof(CMessageHeader))
			{
				printf("\n\nPROCESSMESSAGE MESSAGESTART NOT FOUND\n\n");
				vRecv.erase(vRecv.begin(), vRecv.end() - sizeof(CMessageHeader));
			}
			break;
		}
		if (pstart - vRecv.begin() > 0)
			printf("\n\nPROCESSMESSAGE SKIPPED %d BYTES\n\n", pstart - vRecv.begin());
		vRecv.erase(vRecv.begin(), pstart);

		// Read header
		CMessageHeader hdr;
		vRecv >> hdr;
		if (!hdr.IsValid())
		{
			printf("\n\nPROCESSMESSAGE: ERRORS IN HEADER %s\n\n\n", hdr.GetCommand().c_str());
			continue;
		}
		string strCommand = hdr.GetCommand();

		// Message size
		unsigned int nMessageSize = hdr.nMessageSize;
		if (nMessageSize > vRecv.size())
		{
			printf("MESSAGE-BREAK  2\n");
			vRecv.insert(vRecv.begin(), BEGIN(hdr), END(hdr));
			Sleep(100);
			break;
		}

		CDataStream vMsg(vRecv.begin(), vRecv.begin() + nMessageSize, vRecv.nType, vRecv.nVersion);
		vRecv.ignore(nMessageSize);
		 
		// Process message
		bool fRet = false;
		try
		{
			CheckForShutdown(2);
			CRITICAL_BLOCK(cs_main)
				fRet = ProcessMessage(pfrom, strCommand, vMsg);
			CheckForShutdown(2);
		}
		CATCH_PRINT_EXCEPTION("ProcessMessage()")
			if (!fRet)
				printf("ProcessMessage(%s, %d bytes) from %s to %s FAILED\n", strCommand.c_str(), nMessageSize, pfrom->addr.ToString().c_str(), addrLocalHost.ToString().c_str());
	}
	
	vRecv.Compact();
	return true;
}

bool ProcessMessage(CNode* pfrom, string strCommand, CDataStream& vRecv)
{
	static map<unsigned int, vector<unsigned char> > mapReuseKey;
	printf("received: %-12s (%d bytes) ", strCommand.c_str(), vRecv.size());
	for (int i = 0; i < min(vRecv.size(), (unsigned int)25); i++)
		printf("%02x ", vRecv[i] & 0xff);
	printf("\n");
	if (nDropMessagesTest > 0 && GetRand(nDropMessagesTest) == 0)
	{
		printf("dropmessages DROPPING RECV MESSAGE\n");
		return true;
	}

	if (strCommand == "version")
	{
		// Can only do this once
		if (pfrom->nVersion != 0)
			return false;

		int64 nTime;
		CAddress addrMe;
		vRecv >> pfrom->nVersion >> pfrom->nServices >> nTime >> addrMe;
		if (pfrom->nVersion == 0)
			return false;

		pfrom->vSend.SetVersion(min(pfrom->nVersion, VERSION));
		pfrom->vRecv.SetVersion(min(pfrom->nVersion, VERSION));

		pfrom->fClient = !(pfrom->nServices & NODE_NETWORK);
		if (pfrom->fClient)
		{
			pfrom->vSend.nType |= SER_BLOCKHEADERONLY;
			pfrom->vRecv.nType |= SER_BLOCKHEADERONLY;
		}

		AddTimeData(pfrom->addr.ip, nTime);

		// Ask the first connected node for block updates
		static bool fAskedForBlocks;
		if (!fAskedForBlocks && !pfrom->fClient)
		{
			fAskedForBlocks = true;
			/*pfrom->PushMessage("getblocks", CBlockLocator(pindexBest), uint256(0));*/
		}
		printf("version addrMe = %s\n", addrMe.ToString().c_str());
	}
}