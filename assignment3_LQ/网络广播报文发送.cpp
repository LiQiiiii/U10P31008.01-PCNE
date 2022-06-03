#include <iostream>
#include "winsock.h"
#include "windows.h"
#include <stdio.h>
#pragma comment(lib,"wsock32.lib")
#define RECV_PORT 2222
#define SEND_PORT 3306
BOOL optReturn=TRUE;
SOCKET sock;
struct sockaddr_in sockAddrFrom,sockAddrTo,sockAddrIn;

DWORD CreateSocket()
{
    WSADATA WSAData;
    if(WSAStartup(MAKEWORD(2,2),&WSAData)!=0)
    {
        printf("socket lib load error!");
        return false;
    }
    if((sock=socket(AF_INET,SOCK_DGRAM,0))==INVALID_SOCKET)
    {
        printf("create socket failed!\n");
        WSACleanup();
        return false;
    }
    sockAddrIn.sin_family=AF_INET;
    sockAddrIn.sin_addr.s_addr=INADDR_ANY;
    sockAddrIn.sin_port=htons(SEND_PORT);
    if(bind(sock,(LPSOCKADDR)&sockAddrIn,sizeof(sockAddrIn)))
    {
        closesocket(sock);
        WSACleanup();
        return false;
    }
    if(setsockopt(sock,SOL_SOCKET,SO_BROADCAST,(char *)&optReturn,sizeof(optReturn)==SOCKET_ERROR))
    {
        closesocket(sock);
        WSACleanup();
        return false;
    }
    return true;
}

DWORD BroadDataSend(char lpBuffer[])
{
    int lengthSend=0;
    sockAddrTo.sin_family=AF_INET;
    sockAddrTo.sin_addr.s_addr=INADDR_BROADCAST;
    sockAddrTo.sin_port=htons(RECV_PORT);
    if((lengthSend = sendto(sock,lpBuffer,strlen(lpBuffer),MSG_DONTROUTE,(struct sockaddr*)&sockAddrTo,
                            sizeof(sockaddr)))==SOCKET_ERROR)
    {
        closesocket(sock);
        WSACleanup();
        return FALSE;
    }
    return true;
}
int main()
{
    char buffer[100];
    int i;
    CreateSocket();
    printf("press any key to continue!\n");
    getchar();
    for(i=0;i<100;i++)
    {
        sprintf(buffer,"data %d",i);
        BroadDataSend(buffer);
        Sleep(50);
    }
    getchar();
    return true;
}



