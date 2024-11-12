#ifndef COMMUN_CLIENT_H
#define COMMUN_CLIENT_H

#include "commun_base.h"

class commun_client : public commun_base {
public:
    struct commun_notify : commun_base::commun_notify {
        virtual void on_rsp_state(commun_state state) {}
        virtual void on_rsp_assign(commun_answer answer) {}
        virtual void on_rsp_report(const std::string& name) {}
    };

public:
    using commun_base::commun_base;

public:
    bool start(endpoint::endpoint_type type, const std::string& server_info);

    void req_state();
    void req_assign(const std::string& path);
    void req_report();

protected:
    virtual void on_command(commun_command command, const data_frame::byte_type* data, data_frame::size_type size) override;
    virtual void on_file(commun_command command, const std::string& name, const data_frame::byte_type* data, int size) override;
};

#endif
