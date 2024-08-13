#ifndef TCP_H
#define TCP_H

#include "endpoint.h"

class tcp : public endpoint {
public:
    explicit tcp(const std::string& info = "");
    virtual ~tcp();

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
    void broadcast();

private:
    struct tcp_data* m_data;
};

#endif
