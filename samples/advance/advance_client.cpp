#include "advance_client.h"
#include "common.h"

advance_client::advance_client() {
    m_buffer = new char[DATA_BUFFER_SIZE];
}

advance_client::~advance_client() {
    delete m_client;
    delete m_buffer;
}

void advance_client::start() {
    if (m_client) {
        return;
    }

    endpoint::endpoint_type type = (endpoint::endpoint_type)input_item("type", TYPE_ITEMS);
    std::string server_info = input_text("server info");
    std::cout << "start, type=" << TYPE_ITEMS.at(type) << ", server info=" << server_info << std::endl;
    m_client = new commun_client(this);
    m_client->set_recv_dir("received");
    m_client->start(type, server_info);
}

void advance_client::stop() {
    if (!m_client) {
        return;
    }

    std::cout << "stop" << std::endl;
    delete m_client;
    m_client = nullptr;
}

void advance_client::send_command() {
    if (!m_client) {
        return;
    }

    commun_command command = (commun_command)input_item("command", COMMAND_ITEMS);
    int size = input_data("data", m_buffer, DATA_BUFFER_SIZE);
    std::cout << "send_command, command=" << COMMAND_ITEMS.at(command) << ", size=" << size << std::endl;
    m_client->send_command(command, m_buffer, size);
}

void advance_client::req_interact() {
    if (!m_client) {
        return;
    }

    int size = input_file("interact", m_buffer, DATA_BUFFER_SIZE - 1);
    m_buffer[size] = '\0';
    std::cout << "req_interact, data=" << m_buffer << std::endl;
    m_client->req_interact(commun_interact(m_buffer));
}

void advance_client::rsp_interact() {
    if (!m_client) {
        return;
    }

    int size = input_file("interact", m_buffer, DATA_BUFFER_SIZE - 1);
    m_buffer[size] = '\0';
    std::cout << "rsp_interact, data=" << m_buffer << std::endl;
    m_client->rsp_interact(commun_interact(m_buffer));
}

void advance_client::req_state() {
    if (!m_client) {
        return;
    }

    std::cout << "req_state" << std::endl;
    m_client->req_state();
}

void advance_client::req_assign() {
    if (!m_client) {
        return;
    }

    std::string path = input_text("assign path");
    if (path.empty()) {
        return;
    }
    std::cout << "req_assign, path=" << path << std::endl;
    m_client->req_assign(path);
}

void advance_client::req_report() {
    if (!m_client) {
        return;
    }

    std::cout << "req_report" << std::endl;
    m_client->req_report();
}

void advance_client::on_started(const std::string& info) {
    std::cout << "on_started, info=" << info << std::endl;
}

void advance_client::on_stopped() {
    std::cout << "on_stopped" << std::endl;
}

void advance_client::on_connected(const std::string& remote_info) {
    std::cout << "on_connected, info=" << remote_info << std::endl;
}

void advance_client::on_disconnected() {
    std::cout << "on_disconnected" << std::endl;
}

void advance_client::on_sent(data_frame::size_type size, commun_command command) {
    std::cout << "on_sent, size=" << size << ", command=" << COMMAND_ITEMS.at(command) << std::endl;
}

void advance_client::on_received(data_frame::size_type size, commun_command command) {
    std::cout << "on_received, size=" << size << ", command=" << COMMAND_ITEMS.at(command) << std::endl;
}

void advance_client::on_req_interact(const commun_interact& interact) {
    std::cout << "on_req_interact, interact=" << interact.to_json() << std::endl;
}

void advance_client::on_rsp_interact(const commun_interact& interact) {
    std::cout << "on_rsp_interact, interact=" << interact.to_json() << std::endl;
}

void advance_client::on_rsp_state(commun_state state) {
    std::cout << "on_rsp_state, state=" << STATE_ITEMS.at(state) << std::endl;
}

void advance_client::on_rsp_assign(commun_answer answer) {
    std::cout << "on_rsp_assign, answer=" << ANSWER_ITEMS.at(answer) << std::endl;
}

void advance_client::on_rsp_report(const std::string& name) {
    std::cout << "on_rsp_report, name=" << name << std::endl;
}
