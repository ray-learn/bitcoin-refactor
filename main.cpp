#include "headers.h"
#include <iostream>

void main(int argc, char * argv)
{
	sockaddr_in saddr_test;
	saddr_test.sin_family = AF_INET;
	saddr_test.sin_addr.s_addr = inet_addr("192.168.1.1");
	saddr_test.sin_port = htons(8333);
	CAddress caddress_test(saddr_test);

	SetConsoleCP(65001);			// 设置Input和Output代码页进行序列化测试
	SetConsoleOutputCP(65001);		
	caddress_test.Serialize(std::cout);
	std::cout << "\nthe serialize size value is " << caddress_test.GetSerializeSize();
	std::cout << "\nhello bitcoin refactor" << std::endl;
}