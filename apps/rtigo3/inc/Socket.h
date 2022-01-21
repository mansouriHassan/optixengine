#pragma once
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define MAX_BUFFER_SIZE 1024
#define DEFAULT_PORT "27015"

class Socket
{
private:

    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket;
    SOCKET ClientSocket;

    struct addrinfo* result;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[MAX_BUFFER_SIZE];
    int recvbuflen;
    bool socket_connected;

protected:
    static Socket* socket_server;

public:
    Socket();
    ~Socket();

    static Socket* getInstance();
    int __cdecl socket_start(void);
    int socket_send(std::string& message);
    std::string socket_read();
    std::string socket_read_string();
    int close_socket();
    bool isClientConnected();
};