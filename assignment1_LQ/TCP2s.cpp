#include<winsock2.h>
#include<stdio.h>
#include<stdlib.h>

int main(){
    WSADATA wsaData;
    SOCKET oldSocket,newSocket;

    int ilen=0;
    int isend=0;
    int ircv=0;
    char recv_buffer[512];
    struct sockaddr_in seraddr,cliaddr;

    if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0){
        printf("failed to load winsocket\n");
        return -1;
    }
    oldSocket=socket(AF_INET,SOCK_STREAM,0);
    if(oldSocket==INVALID_SOCKET){
        printf("create socket failed:%d",WSAGetLastError());
        return -1;
    }
    seraddr.sin_family=AF_INET;
    seraddr.sin_port=htons(5050);
    seraddr.sin_addr.s_addr=inet_addr("192.168.9.35");
    if(bind(oldSocket,(LPSOCKADDR)&seraddr,sizeof(seraddr))==SOCKET_ERROR){
        printf("bind() failed:%d\n",WSAGetLastError());
        return -1;
    }
    printf("Server start to recive data:\n");

    if(listen(oldSocket,5)==SOCKET_ERROR){
        printf("listen() failed:%d\n",WSAGetLastError());
        return -1;
    }
    ilen=sizeof(cliaddr);
    newSocket=accept(oldSocket,(struct sockaddr*)&cliaddr,&ilen);
    if(newSocket==INVALID_SOCKET){
        printf("accept() failed:%d\n",WSAGetLastError());
        return -1;
    }
    while(1){
        memset(recv_buffer,0,512);
        ircv=recvfrom(newSocket,recv_buffer,sizeof(recv_buffer),0,(struct sockaddr*)&cliaddr,&ilen);
        if(ircv==SOCKET_ERROR){
            printf("rcv() failed:%d\n",WSAGetLastError());
            break;
        }
        else if(ircv==0)    break;
        else{
            printf("Server receive data from Client: %s\n",recv_buffer);
            printf("Client ip:[%s],port:[%d]\n",inet_ntoa(cliaddr.sin_addr),ntohs (cliaddr.sin_port));
        }
        int isnd=sendto(newSocket,recv_buffer,sizeof(recv_buffer),0,(struct sockaddr*)&cliaddr,sizeof(cliaddr));
    }
    closesocket(newSocket);
    closesocket(oldSocket);
    WSACleanup();

}
