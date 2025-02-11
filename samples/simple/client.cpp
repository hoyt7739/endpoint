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

#include <future>
#include "common.h"

int main(int argc, const char* argv[]) {
    endpoint::endpoint_type type = endpoint::TCP;
    if (1 < argc) {
        type = (endpoint::endpoint_type)atoi(argv[1]);
    }

    std::string remote_info;
    if (2 < argc) {
        remote_info = argv[2];
    }

    endpoint* ep = endpoint::create(type);
    if (!ep) {
        std::cout << "create endpoint failed." << std::endl;
        return -1;
    }

    std::future<void> connect_future;
    bool res = ep->connect(remote_info, [ep, &connect_future] {
        std::cout << "connected to server, " << ep->remote_info() << std::endl;
        connect_future = std::async(std::launch::async, &handle_recv, ep, true);
    });
    if (!res) {
        std::cout << "connect failed." << std::endl;
        return -1;
    }

    handle_send(ep, true);

    endpoint::destroy(ep);

    return 0;
}
