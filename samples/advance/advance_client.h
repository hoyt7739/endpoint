#ifndef ADVANCE_CLIENT_H
#define ADVANCE_CLIENT_H

#include "commun_client.h"

class advance_client : public commun_client::commun_notify {
public:
    advance_client();
    ~advance_client();

public:
    void start();
    void stop();
    void send_command();
    void req_interact();
    void rsp_interact();
    void req_state();
    void req_assign();
    void req_report();

private:
    virtual void on_started(const std::string& info) override;
    virtual void on_stopped() override;
    virtual void on_connected(const std::string& remote_info) override;
    virtual void on_disconnected() override;
    virtual void on_sent(data_frame::size_type size, commun_command command) override;
    virtual void on_received(data_frame::size_type size, commun_command command) override;
    virtual void on_req_interact(const commun_interact& interact) override;
    virtual void on_rsp_interact(const commun_interact& interact) override;
    virtual void on_rsp_state(commun_state state) override;
    virtual void on_rsp_assign(commun_answer answer) override;
    virtual void on_rsp_report(const std::string& name) override;

private:
    commun_client* m_client = nullptr;
    char* m_buffer = nullptr;
};

#endif
