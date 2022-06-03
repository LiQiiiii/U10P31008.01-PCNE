#include <winsock2.h>
#include <stdio.h>
#define BUFFER_SIZE 1024
int main()
{
    char * serip = "192.168.9.35";
    int Seriport = 5050;
    char send_buf[BUFFER_SIZE];
    memset(send_buf,0,sizeof(send_buf));

    WSADATA wsadata;
    if(WSAStartup(MAKEWORD(2,2),&wsadata)!=0)
    {
        printf("failed to load winsock\n");
        return ;
    }

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
    while(1)
    {
        scanf("%s",send_buf);
        sendto(sclient,send_buf,sizeof(send_buf),0,(struct sockaddr*)&seradd,sizeof(seradd));
    }
    closesocket(sclient);
    WSACleanup();
}