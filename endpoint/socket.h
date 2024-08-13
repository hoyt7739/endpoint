#ifndef SOCKET_H
#define SOCKET_H

#if defined K_WINDOWS
#include <WS2tcpip.h>
#include <Windows.h>
using socket_t = SOCKET;
using socklen_t = int;
#elif defined K_LINUX
#include <unistd.h>
#include <arpa/inet.h>
using socket_t = int;
const socket_t INVALID_SOCKET = -1;
#define closesocket ::close
#endif

class socket_data {
public:
    static void init() {
        static socket_data data;
    }

private:
    socket_data() {
#ifdef K_WINDOWS
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
    }

    ~socket_data() {
#ifdef K_WINDOWS
        WSACleanup();
#endif
    }
};

#endif
