#include "commun_server.h"

bool commun_server::start(endpoint::endpoint_type type, const std::string& server_info) {
    return commun_base::start({type, commun_endpoint::SERVER, server_info, std::string()});
}

void commun_server::rsp_state(commun_state state) {
    send_command(COMMAND_RSP_STATE, state);
}

void commun_server::rsp_assign(commun_answer answer) {
    send_command(COMMAND_RSP_ASSIGN, answer);
}

void commun_server::rsp_report(const std::string& path) {
    send_command(COMMAND_RSP_REPORT, path);
}

void commun_server::on_command(commun_command command, const data_frame::byte_type* data, data_frame::size_type size) {
    switch (command) {
    case COMMAND_REQ_STATE:
        COMMUN_NOTIFY(on_req_state);
        return;
    case COMMAND_REQ_ASSIGN:
        prepare_recv_file(command, data);
        return;
    case COMMAND_REQ_REPORT:
        COMMUN_NOTIFY(on_req_report);
        return;
    }

    commun_base::on_command(command, data, size);
}

void commun_server::on_file(commun_command command, const std::string& name, const data_frame::byte_type* data, int size) {
    switch (command) {
    case COMMAND_REQ_ASSIGN:
        save_file(name, data, size);
        COMMUN_NOTIFY(on_req_assign, name);
        return;
    }

    commun_base::on_file(command, name, data, size);
}
