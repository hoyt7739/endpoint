#include "commun_base.h"
#include <fstream>

commun_base::commun_base(commun_notify* notify) : m_notify(notify) {
    m_endpoint.set_notify([this](commun_endpoint::endpoint_event event, void* info) {
        switch (event) {
        case commun_endpoint::STARTED:
            m_params.info = (char*)info;
            COMMUN_NOTIFY(on_started, m_params.info);
            break;
        case commun_endpoint::STOPPED:
            COMMUN_NOTIFY(on_stopped);
            break;
        case commun_endpoint::CONNECTED:
            m_send_future = std::async(std::launch::async, &commun_base::start_send, this);
            m_recv_future = std::async(std::launch::async, &commun_base::start_recv, this);
            if (0 < m_ping_interval) {
                m_ping_future = std::async(std::launch::async, &commun_base::start_ping, this);
            }
            m_params.remote_info = (char*)info;
            COMMUN_NOTIFY(on_connected, m_params.remote_info);
            break;
        case commun_endpoint::DISCONNECTED:
            stop_trans();
            COMMUN_NOTIFY(on_disconnected);
            break;
        case commun_endpoint::SENT:
        case commun_endpoint::RECEIVED: {
            data_frame::size_type* size_ptr = (data_frame::size_type*)&info;
            commun_command* command_ptr = (commun_command*)(size_ptr + 1);
            if (commun_endpoint::SENT == event) {
                COMMUN_NOTIFY(on_sent, *size_ptr, *command_ptr);
            } else {
                COMMUN_NOTIFY(on_received, *size_ptr, *command_ptr);
            }
            break;
        }
        }
    });
}

commun_base::~commun_base() {
    stop();
}

bool commun_base::start(const commun_endpoint::endpoint_params& params) {
    m_params = params;
    return m_endpoint.start(m_params);
}

bool commun_base::stop() {
    return m_endpoint.stop();
}

void commun_base::send_command(commun_command command) {
    sending_object object;
    object.type = sending_object::EMPTY;
    object.command = command;
    m_sending_queue.put(std::move(object));
    m_send_condition.notify_all();
}

void commun_base::send_command(commun_command command, const data_frame::byte_type* data, int size) {
    sending_object object;
    object.type = sending_object::BUFFER;
    object.command = command;
    object.bytes.put(data, size);
    m_sending_queue.put(std::move(object));
    m_send_condition.notify_all();
}

void commun_base::send_command(commun_command command, data_frame::byte_type byte) {
    send_command(command, &byte, sizeof(byte));
}

void commun_base::send_command(commun_command command, const std::string& path) {
    sending_object object;
    object.type = sending_object::PATH;
    object.command = command;
    object.bytes.put(path.data(), (int)path.size());
    m_sending_queue.put(std::move(object));
    m_send_condition.notify_all();
}

void commun_base::req_interact(const commun_interact& interact) {
    std::string json(interact.to_json());
    send_command(COMMAND_REQ_INTERACT, json.data(), (int)json.size());
}

void commun_base::rsp_interact(const commun_interact& interact) {
    std::string json(interact.to_json());
    send_command(COMMAND_RSP_INTERACT, json.data(), (int)json.size());
}

void commun_base::on_command(commun_command command, const data_frame::byte_type* data, data_frame::size_type size) {
    switch (command) {
    case COMMAND_PING:
        send_command(COMMAND_PONG);
        return;
    case COMMAND_REQ_FRAGMENT:
        on_req_fragment(data, size);
        return;
    case COMMAND_RSP_FRAGMENT:
        on_rsp_fragment((commun_answer)*data);
        return;
    case COMMAND_REQ_INTERACT:
        if (0 < size) {
            COMMUN_NOTIFY(on_req_interact, commun_interact(data));
        } else {
            prepare_recv_file(command);
        }
        return;
    case COMMAND_RSP_INTERACT:
        if (0 < size) {
            COMMUN_NOTIFY(on_rsp_interact, commun_interact(data));
        } else {
            prepare_recv_file(command);
        }
        return;
    }
}

void commun_base::on_file(commun_command command, const std::string& name, const data_frame::byte_type* data, int size) {
    switch (command) {
    case COMMAND_REQ_INTERACT:
        COMMUN_NOTIFY(on_req_interact, commun_interact(data));
        return;
    case COMMAND_RSP_INTERACT:
        COMMUN_NOTIFY(on_rsp_interact, commun_interact(data));
        return;
    }
}

bool commun_base::prepare_send_file(const std::string& path) {
    if (path.empty()) {
        return false;
    }

    std::ifstream file(path, std::ifstream::ate | std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    int size = (int)file.tellg();
    file.seekg(0);

    m_sending_file.reset();
    m_sending_file.put([&file](char* dest, int size) {
        file.read(dest, size);
        return (int)file.gcount();
    }, size);
    return true;
}

void commun_base::prepare_recv_file(commun_command command, const std::string& name) {
    m_receiving_command = command;
    m_receiving_name = name;
    m_receiving_file.reset();
    send_command(COMMAND_RSP_FRAGMENT, COMMUN_ANSWER_ACCEPT);
}

bool commun_base::save_file(const std::string& name, const data_frame::byte_type* data, int size) {
    if (name.empty()) {
        return false;
    }

    std::ofstream file(m_recv_dir.empty() ? name : m_recv_dir + "/" + name, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.write(data, size);
    return true;
}

void commun_base::start_send() {
    m_sending_queue.clear();
    m_busying = false;
    while (true) {
        std::unique_lock<std::mutex> lock(m_send_mutex);
        m_send_condition.wait_for(lock, std::chrono::seconds(m_send_timeout), [this] { return !m_busying; });

        sending_object object(m_sending_queue.take());

        if (!is_connected()) {
            break;
        }

        switch (object.type) {
        case sending_object::EMPTY:
            m_endpoint.send(object.command);
            break;
        case sending_object::BUFFER:
            if (data_frame::MAX_DATA_SIZE < object.bytes.size()) {
                m_sending_file = std::move(object.bytes);
                m_busying = true;
                m_endpoint.send(object.command);
            } else {
                object.bytes.take([this, &object](char* buffer, int size) {
                    m_endpoint.send(data_frame(object.command, buffer, (data_frame::size_type)size));
                    return size;
                }, false);
            }
            break;
        case sending_object::PATH:
            object.bytes.take([this, &object](char* buffer, int size) {
                std::string path(buffer, size);
                std::string name(path);
                size_t pos = path.find_last_of("/\\");
                if (std::string::npos != pos) {
                    name = path.substr(pos + 1);
                }
                if (prepare_send_file(path)) {
                    m_busying = true;
                    m_endpoint.send(data_frame(object.command, name.data(), (data_frame::size_type)name.size()));
                }
                return size;
            }, false);
            break;
        }
    }
}

void commun_base::start_recv() {
    while (true) {
        data_frame frame(m_endpoint.recv());
        if (!frame.size_valid() || !is_connected()) {
            break;
        }

        frame.set_tail(0);
        on_command((commun_command)frame.get_command(), frame.get_data(), frame.get_data_size());

        std::lock_guard<std::mutex> lock(m_ping_mutex);
        m_alive = true;
        m_discharge = true;
        m_ping_condition.notify_all();
    }
}

void commun_base::start_ping() {
    m_alive = true;
    m_discharge = false;
    m_ping_loss = 0;
    while (true) {
        std::unique_lock<std::mutex> lock(m_ping_mutex);
        m_ping_condition.wait_for(lock, std::chrono::seconds(m_ping_interval));

        if (!is_connected()) {
            break;
        }

        if (!m_alive && m_max_ping_loss <= ++m_ping_loss) {
            m_endpoint.disconnect();
        } else if (!m_discharge) {
            m_alive = false;
            send_command(COMMAND_PING);
        }

        m_discharge = false;
    }
}

void commun_base::stop_trans() {
    m_sending_queue.put(sending_object());
    m_busying = false;
    m_send_condition.notify_all();
    m_ping_condition.notify_all();
}

void commun_base::on_rsp_fragment(commun_answer answer) {
    if (COMMUN_ANSWER_REJECT == answer) {
        std::lock_guard<std::mutex> lock(m_send_mutex);
        m_sending_file.reset();
        m_busying = false;
        m_send_condition.notify_all();
        return;
    }

    data_frame frame(COMMAND_REQ_FRAGMENT);
    m_sending_file.take([&frame](char* buffer, int size) {
        return frame.set_buffer(COMMAND_REQ_FRAGMENT, buffer, std::min(size, (int)data_frame::MAX_DATA_SIZE));
    }, false);

    m_endpoint.send(std::move(frame));
    if (!frame.get_data_size()) {
        std::lock_guard<std::mutex> lock(m_send_mutex);
        m_busying = false;
        m_send_condition.notify_all();
    }
}

void commun_base::on_req_fragment(const data_frame::byte_type* fragment, data_frame::size_type size) {
    if (fragment && 0 < size) {
        m_receiving_file.put(fragment, size);
        send_command(COMMAND_RSP_FRAGMENT, COMMUN_ANSWER_ACCEPT);
    } else {
        m_receiving_file.take([this](char* buffer, int size) {
            on_file(m_receiving_command, m_receiving_name, buffer, size);
            return size;
        }, false);
    }
}
