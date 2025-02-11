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

#include "endpoint.h"
#include "bluetooth.h"
#include "tcp.h"
#include "websocket.h"

using namespace std::placeholders;

template<class data_t, class func_t>
bool full_trans(data_t buffer, int size, func_t func) {
    if (!buffer || size <= 0) {
        return false;
    }

    int count = 0;
    while (count < size) {
        int piece = func(buffer + count, size - count);
        if (piece <= 0) {
            return false;
        }

        count += piece;
    }

    return true;
}

endpoint* endpoint::create(endpoint_type type, const std::string& info) {
    switch (type) {
    case BLUETOOTH:
        return new bluetooth(info);
    case TCP:
        return new tcp(info);
    case WEBSOCKET:
        return new websocket(info);
    }

    return nullptr;
}

void endpoint::destroy(endpoint* ep) {
    delete ep;
}

bool endpoint::full_send(const char* buffer, int size) {
    return full_trans(buffer, size, std::bind(&endpoint::send, this, _1, _2));
}

bool endpoint::full_recv(char* buffer, int size) {
    return full_trans(buffer, size, std::bind(&endpoint::recv, this, _1, _2));
}
