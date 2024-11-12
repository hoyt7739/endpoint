#ifndef ADVANCE_SERVER_H
#define ADVANCE_SERVER_H

#include "commun_server.h"

class advance_server : public commun_server::commun_notify {
public:
    advance_server();
    ~advance_server();

public:
    void start();
    void stop();
    void send_command();
    void req_interact();
    void rsp_interact();
    void rsp_state();
    void rsp_assign();
    void rsp_report();

private:
    virtual void on_started(const std::string& info) override;
    virtual void on_stopped() override;
    virtual void on_connected(const std::string& remote_info) override;
    virtual void on_disconnected() override;
    virtual void on_sent(data_frame::size_type size, commun_command command) override;
    virtual void on_received(data_frame::size_type size, commun_command command) override;
    virtual void on_req_interact(const commun_interact& interact) override;
    virtual void on_rsp_interact(const commun_interact& interact) override;
    virtual void on_req_state() override;
    virtual void on_req_assign(const std::string& name) override;
    virtual void on_req_report() override;

private:
    commun_server* m_server = nullptr;
    char* m_buffer = nullptr;
};

#endif
