#include <winsock2.h>
#include <stdio.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <time.h>
#pragma comment(lib,"WS2_32.lib")
#pragma warning(disable:4996)
DWORD WINAPI ScanThread(LPVOID port);

int main(int argc, char* argv[])
{
    WSADATA  wsd;
    int port = 0;
    int MAX_PORT;
    clock_t start, end;
    HANDLE handle;
    DWORD dwThreadId;
    //Initialize socket lib
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
    {
        printf("WSAStartup failed!\n");
        return 1;
    }
    printf("请输入要扫描的最大端口：");
    scanf("%d", &MAX_PORT);
    printf("Scaning......\n");
    start = clock();
    //扫描的主要代码，根据需要删减
    do {
        handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ScanThread, (LPVOID)port, 0, &dwThreadId);
        port++;
    } while (port < MAX_PORT);
    WaitForSingleObject(handle, INFINITE);//等待最后一个线程结束
    end = clock();
    int duration = end - start;
    printf("总耗时 %d ms", duration);
    system("pause");
    return 0;
}

DWORD WINAPI ScanThread(LPVOID port)
{
    int Port = (int)(LPVOID)port;
    int retval;//调用各种socket函数的返回值
    SOCKET sHost;
    SOCKADDR_IN servAddr;
    sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == sHost)
    {
        printf("socket failed!\n");
        WSACleanup();
        return -1;
    }
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    //setsockopt (sHost, IPPROTO_TCP, TCP_MAXRT, (char *)&Timeout, sizeof (Timeout));//设置快速扫描
    servAddr.sin_port = htons(Port);
    retval = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));//lpsockaddr is 环路地址
    if (retval == SOCKET_ERROR) {
        printf("端口%d关闭！\n", Port);  //这里不要使用WSACleanup（）函数，不然后续的线程会创建不了socket
        closesocket(sHost);
        return -1;
    }
    printf("端口%d开放！\n", Port);
    closesocket(sHost);
    return 1;
}