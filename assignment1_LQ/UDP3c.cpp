//#include <stdafx.h>
#include <winsock2.h>
#include <stdio.h>
#define BUFFER_SIZE 1024
int main()
{
    //初始化服务器IP地址和端口号
    char * serip = "192.168.9.35";
    int Seriport = 5050;
    WSADATA wsadata;
    if(WSAStartup(MAKEWORD(2,2),&wsadata)!=0)
    {
        printf("failed to load winsock\n");
        return -1;
    }
    //初始化服务器....
    struct sockaddr_in seradd;
    seradd.sin_family = AF_INET;
    seradd.sin_port = htons(Seriport);
    seradd.sin_addr.s_addr = inet_addr(serip);

    SOCKET sclient;
    sclient = socket(AF_INET,SOCK_DGRAM,0);
    if(sclient == INVALID_SOCKET)
    {
        printf("build socket failed!\n");
        return -1;
    }
    FILE *fileptr = NULL;
    fileptr = fopen("C:/Documents and Settings/Administrator/桌面/多媒体测试文件.avi","rb");
    if(fileptr == NULL)
    {
        printf("file can not open!\n");
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
            int isend = sendto(sclient,data_buffer,iread,0,(struct sockaddr*)&seradd,sizeof(seradd));

        }
        else break;
    }
    printf("file send end successfully!\n");
    char str[100] = "quit";
    sendto(sclient,str,sizeof(str),0,(struct sockaddr*)&seradd,sizeof(seradd));

    fclose(fileptr);
    closesocket(sclient);
    WSACleanup();
}