#include <stdio.h>
#include<winsock.h>
#include<string.h>
#include<iostream>
using namespace std;
#pragma comment(lib,"wsock32.lib")

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "WSAStartup  无法初始化！" << endl;
		return 0;
	}
	// 填写远程地址信息
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	while (true)
	{
		char aim_ip[100];
		cout << "请输入目的主机IP地址：";
		cin >> aim_ip;
		cout << endl;
		//设置获取的用户输入IP地址为远程IP地址 
		addr.sin_addr.S_un.S_addr = inet_addr(aim_ip);
		int port_start;
		int port_end;
		cout << "请输入起始端口号：";
		cin >> port_start;
		cout << endl;
		cout << "请输入结束端口号：";
		cin >> port_end;
		cout << endl;
		for (int i = port_start; i <= port_end; i++)
		{
			//每扫描一个端口创建一个新的套接字
			SOCKET s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (s == INVALID_SOCKET)
			{
				printf("Failed socket() %d \n", ::WSAGetLastError());
				//return -1;
			}
			//设置远程地址信息中的端口号为需要扫描的当前端口号
			addr.sin_port = htons(i);
			int ret = connect(s, (LPSOCKADDR)&addr, sizeof(addr));
			if (ret == 0)
			{
				printf("%s:%d  端口开启\n", aim_ip, i);
			}
			else
			{
				printf("%s:%d  端口关闭\n", aim_ip, i);
			}
			::closesocket(s);
		}
		cout << endl;
	}
	if (WSACleanup() == SOCKET_ERROR)
		cout << "WSACleanup 出错！" << endl;
	return 0;
}