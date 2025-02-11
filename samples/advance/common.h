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
#include <fstream>
#include <iomanip>
#include <map>
#include "endpoint.h"
#include "commun_command.h"

using menu_items = std::map<int, std::string>;

const int DATA_BUFFER_SIZE = 4096 * 10;

const menu_items TYPE_ITEMS = {
    {endpoint::BLUETOOTH,       "BLUETOOTH"},
    {endpoint::TCP,             "TCP"},
    {endpoint::WEBSOCKET,       "WEBSOCKET"}
};

const menu_items COMMAND_ITEMS = {
    {COMMAND_PING,              "PING"},
    {COMMAND_PONG,              "PONG"},
    {COMMAND_REQ_FRAGMENT,      "REQ_FRAGMENT"},
    {COMMAND_RSP_FRAGMENT,      "RSP_FRAGMENT"},
    {COMMAND_REQ_INTERACT,      "REQ_INTERACT"},
    {COMMAND_RSP_INTERACT,      "RSP_INTERACT"},
    {COMMAND_REQ_STATE,         "REQ_STATE"},
    {COMMAND_RSP_STATE,         "RSP_STATE"},
    {COMMAND_REQ_ASSIGN,        "REQ_ASSIGN"},
    {COMMAND_RSP_ASSIGN,        "RSP_ASSIGN"},
    {COMMAND_REQ_REPORT,        "REQ_REPORT"},
    {COMMAND_RSP_REPORT,        "RSP_REPORT"}
};

const menu_items ANSWER_ITEMS = {
    {COMMUN_ANSWER_REJECT,      "REJECT"},
    {COMMUN_ANSWER_ACCEPT,      "ACCEPT"}
};

const menu_items STATE_ITEMS = {
    {COMMUN_STATE_INVALID,      "INVALID"},
    {COMMUN_STATE_READY,        "READY"},
    {COMMUN_STATE_ERROR,        "ERROR"}
};

inline std::string data_to_hex(const char* data, int size) {
    std::stringstream hex_stream;
    hex_stream << std::hex;
    for (int pos = 0; pos < size; ++pos) {
        hex_stream << std::setw(2) << std::setfill('0') << (unsigned)(unsigned char)data[pos] << ' ';
    }
    return hex_stream.str();
}

inline int hex_to_data(const std::string& hex, char* data, int size) {
    std::stringstream hex_stream(hex);
    hex_stream << std::hex;
    unsigned number = 0;
    int pos = 0;
    while (hex_stream >> number && pos < size) {
        data[pos++] = (char)number;
    }
    return pos;
}

inline int input_item(const std::string& hint, const menu_items& items) {
    if (items.empty()) {
        return -1;
    }

    for (auto& pair : items) {
        std::cout << pair.first << " " << pair.second << '\n';
    }

    int item = -1;
    while (item < items.begin()->first || items.rbegin()->first < item) {
        std::cout << hint << ": ";
        std::cin >> item;
    }

    return item;
}

inline std::string input_text(const std::string& hint) {
    std::cout << hint << ": ";
    std::string text;
    std::getline(std::cin >> std::ws, text);
    return text;
}

inline int input_data(const std::string& hint, char* data, int size) {
    std::cout << hint << " hex: ";
    std::string hex;
    std::getline(std::cin >> std::ws, hex);
    return hex_to_data(hex, data, size);
}

inline int input_file(const std::string& hint, char* data, int size) {
    std::string path = input_text(hint + " path");
    if (path.empty()) {
        return 0;
    }

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return 0;
    }

    file.read(data, size);
    return (int)file.gcount();
}
