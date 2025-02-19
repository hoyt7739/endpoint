/*
  MIT License

  Copyright (c) 2025 Kong Pengsheng

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

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
