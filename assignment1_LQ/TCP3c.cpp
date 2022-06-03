#include <winsock2.h>
#include <iostream>
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")
#define DATA_BUFFER 512

int main()
{
    WSADATA wsaData;
    SOCKET sClient;
    char *Serip = "192.168.9.35";
    int SeriPort = 5050;

    struct sockaddr_in seraddr;
    if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
    {
        printf("failed to load winsock\n");
        return -1;
    }

    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(SeriPort);
    seraddr.sin_addr.s_addr = inet_addr(Serip);

    sClient = socket(AF_INET,SOCK_STREAM,0);

    if(sClient == INVALID_SOCKET)
    {
        printf("creat socket failed :%d\n",WSAGetLastError());
        return -1;
    }

    FILE *fileptr = NULL;
    fileptr = fopen("C:/Documents and Settings/Administrator/桌面/多媒体测试文件.avi","rb");
    if(fileptr == NULL)
    {
        printf("file can not open!\n");
        return -1;
    }
    if(connect(sClient,(struct sockaddr*)&seraddr,sizeof(seraddr))==INVALID_SOCKET)
    {
        printf("connect() failed:%d\n",WSAGetLastError());
        return -1;
    }
    printf("cilent start to send file!\n");
    char data_buffer[512];
    memset(data_buffer,0,512);
    while(1)
    {

        if(!feof(fileptr))
        {
            int iread = fread(data_buffer,1,512,fileptr);
            int isend = send(sClient,data_buffer,iread,0);

        }
        else break;
    }
    printf("file send end successfully!\n");
    char str[100] = "quit";

    send(sClient,str,sizeof(str),0);

    fclose(fileptr);
    closesocket(sClient);
    WSACleanup();
    return 0;

}