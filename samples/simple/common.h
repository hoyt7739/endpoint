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

#include <iostream>
#include <sstream>
#include <iomanip>
#include "endpoint.h"

const int DATA_BUFFER_SIZE = 4096;

std::string data_to_hex(const char* data, int size) {
    std::stringstream hex_stream;
    hex_stream << std::hex;
    for (int pos = 0; pos < size; ++pos) {
        hex_stream << std::setw(2) << std::setfill('0') << (unsigned)(unsigned char)data[pos] << ' ';
    }
    return hex_stream.str();
}

int hex_to_data(const std::string& hex, char* data, int size) {
    std::stringstream hex_stream(hex);
    hex_stream << std::hex;
    unsigned number = 0;
    int pos = 0;
    while (hex_stream >> number && pos < size) {
        data[pos++] = (char)number;
    }
    return pos;
}

void handle_send(endpoint* ep, bool loop) {
    if (!ep) {
        return;
    }

    char* data = new char[DATA_BUFFER_SIZE];
    int size = 0;
    std::string hex;
    do {
        std::getline(std::cin >> std::ws, hex);
        if ("exit" == hex) {
            break;
        }

        size = hex_to_data(hex, data, DATA_BUFFER_SIZE);
        if (size <= 0) {
            continue;
        }

        size = ep->send(data, size);
        std::cout << ep->remote_info() << " <- [" << size << "] " << hex << std::endl;
    } while (loop);
    delete[] data;
}

void handle_recv(endpoint* ep, bool loop) {
    if (!ep) {
        return;
    }

    char* data = new char[DATA_BUFFER_SIZE];
    int size = 0;
    std::string hex;
    do {
        size = ep->recv(data, DATA_BUFFER_SIZE);
        hex = data_to_hex(data, size);
        std::cout << ep->remote_info() << " -> [" << size << "] " << hex << std::endl;
        if (size <= 0) {
            break;
        }
    } while (loop);
    delete[] data;
}
