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
