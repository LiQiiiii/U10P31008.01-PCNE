//#include"stdafx.h"
#include<winsock2.h>
#include<stdio.h>
#include<stdlib.h>
#define BUFFER_SIZE 1024

int main(){
    SOCKET sSocket;
    char  recv_buf[BUFFER_SIZE],send_buf[BUFFER_SIZE];

    struct sockaddr_in seradd, cliadd;
    WSADATA wsadata;

    if (WSAStartup(MAKEWORD(2,2),&wsadata)!=0){
            printf("failed to load winsocket\n");
            return -1;
        }
    sSocket=socket(AF_INET,SOCK_DGRAM,0);
    if(sSocket==INVALID_SOCKET){
        printf("socket() failed:%d/n",WSAGetLastError());
        return -1;
    }
    seradd.sin_family=AF_INET;
    seradd.sin_port=htons(5050);
    seradd.sin_addr.s_addr= inet_addr("192.168.9.35");
    if(bind(sSocket,(LPSOCKADDR)&seradd,sizeof(seradd))==SOCKET_ERROR){
        printf("地址绑定时出错:%d\n",WSAGetLastError());
        return -1;
    }
    int ilen=sizeof(cliadd);

    memset(recv_buf,0,sizeof(recv_buf));
    printf("Server start to recive data:\n");
    while(1){
        int irecv=recvfrom(sSocket,recv_buf,sizeof(recv_buf),0,(struct sockaddr*)&cliadd,&ilen);
        if(irecv==SOCKET_ERROR){
            printf("接收时出错%d\n",WSAGetLastError());
            return-1;
        }
        else if(irecv==0)
            break;
        else{
            printf("Server received from Client %s\n",recv_buf);
            printf("Client ip:[%s],port:[%d]\n",inet_ntoa(cliadd.sin_addr),ntohs (cliadd.sin_port));
        }
        int isend = sendto(sSocket, recv_buf,sizeof(recv_buf),0,(struct sockaddr*)&cliadd,sizeof(cliadd));
    }
    closesocket(sSocket);
    WSACleanup();
}
