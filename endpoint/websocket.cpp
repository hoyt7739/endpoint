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

#include "websocket.h"
#include <future>
#include <random>
#include <algorithm>
#include "base64/base64.h"
#include "tcp.h"
#include "byte_queue.h"

using namespace std::placeholders;

const int RAW_KEY_SIZE = 16;
const int INPUT_BUFFER_SIZE = 1024 * 6;

struct websocket_data {
    tcp* atcp = nullptr;
    bool handshaked = false;
    endpoint::connected_notify on_connected;
    std::string key;
    byte_queue message_queue;
    byte_queue payload_queue;
    std::future<void> input_future;
    std::future<void> disconnect_future;
};

websocket::websocket(const std::string& info) : endpoint(info), m_data(new websocket_data) {
    m_data->atcp = new tcp(info);
}

websocket::~websocket() {
    close();
    delete m_data->atcp;
    delete m_data;
}

bool websocket::listen(connected_notify notify) {
    m_data->on_connected = notify;

    bool res = m_data->atcp->listen([this] {
        m_remote_info = m_data->atcp->remote_info();
        m_data->input_future = std::async(std::launch::async, &websocket::input_handler, this);
    });

    m_info = m_data->atcp->info();
    return res;
}

bool websocket::connect(const std::string& remote_info, connected_notify notify) {
    m_data->on_connected = notify;

    bool res = m_data->atcp->connect(remote_info, [this, remote_info] {
        m_data->input_future = std::async(std::launch::async, &websocket::input_handler, this);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 0xFF);
        char raw_key[RAW_KEY_SIZE];
        std::generate(raw_key, raw_key + RAW_KEY_SIZE, [&dis, &gen] { return dis(gen); });
        m_data->key = base64_encode(raw_key, RAW_KEY_SIZE);
        pack_handshake(remote_info, m_data->key, std::bind(&tcp::send, m_data->atcp, _1, _2));
    });

    m_info = m_data->atcp->info();
    m_remote_info = m_data->atcp->remote_info();
    return res;
}

bool websocket::is_listening() const {
    return m_data->atcp->is_listening();
}

bool websocket::is_connected() const {
    return m_data->atcp->is_connected() && m_data->handshaked;
}

void websocket::disconnect() {
    m_data->disconnect_future = std::async(std::launch::async, [this] {
        if (is_connected()) {
            pack_frame(WS_OPCODE_CLOSE, !is_listening(), nullptr, 0, std::bind(&tcp::send, m_data->atcp, _1, _2));
        }

        m_data->atcp->disconnect();
        m_data->handshaked = false;
        m_data->message_queue.exit();
        m_data->payload_queue.exit();
    });
}

void websocket::close() {
    disconnect();
    m_data->atcp->close();
}

int websocket::send(const char* buffer, int size) {
    if (!is_connected()) {
        return 0;
    }

    if (pack_frame(WS_OPCODE_BINARY, !is_listening(), buffer, size, std::bind(&tcp::send, m_data->atcp, _1, _2)) <= 0) {
        return 0;
    }

    return size;
}

int websocket::recv(char* buffer, int size) {
    if (!is_connected()) {
        return 0;
    }

    return m_data->payload_queue.take(buffer, size);
}

void websocket::input_handler() {
    m_data->message_queue.reset();
    m_data->payload_queue.reset();

    char* buffer = new char[INPUT_BUFFER_SIZE];
    int size = 0;
    while (true) {
        size = m_data->atcp->recv(buffer, INPUT_BUFFER_SIZE);
        if (size <= 0) {
            disconnect();
            break;
        }
        m_data->message_queue.put(buffer, size);
        m_data->message_queue.take(std::bind(&websocket::message_handler, this, _1, _2), false);
    }
    delete[] buffer;
}

int websocket::message_handler(const char* message, int size) {
    if (m_data->handshaked) {
        return unpack_frame(message, size, [this](ws_opcode opcode, const char* payload, int payload_size) {
            if (WS_OPCODE_CLOSE == opcode) {
                disconnect();
            } else if (WS_OPCODE_PING == opcode) {
                pack_frame(WS_OPCODE_PONG, !is_listening(), nullptr, 0, std::bind(&tcp::send, m_data->atcp, _1, _2));
            } else {
                m_data->payload_queue.put(payload, payload_size);
            }
        });
    }

    if (m_data->atcp->is_listening()) {
        return unpack_handshake(message, size, [this](bool accept, const std::string& key) {
            if (accept) {
                pack_rhandshake(key, std::bind(&tcp::send, m_data->atcp, _1, _2));
                m_data->handshaked = true;
                if (m_data->on_connected) {
                    m_data->on_connected();
                }
            } else {
                disconnect();
            }
        });
    }

    return unpack_rhandshake(message, size, m_data->key, [this](bool accept) {
        if (accept) {
            m_data->handshaked = true;
            if (m_data->on_connected) {
                m_data->on_connected();
            }
        } else {
            disconnect();
        }
    });
}
