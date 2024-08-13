#include "ws_message.h"
#include <cstring>
#include <sstream>
#include <map>
#include <random>
#include "base64/base64.h"
#include "sha1/sha1.h"

const char* const SPACE_SET = "\t\r\n ";
const int MAX_FRAME_HEADER_SIZE = 14;

using line_info_t = std::vector<std::string>;
using header_info_t = std::map<std::string, std::string>;

inline std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(SPACE_SET);
    if (std::string::npos == first) {
        return std::string();
    }
    size_t last = str.find_last_not_of(SPACE_SET);
    return str.substr(first, last + 1 - first);
}

inline int unpack_http(const char* message, int size, line_info_t& line_info, header_info_t& header_info) {
    if (!message || size < 4) {
        return 0;
    }

    std::string data(message, size);
    size_t end_pos = data.find("\r\n\r\n");
    if (std::string::npos == end_pos) {
        return 0;
    }

    std::istringstream data_is(data.erase(end_pos + 4));

    std::string text;
    std::getline(data_is, text);
    std::istringstream line_is(text);
    while (line_is >> text) {
        line_info.push_back(text);
    }

    while (std::getline(data_is, text)) {
        size_t delim_pos = text.find(':');
        if (std::string::npos != delim_pos) {
            header_info[trim(text.substr(0, delim_pos))] = trim(text.substr(delim_pos + 1));
        }
    }

    return (int)data.size();
}

int pack_handshake(const std::string& host, const std::string& key, pack_output output) {
    if (host.empty() || key.empty() || !output) {
        return 0;
    }

    std::string data =
        "GET / HTTP/1.1\r\n"
        "Connection: Upgrade\r\n"
        "Upgrade: websocket\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "Sec-WebSocket-Key: ";
    data += key;
    data += "\r\nHost: ";
    data += host;
    data += "\r\nOrigin: http://";
    data += host;
    data += "\r\n\r\n";

    int size = (int)data.size();
    output(data.c_str(), size);

    return size;
}

int unpack_handshake(const char* message, int size, unpack_handshake_output output) {
    if (!output) {
        return 0;
    }

    line_info_t line_info;
    header_info_t header_info;
    int res = unpack_http(message, size, line_info, header_info);
    if (res <= 0) {
        return res;
    }

    std::string& key = header_info["Sec-WebSocket-Key"];
    bool accept = header_info["Connection"] == "Upgrade" &&
                  header_info["Upgrade"] == "websocket" &&
                  header_info["Sec-WebSocket-Version"] == "13" &&
                  !key.empty();
    output(accept, key);

    return res;
}

int pack_rhandshake(const std::string& key, pack_output output) {
    if (key.empty() || !output) {
        return 0;
    }

    std::string data =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Connection: Upgrade\r\n"
        "Upgrade: websocket\r\n"
        "Sec-WebSocket-Accept: ";
    data += base64_encode(SHA1(key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11").final());
    data += "\r\n\r\n";

    int size = (int)data.size();
    output(data.c_str(), size);

    return size;
}

int unpack_rhandshake(const char* message, int size, const std::string& key, unpack_rhandshake_output output) {
    if (key.empty() || !output) {
        return 0;
    }

    line_info_t line_info;
    header_info_t header_info;
    int res = unpack_http(message, size, line_info, header_info);
    if (res <= 0) {
        return res;
    }

    std::string sec = base64_encode(SHA1(key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11").final());
    bool accept = 2 <= line_info.size() && line_info[1] == "101" &&
                  header_info["Connection"] == "Upgrade" &&
                  header_info["Upgrade"] == "websocket" &&
                  header_info["Sec-WebSocket-Accept"] == sec;
    output(accept);

    return res;
}

int pack_frame(ws_opcode opcode, bool mask, const char* payload, int size, pack_output output) {
    if (size < 0 || (0 < size && !payload) || !output) {
        return 0;
    }

    unsigned char* message = new unsigned char[MAX_FRAME_HEADER_SIZE + size];

    int pos = 0;
    message[pos++] = 0x80 | opcode;
    message[pos] = mask ? 0x80 : 0;
    if (size < 126) {
        message[pos++] |= size;
    } else if (126 <= size && size < 0x10000) {
        message[pos++] |= 126;
        message[pos++] = (size >> 8) & 0xFF;
        message[pos++] = size & 0xFF;
    } else {
        message[pos++] |= 127;
        message[pos++] = (int64_t)size >> 56 & 0xFF;
        message[pos++] = (int64_t)size >> 48 & 0xFF;
        message[pos++] = (int64_t)size >> 40 & 0xFF;
        message[pos++] = (int64_t)size >> 32 & 0xFF;
        message[pos++] = size >> 24 & 0xFF;
        message[pos++] = size >> 16 & 0xFF;
        message[pos++] = size >> 8 & 0xFF;
        message[pos++] = size & 0xFF;
    }

    if (mask) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 0xFF);
        unsigned char mask_key[4];
        for (unsigned char& k : mask_key) {
            message[pos++] = k = dis(gen);
        }

        for (int i = 0; i < size; ++i) {
            message[pos++] = ((const unsigned char*)payload)[i] ^ mask_key[i % 4];
        }
    } else {
        memcpy(message + pos, payload, size);
        pos += size;
    }

    output((char*)message, pos);

    delete[] message;

    return pos;
}

int unpack_frame(const char* message, int size, unpack_frame_output output) {
    if (!message || size < 2 || !output) {
        return 0;
    }

    const unsigned char* _message = (const unsigned char*)message;

    int pos = 0;
    int fin = _message[pos] & 0x80;
    ws_opcode opcode = (ws_opcode)(_message[pos++] & 0x0F);
    int mask = _message[pos] & 0x80;
    int payload_size = _message[pos++] & 0x7F;

    if (126 == payload_size) {
        if (size < pos + 2) {
            return 0;
        }
        payload_size = _message[pos++] << 8;
        payload_size |= _message[pos++];
    } else if (127 == payload_size) {
        if (size < pos + 8) {
            return 0;
        }
        payload_size = (int64_t)_message[pos++] << 56;
        payload_size |= (int64_t)_message[pos++] << 48;
        payload_size |= (int64_t)_message[pos++] << 40;
        payload_size |= (int64_t)_message[pos++] << 32;
        payload_size |= _message[pos++] << 24;
        payload_size |= _message[pos++] << 16;
        payload_size |= _message[pos++] << 8;
        payload_size |= _message[pos++];
    }

    unsigned char mask_key[4];
    if (mask) {
        if (size < pos + 4) {
            return 0;
        }
        memcpy(mask_key, _message + pos, 4);
        pos += 4;
    }

    if (size < pos + payload_size) {
        return 0;
    }

    if (!payload_size) {
        output(opcode, nullptr, 0);
        return pos;
    }

    if (mask) {
        unsigned char* payload = new unsigned char[payload_size];
        for (int i = 0; i < payload_size; ++i) {
            payload[i] = _message[pos++] ^ mask_key[i % 4];
        }
        output(opcode, (char*)payload, payload_size);
        delete[] payload;
    } else {
        output(opcode, message + pos, payload_size);
        pos += payload_size;
    }

    return pos;
}
