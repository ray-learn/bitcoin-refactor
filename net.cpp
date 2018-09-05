#include <iostream>
#include "winsock2.h"
#include "headers.h"


bool fClient = false;
uint64 nLocalServices = (fClient ? 0 : NODE_NETWORK);
CAddress addrLocalHost(0, DEFAULT_PORT, nLocalServices);

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
}