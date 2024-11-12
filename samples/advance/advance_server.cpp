#include "advance_server.h"
#include "common.h"

advance_server::advance_server() {
    m_buffer = new char[DATA_BUFFER_SIZE];
}

advance_server::~advance_server() {
    delete m_server;
    delete m_buffer;
}

void advance_server::start() {
    if (m_server) {
        return;
    }

    endpoint::endpoint_type type = (endpoint::endpoint_type)input_item("type", TYPE_ITEMS);
    std::string server_info = input_text("server info");
    std::cout << "start, type=" << TYPE_ITEMS.at(type) << ", server info=" << server_info << std::endl;
    m_server = new commun_server(this);
    m_server->set_ping_interval(0);
    m_server->set_recv_dir("received");
    m_server->start(type, server_info);
}

void advance_server::stop() {
    if (!m_server) {
        return;
    }

    std::cout << "stop" << std::endl;
    delete m_server;
    m_server = nullptr;
}

void advance_server::send_command() {
    if (!m_server) {
        return;
    }

    commun_command command = (commun_command)input_item("command", COMMAND_ITEMS);
    int size = input_data("data", m_buffer, DATA_BUFFER_SIZE);
    std::cout << "send_command, command=" << COMMAND_ITEMS.at(command) << ", size=" << size << std::endl;
    m_server->send_command(command, m_buffer, size);
}

void advance_server::req_interact() {
    if (!m_server) {
        return;
    }

    int size = input_file("interact", m_buffer, DATA_BUFFER_SIZE - 1);
    m_buffer[size] = '\0';
    std::cout << "req_interact, data=" << m_buffer << std::endl;
    m_server->req_interact(commun_interact(m_buffer));
}

void advance_server::rsp_interact() {
    if (!m_server) {
        return;
    }

    int size = input_file("interact", m_buffer, DATA_BUFFER_SIZE - 1);
    m_buffer[size] = '\0';
    std::cout << "rsp_interact, data=" << m_buffer << std::endl;
    m_server->rsp_interact(commun_interact(m_buffer));
}

void advance_server::rsp_state() {
    if (!m_server) {
        return;
    }

    commun_state state = (commun_state)input_item("state", STATE_ITEMS);
    std::cout << "rsp_state, state=" << STATE_ITEMS.at(state) << std::endl;
    m_server->rsp_state(state);
}

void advance_server::rsp_assign() {
    if (!m_server) {
        return;
    }

    commun_answer answer = (commun_answer)input_item("answer", ANSWER_ITEMS);
    std::cout << "rsp_assign, answer=" << ANSWER_ITEMS.at(answer) << std::endl;
    m_server->rsp_assign(answer);
}

void advance_server::rsp_report() {
    if (!m_server) {
        return;
    }

    std::string path = input_text("report path");
    if (path.empty()) {
        return;
    }
    std::cout << "rsp_report, path=" << path << std::endl;
    m_server->rsp_report(path);
}

void advance_server::on_started(const std::string& info) {
    std::cout << "on_started, info=" << info << std::endl;
}

void advance_server::on_stopped() {
    std::cout << "on_stopped" << std::endl;
}

void advance_server::on_connected(const std::string& remote_info) {
    std::cout << "on_connected, info=" << remote_info << std::endl;
}

void advance_server::on_disconnected() {
    std::cout << "on_disconnected" << std::endl;
}

void advance_server::on_sent(data_frame::size_type size, commun_command command) {
    std::cout << "on_sent, size=" << size << ", command=" << COMMAND_ITEMS.at(command) << std::endl;
}

void advance_server::on_received(data_frame::size_type size, commun_command command) {
    std::cout << "on_received, size=" << size << ", command=" << COMMAND_ITEMS.at(command) << std::endl;
}

void advance_server::on_req_interact(const commun_interact& interact) {
    std::cout << "on_req_interact, interact=" << interact.to_json() << std::endl;
}

void advance_server::on_rsp_interact(const commun_interact& interact) {
    std::cout << "on_rsp_interact, interact=" << interact.to_json() << std::endl;
}

void advance_server::on_req_state() {
    std::cout << "on_req_state" << std::endl;
}

void advance_server::on_req_assign(const std::string& name) {
    std::cout << "on_req_assign, name=" << name << std::endl;
}

void advance_server::on_req_report() {
    std::cout << "on_req_report" << std::endl;
}
