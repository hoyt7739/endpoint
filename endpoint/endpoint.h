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

#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <string>
#include <functional>

class endpoint {
public:
    enum endpoint_type {
        BLUETOOTH,
        TCP,
        WEBSOCKET
    };

    using connected_notify = std::function<void()>;

public:
    static endpoint* create(endpoint_type type, const std::string& info = std::string());
    static void destroy(endpoint* ep);

public:
    explicit endpoint(const std::string& info = std::string()) : m_info(info) {}
    endpoint(const endpoint&) = delete;
    endpoint(endpoint&&) = delete;
    virtual ~endpoint() = default;

    endpoint& operator=(const endpoint&) = delete;
    endpoint& operator=(endpoint&&) = delete;

public:
    virtual bool listen(connected_notify notify) = 0;
    virtual bool connect(const std::string& remote_info, connected_notify notify) = 0;
    virtual bool is_listening() const = 0;
    virtual bool is_connected() const = 0;
    virtual void disconnect() = 0;
    virtual void close() = 0;
    virtual int send(const char* buffer, int size) = 0;
    virtual int recv(char* buffer, int size) = 0;

public:
    const std::string& info() const { return m_info; }
    const std::string& remote_info() const { return m_remote_info; }
    bool full_send(const char* buffer, int size);
    bool full_recv(char* buffer, int size);

protected:
    std::string m_info;
    std::string m_remote_info;
};

#endif
