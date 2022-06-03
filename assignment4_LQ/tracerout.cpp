#include "stdafx.h"
#include "tracerout.h"
#include <iostream>
#include <string> 
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>



using namespace std;

const int ICMP_ECHO = 8;
const int ICMP_ECHOREPLY = 0;
const int ICMP_TIMEOUT = 11;
const int ICMP_MIN = 8;
const int MAX_PACKET = 1024;
const int DEICMP_PACKSIZE = 44;

typedef struct {
    UCHAR ipVer;
    UCHAR ipTos;
    USHORT ipLen;
    USHORT ipId;
    USHORT ipFlag;
    UCHAR ipTTL;
    UCHAR ipProtocal;
    USHORT ipChecksum;
    ULONG ipSrc;
    ULONG ipDest;
}IpHeader;

typedef struct {
    BYTE i_type;
    BYTE i_code;
    USHORT i_cksum;
    USHORT i_id;
    USHORT i_seq;
}IcmpHeader;

USHORT GetCheckSum(USHORT* buf, int size)
{
    ULONG ans = 0;
    while(size > 1)
    {
        ans += *buf++;
        size -= 2;
    }
    if( size )
    {
        ans += *(char *)buf;
    }
    
    while( ans>>16 )
    {
        ans = ( ans>>16 ) + ( ans&0xffff ) ;
    }
    
    return (USHORT)(~ans);
}
void FillIcmpContent(char* buf, int size)
{
    IcmpHeader *icmp_hdr = (IcmpHeader *)buf;
    icmp_hdr->i_type = 8;
    icmp_hdr->i_code = 0;
    icmp_hdr->i_cksum = 0;
    icmp_hdr->i_id = (USHORT)GetCurrentProcessId();
    icmp_hdr->i_seq = 0;
    memset(buf + sizeof(IcmpHeader), 'E', size - sizeof(IcmpHeader));
    
    icmp_hdr->i_cksum = GetCheckSum((USHORT *)buf, size);
}

int main()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int error;
    
    wVersionRequested = MAKEWORD( 2, 2 );
    error =  WSAStartup( wVersionRequested, &wsaData );
    
    if ( error != 0 ) {
        cout << "WSAStartup fails" << endl;
        system("pause");
        return 0;
    }
    if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) 
    {
        WSACleanup();
        cout << "The winsock dll is no use!" << endl;
        system("pause");
        return 0;
    }
    
    string ip_str ;
    cout << "trace ip : " ;
    cin >> ip_str;
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(10);
    server_addr.sin_addr.s_addr = inet_addr(ip_str.c_str());
    
    SOCKET raw_socket = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP , \
                                  NULL, 0, WSA_FLAG_OVERLAPPED);
    int timeOut = 1000;                       
    int res ;
    res = setsockopt(raw_socket, SOL_SOCKET, SO_RCVTIMEO, \
                         (char*)&timeOut, sizeof(timeOut));                   
    
    
    char icmpBuffer[sizeof(IcmpHeader) + DEICMP_PACKSIZE];
    FillIcmpContent(icmpBuffer, sizeof(icmpBuffer));
    
    int ttl = 1;
    int count = 0;
    int type ;
    do
    {
        
        setsockopt(raw_socket, IPPROTO_IP, IP_TTL, (char *)&ttl, sizeof(int));
        
        
        res = sendto(raw_socket, icmpBuffer, sizeof(icmpBuffer), 0, \
           (struct sockaddr*)&server_addr, sizeof(struct sockaddr));
        cout << "sending " << res << " bytes to " \
             << inet_ntoa(server_addr.sin_addr) << endl;
        
        struct sockaddr_in from_addr;
        int addr_len = sizeof(struct sockaddr_in);  
        char recvBuffer[MAX_PACKET];     
        res = recvfrom(raw_socket, recvBuffer, sizeof(recvBuffer), 0, \
                               (struct sockaddr*)&from_addr, &addr_len);
        
        cout << ++count << " : ";
        if(res != -1)
        {
            cout << inet_ntoa(from_addr.sin_addr) << endl;
        }
        else if(WSAETIMEDOUT == WSAGetLastError())
        {
            cout << "Time Out!" << endl;
        }
        else cout << "Unknown Error!" << endl;
        
        type = ( (IcmpHeader *)(recvBuffer + sizeof(IpHeader)) )->i_type;
        
        ++ttl;
        
    }while(type == ICMP_TIMEOUT);
    
    closesocket(raw_socket);
    WSACleanup();
    system("pause");
    return 0;
}