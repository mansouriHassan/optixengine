#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include "Socket.h"

Socket* Socket::socket_server = nullptr;

Socket::Socket() {

    iResult = 0;

    ListenSocket = INVALID_SOCKET;
    ClientSocket = INVALID_SOCKET;

    addrinfo* result = NULL;
    struct addrinfo hints = {0};

    iSendResult = 0;
    memset(recvbuf, 0, sizeof(recvbuf));
    recvbuflen = MAX_BUFFER_SIZE;
    socket_connected = false;
}

Socket::~Socket() {
    ListenSocket = INVALID_SOCKET;
    ClientSocket = INVALID_SOCKET;
    memset(recvbuf, 0, sizeof(recvbuf));
    close_socket();
}

Socket* Socket::getInstance() {
    if (socket_server == nullptr) {
        socket_server = new Socket();
    }

    return socket_server;
}

int __cdecl Socket::socket_start(void) {
    while (true) {

        while (!socket_connected) {
            // Initialize Winsock
            iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (iResult != 0) {
                printf("WSAStartup failed with error: %d\n", iResult);
                return iResult;
            }

            ZeroMemory(&hints, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
            hints.ai_flags = AI_PASSIVE;

            // Resolve the server address and port
            iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
            if (iResult != 0) {
                printf("getaddrinfo failed with error: %d\n", iResult);
                WSACleanup();
                socket_connected = false;
                return iResult;
            }

            // Create a SOCKET for connecting to server
            ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
            if (ListenSocket == INVALID_SOCKET) {
                printf("socket failed with error: %ld\n", WSAGetLastError());
                freeaddrinfo(result);
                WSACleanup();
                socket_connected = false;
                return WSAGetLastError();
            }

            // Setup the TCP listening socket
            iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
            if (iResult == SOCKET_ERROR) {
                printf("bind failed with error: %d\n", WSAGetLastError());
                freeaddrinfo(result);
                closesocket(ListenSocket);
                WSACleanup();
                socket_connected = false;
                return WSAGetLastError();
            }
            printf("server start, waiting for client to connect ...");
            freeaddrinfo(result);

            iResult = listen(ListenSocket, SOMAXCONN);
            if (iResult == SOCKET_ERROR) {
                printf("listen failed with error: %d\n", WSAGetLastError());
                closesocket(ListenSocket);
                WSACleanup();
                socket_connected = false;
                return WSAGetLastError();
            }

            // Accept a client socket
            ClientSocket = accept(ListenSocket, NULL, NULL);
            if (ClientSocket == INVALID_SOCKET) {
                printf("accept failed with error: %d\n", WSAGetLastError());
                closesocket(ListenSocket);
                WSACleanup();
                socket_connected = false;
                return WSAGetLastError();
            }
            else {
                printf("Client connected");
                socket_connected = true;
            }
        }
    }
    return 0;
}

int Socket::socket_send(std::string& message) {
    // Echo the buffer back to the sender
    int iSendResult = 0;
    if (socket_connected) {
        if (ClientSocket != INVALID_SOCKET) {
            //message = "$" + message + "#";
            iSendResult = send(ClientSocket, message.c_str(), message.length(), 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                socket_connected = false;
            } else if (iSendResult == 0) {
                closesocket(ClientSocket);
                WSACleanup();
                socket_connected = false;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
    }
    return iSendResult;
}

std::string Socket::socket_read() {
    int iReadResult = 0;
    std::string message;
    bool reading = true;

    if (socket_connected) {
        if (ClientSocket != INVALID_SOCKET) {
            while (reading) {
                memset(recvbuf, 0, sizeof(recvbuf)); // clear buffer
                iReadResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
                message = message + std::string(recvbuf);

                if (message[0] == '$' && message[message.length() - 1] == '#') {
                    printf("%s reading : %d", __FUNCTION__, iReadResult);
                    reading = false;
                }
                if (iReadResult <= 0) {
                    printf("%s error in reading : %d", __FUNCTION__, iReadResult);
                    reading = false;
                }
            }
        }
    }
    if (message.length() > 0) {
        std::size_t first = message.find("$");
        std::size_t last = message.find("#");
        if (first >= 0 && last > 1) {
            message = message.substr(first + 1, last - first - 1);
        }
    }

    //printf("Bytes received: %d\n", message.length());
    //printf("Message received: %s\n", message.c_str());

    return message;
}

std::string Socket::socket_read_string() {
    int iReadResult = 0;
    std::string message;

    if (socket_connected) {
        if (ClientSocket != INVALID_SOCKET) {
            memset(recvbuf, 0, sizeof(recvbuf)); // clear buffer
            iReadResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
            if (iReadResult > 0) {
                message = message + std::string(recvbuf);

                printf("Bytes received: %d\n", message.length());
                printf("Message received: %s\n", message.c_str());
            }
        }
    }
    
    return message;
}

int Socket::close_socket() {
    // shutdown the connection since we're done
    int iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return iResult;
}

bool Socket::isClientConnected() {
    return socket_connected;
}