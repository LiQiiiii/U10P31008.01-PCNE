#include "conio.h"
#include "iostream"
#include "stdio.h"
#include "string.h"
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")
#define MAX_SiZE 4096
char cmdBuf[MAX_SiZE];
char command[MAX_SiZE];
char ReplyMsg[MAX_SiZE];
int nReplyCode;
bool bConnected = false;
SOCKET SocketControl;
SOCKET SocketDATA;

bool RecvReply(SOCKET sock)
{
    int nRecv = 0;
    memset(ReplyMsg, 0, MAX_SiZE);
    nRecv = recv(sock, ReplyMsg, MAX_SiZE, 0);
    if (nRecv == SOCKET_ERROR)
    {
        //cout<<endl << "socket recv failed!"<<endl;
        printf("socket recv failed!\n");
        closesocket(sock);
        return false;
    }
    if (nRecv > 4)
    {
        char *ReplyCodes = new char[3];
        memset(ReplyCodes, 0, 3);
        memcpy(ReplyCodes, ReplyMsg, 3);
        nReplyCode = atoi(ReplyCodes);
    }
    return true;
}

bool Sendcommand()
{
    int nSend;
    nSend = send(SocketControl, command, strlen(command), 0);
    if (nSend == SOCKET_ERROR)
    {
        printf("SocketControl create error:%d\n", WSAGetLastError());
        return false;
    }
    return true;
}

bool DataConnect(char *ServeripAddr)
{
    //向FTP服务器发送PASV命令
    memset(command, 0, MAX_SiZE);
    memcpy(command, "PASV", strlen("PASV"));
    memcpy(command + strlen("PASV"), "\r\n", 2);
    if (!Sendcommand())
    {
        return false;
    }
    //获得PASV命令的应答消息
    if (RecvReply(SocketControl))
    {
        if (nReplyCode != 227)
        {
            printf("PASV命令应答错误！");
            closesocket(SocketControl);
            return false;
        }
    }
    printf("%s\n",ReplyMsg);
    //解析PASV命令和应答消息
    char *part[6];
    if (strtok(ReplyMsg, "("))
    {
        for (int i = 0; i < 5; i++)
        {
            part[i] = strtok(NULL, ",");
            if (!part[i])
            {
                return false;
            }
        }
        part[5] = strtok(NULL, ")");
        if (!part[5])
            return false;
    }
    else
        return false;
    //获得FTP服务器的数据端口号
    unsigned short serverPort;
    serverPort = (unsigned short)((atoi(part[4]) << 8) + atoi(part[5]));
    SocketDATA = socket(AF_INET, SOCK_STREAM, 0); //创建数据SOCKET
    if (SocketDATA == INVALID_SOCKET)
    {
        printf("data socket creat error: %d", WSAGetLastError());
        return false;
    }
    SOCKADDR_IN server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(serverPort);
    //server_addr.sin_addr.s_addr = inet_addr(ServeripAddr);
    server_addr.sin_addr.S_un.S_addr = inet_addr(ServeripAddr);

    //与FTP服务器之间建立数据TCP连接
    int nConnect = connect(SocketDATA, (sockaddr *)&server_addr, sizeof(server_addr));
    if (nConnect == SOCKET_ERROR)
    {
        printf("create data TCP connection error : %d\n", WSAGetLastError());
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("please input param as the following: ftpclient ftpiPaddr\n");
        return false;
    }
    if (bConnected == true)
    {
        printf("client has established the TCP control connection with server\n");
        closesocket(SocketControl);
    }

    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 2), &WSAData);
    SocketControl = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(21);
    server_addr.sin_addr.S_un.S_addr = inet_addr(argv[1]);
    int nConnect = connect(SocketControl, (sockaddr *)&server_addr, sizeof(server_addr));
    if (nConnect == SOCKET_ERROR)
    {
        printf("client could not establish the FTP control connection with server\n");
        return false;
    }
    //获取Connect应答消息
    if (RecvReply(SocketControl))
    {
        printf("%d\n", nReplyCode);
        if (nReplyCode == 220) //判断应答Code

            printf("%s \n", ReplyMsg);
        else
        {
            printf("the reply msg is error\n");
            closesocket(SocketControl);
            return false;
        }
    }

    //向服务器发送USER命令
    printf("FTP->USER:");
    memset(cmdBuf, 0, MAX_SiZE);
    gets(cmdBuf); //输入用户名并保存
    memset(command, 0, MAX_SiZE);
    memcpy(command, "USER ", strlen("USER "));
    memcpy(command + strlen("USER "), cmdBuf, strlen(cmdBuf));
    memcpy(command + strlen("USER ") + strlen(cmdBuf), "\r\n", 2);

    if (!Sendcommand())
        return 0;
    //获得USER命令的应答信息
    if (RecvReply(SocketControl))
    {
        //printf("%d\n", nReplyCode);
        if (nReplyCode == 220 || nReplyCode == 331)
            printf("%s", ReplyMsg);
        else
        {
            printf("USER命令应答错误\n");
            closesocket(SocketControl);
            return false;
        }
    }
    if (nReplyCode == 331)
    {
        //向FTP服务器发送PASS命令
        printf("FTP-> PASS:");
        memset(cmdBuf, 0, MAX_SiZE);
        gets(cmdBuf);
        memset(command, 0, MAX_SiZE);
        memcpy(command, "PASS ", strlen("PASS "));
        memcpy(command + strlen("PASS "), cmdBuf, strlen(cmdBuf));
        memcpy(command + strlen("PASS ") + strlen(cmdBuf), "\r\n", 2);
        //获得PASV命令的应答信息
        if (!Sendcommand())
            return 0;
        //printf("%s\n",command);
        if (RecvReply(SocketControl))
        {
            Sleep(1000);
            printf("%d\n", nReplyCode);
            if (nReplyCode == 230)
                printf("%s", ReplyMsg);
            else
            {
                printf("PASS命令应答错误\n");
                closesocket(SocketControl);
                return false;
            }
        }
    }

    //
    DataConnect(argv[1]);

    //
    printf("FTP->LIST:\n");
    memset(command, 0, MAX_SiZE);
    memcpy(command, "LIST /", strlen("LIST /")); // /为参数
    memcpy(command + strlen("LIST /"), "\r\n", 2);
    if (!Sendcommand())
        return false;
    //获得quit命令的应答信息
    if (RecvReply(SocketControl))
    {
        printf("%s", ReplyMsg);
    }
    if (RecvReply(SocketDATA))
    {
        printf("%s", ReplyMsg);
    }
    if (RecvReply(SocketControl))
    {
        printf("%s", ReplyMsg);
    }
    //Sleep(1000);

    //向FTP服务器发送quit命令
    printf("FTP->QUIT:\n");
    memset(command, 0, MAX_SiZE);
    memcpy(command, "QUIT", strlen("QUIT"));
    memcpy(command + strlen("QUIT"), "\r\n", 2);
    if (!Sendcommand())
        return false;
    //获得quit命令的应答信息
    if (RecvReply(SocketControl))
    {
        printf("%s", ReplyMsg);
        if (nReplyCode == 221)
        {
            printf("%s", ReplyMsg);
            bConnected = false;
            closesocket(SocketControl);
            return true;
        }
        else
        {
            printf("QUiT命令应答错误\n");
            closesocket(SocketControl);
            return false;
        }
    }
    WSACleanup();
    return 0;
}
