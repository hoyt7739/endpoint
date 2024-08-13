#include <iostream>
#include <sstream>
#include <iomanip>
#include "endpoint.h"

const int DATA_BUFFER_SIZE = 1024 * 10;

std::string data_to_hex(const char* data, int size) {
    std::stringstream hex_stream;
    hex_stream << std::hex;
    for (int pos = 0; pos < size; ++pos) {
        hex_stream << std::setw(2) << std::setfill('0') << (unsigned)(unsigned char)data[pos] << ' ';
    }
    return hex_stream.str();
}

int hex_to_data(const std::string& hex, char* data) {
    std::stringstream hex_stream(hex);
    hex_stream << std::hex;
    unsigned number = 0;
    int pos = 0;
    while (hex_stream >> number && pos < DATA_BUFFER_SIZE) {
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
        std::getline(std::cin, hex);
        if ("exit" == hex) {
            break;
        }

        size = hex_to_data(hex, data);
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
