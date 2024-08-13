#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "endpoint.h"
#include "ws_message.h"

class websocket : public endpoint {
public:
    explicit websocket(const std::string& info = "");
    virtual ~websocket();

public:
    virtual bool listen(connected_notify notify) override;
    virtual bool connect(const std::string& remote_info, connected_notify notify) override;
    virtual bool is_listening() const override;
    virtual bool is_connected() const override;
    virtual void disconnect() override;
    virtual void close() override;
    virtual int send(const char* buffer, int size) override;
    virtual int recv(char* buffer, int size) override;

private:
    void input_handler();
    int message_handler(const char* message, int size);

private:
    struct websocket_data* m_data;
};

#endif
