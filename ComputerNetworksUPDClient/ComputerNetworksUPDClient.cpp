#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27015//можно сюда было 80 поставить без функции где pmk.tversu.ru

using namespace std;

sockaddr_in SenderAddr;
int SenderAddrSize = sizeof(SenderAddr);

sockaddr_in RecvAddr;

int __cdecl main(int argc, char** argv)
{
    SetConsoleOutputCP(CP_UTF8);
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    const char* sendbuf = "GET / HTTP/1.1\r\nAccept: text / html, application / xhtml + xml, application / xml; q = 0.9, image / avif, image / webp, image / apng, */*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\nAccept-Language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7\r\nCache-Control: max-age=0\r\nConnection: keep-alive\r\nHost: pmk.tversu.ru\r\nReferer: https://www.google.com/\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/116.0.0.0 Safari/537.36\r\n\r\n";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;



    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    //ZeroMemory(&hints, sizeof(hints));
    //hints.ai_family = AF_INET;
    //hints.ai_socktype = SOCK_DGRAM;
    //hints.ai_protocol = IPPROTO_UDP;


    ////Тестовая часть
    //struct sockaddr_in server_address;
    //memset(&server_address, 0, sizeof(server_address));
    //server_address.sin_family = AF_INET;
    //inet_pton(AF_INET, "localhost", &server_address.sin_addr);
    //u_short port = 27015;
    //server_address.sin_port = htons(port);


    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(DEFAULT_PORT);
    //RecvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    inet_pton(AF_INET, "127.0.0.1", &RecvAddr.sin_addr.s_addr);
    // Resolve the server address and port
   // iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
   /* if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }*/

    // Attempt to connect to an address until one succeeds
    //for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
    ConnectSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }


    freeaddrinfo(result);

    u_long iMode = 1;
    iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
    if (iResult != NO_ERROR) {
        printf("ioctlsocket failed with error: %ld\n", iResult);
    }

    // Send an initial buffer
    iResult = sendto(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));

        if (iResult == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAEWOULDBLOCK) {
                printf("Some work\n");
            }
            else {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ConnectSocket);
                WSACleanup();
                return 1;
            }
        }

    printf("Bytes Sent: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
    /*iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }*/

    // Receive until the peer closes the connection
    do {
        iResult = recvfrom(ConnectSocket, recvbuf, recvbuflen, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);

        if (iResult == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAEWOULDBLOCK) {
                printf("Some work\n");
                continue;
            }
            else {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ConnectSocket);
                WSACleanup();
                return 1;
            }
        }
        printf("Readed: %d\n", iResult);
        if (iResult > 0)
            printf("Response Headers: %s\n", recvbuf);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while (iResult > 0);

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}