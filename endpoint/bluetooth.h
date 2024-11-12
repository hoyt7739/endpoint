#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "endpoint.h"

class bluetooth : public endpoint {
public:
    explicit bluetooth(const std::string& info = std::string());
    virtual ~bluetooth();

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
    bool init_service();
    void uninit_service();

private:
    struct bluetooth_data* m_data;
};

#endif
