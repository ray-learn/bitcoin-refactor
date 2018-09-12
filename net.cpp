#include <iostream>

#include "headers.h"

void ThreadMessageHandler2(void* parg);

bool fShutdown = false;
array<bool, 10> vfThreadRunning;
bool fClient = false;
uint64 nLocalServices = (fClient ? 0 : NODE_NETWORK);
CAddress addrLocalHost(0, DEFAULT_PORT, nLocalServices);
map<CInv, int64> mapAlreadyAskedFor;
vector<CNode*> vNodes;
CCriticalSection cs_vNodes;

void main(int argc, char * argv)
{
	sockaddr_in saddr_test;
	saddr_test.sin_family = AF_INET;
	saddr_test.sin_addr.s_addr = inet_addr("192.168.1.1");
	saddr_test.sin_port = htons(8333);
	CAddress caddress_test(saddr_test);

	//SetConsoleCP(65001);			// 设置Input和Output代码页进行序列化测试
	//SetConsoleOutputCP(65001);
	caddress_test.Serialize(cout);
	cout << "\nthe serialize size value is " << (char)caddress_test.GetSerializeSize();
	cout << "\nthe GetKey method result is " << caddress_test.GetKey()[0] <<endl;
	cout << "\nthe GetByte method result is " << caddress_test.GetByte(3) << endl;
	SOCKET test_socket = socket(PF_INET, SOCK_STREAM, 0);

	uint160 test_uint160;
	uint256 test_uint256;
	vector<string> test_vstr{ "abc" };
	CCriticalSection test_cs;
	CNode cn(test_socket, caddress_test);

	/*Testuint256AdHoc(test_vstr);*/
	/*cout << "\n the operator !() result is " << test_uint160 << endl; */

	CInv test_cinv;
	cn.AddInventoryKnown(test_cinv);
	cn.AskFor(test_cinv);

	cn.BeginMessage("abc");
	cn.AbortMessage();
	cn.EndMessage();
	cn.EndMessageAbortIfEmpty();
	cn.GetMessageCommand();
	cn.PushMessage("abcd");

	cout << "\n GetRand " << GetRand(3);

	ThreadMessageHandler2("gogo");
}

void ThreadMessageHandler2(void* parg)
{
	extern bool ProcessMessages(CNode* pnode);
	printf("ThreadMessageHandler started\n");
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
	/*loop
	{*/
		// Poll the connected nodes for messages
		vector<CNode*> vNodesCopy;
		CRITICAL_BLOCK(cs_vNodes)
			vNodesCopy = vNodes;

		// TODO analysis the foreach procedure
		foreach(CNode* pnode, vNodesCopy)
		{
			pnode->AddRef();

			TRY_CRITICAL_BLOCK(pnode->cs_vRecv)
				ProcessMessages(pnode);

			//TRY_CRITICAL_BLOCK(pnode->cs_vSend)
			//	SendMessages(pnode);
		}
	/*	break;
	}*/
}

void CheckForShutdown(int n)
{
	if (fShutdown)
	{
		if (n != -1)
			vfThreadRunning[n] = false;
		_endthread();
	}
}
