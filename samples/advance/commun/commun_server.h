#ifndef COMMUN_SERVER_H
#define COMMUN_SERVER_H

#include "commun_base.h"

class commun_server : public commun_base {
public:
    struct commun_notify : commun_base::commun_notify {
        virtual void on_req_state() {}
        virtual void on_req_assign(const std::string& name) {}
        virtual void on_req_report() {}
    };

public:
    using commun_base::commun_base;

public:
    bool start(endpoint::endpoint_type type, const std::string& server_info = std::string());

    void rsp_state(commun_state state);
    void rsp_assign(commun_answer answer);
    void rsp_report(const std::string& path);

protected:
    virtual void on_command(commun_command command, const data_frame::byte_type* data, data_frame::size_type size) override;
    virtual void on_file(commun_command command, const std::string& name, const data_frame::byte_type* data, int size) override;
};

#endif
