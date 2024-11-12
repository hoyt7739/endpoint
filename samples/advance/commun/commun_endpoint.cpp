#include "commun_endpoint.h"

commun_endpoint::commun_endpoint() {
}

commun_endpoint::~commun_endpoint() {
    stop();
}

bool commun_endpoint::start(const endpoint_params& params) {
    if (m_endpoint) {
        return false;
    }

    m_endpoint = endpoint::create(params.type, params.info);
    if (!m_endpoint) {
        return false;
    }

    endpoint::connected_notify notify = std::bind(&commun_endpoint::on_connected, this);
    bool res = false;
    switch (params.role) {
    case SERVER:
        res = m_endpoint->listen(notify);
        break;
    case CLIENT:
        res = m_endpoint->connect(params.remote_info, notify);
        break;
    }

    if (!res) {
        stop();
        return false;
    }

    if (m_notify) {
        m_notify(STARTED, (void*)m_endpoint->info().data());
    }

    return true;
}

bool commun_endpoint::stop() {
    if (!m_endpoint) {
        return true;
    }

    m_endpoint->close();
    endpoint::destroy(m_endpoint);
    m_endpoint = nullptr;

    if (m_notify) {
        m_notify(STOPPED, nullptr);
    }

    return true;
}

void commun_endpoint::disconnect() {
    if (m_endpoint) {
        m_endpoint->disconnect();
    }
}

void commun_endpoint::send(const data_frame& frame) {
    m_send_queue.put(frame);
}

void commun_endpoint::send(data_frame&& frame) {
    m_send_queue.put(frame);
}

data_frame commun_endpoint::recv() {
    return m_recv_queue.take();
}

void commun_endpoint::on_connected() {
    m_send_future = std::async(std::launch::async, [this] {
        m_send_queue.clear();
        while (true) {
            data_frame frame(m_send_queue.take());
            if (!frame.size_valid()) {
                break;
            }

            if (!m_endpoint || !m_endpoint->full_send(frame.get_buffer(), frame.get_size())) {
                break;
            }

            if (m_notify) {
                void* info = nullptr;
                memcpy(&info, frame.get_buffer(), sizeof(data_frame::size_type) + sizeof(data_frame::command_type));
                m_notify(SENT, info);
            }
        }
    });

    m_recv_future = std::async(std::launch::async, [this] {
        m_recv_queue.clear();
        while (true) {
            data_frame frame;
            const int head_size = sizeof(data_frame::size_type);
            if (!m_endpoint || !m_endpoint->full_recv(frame.get_buffer(), head_size)) {
                break;
            }

            if (!frame.size_valid()) {
                continue;
            }

            if (!m_endpoint || !m_endpoint->full_recv(frame.get_buffer() + head_size, frame.get_size() - head_size)) {
                break;
            }

            if (!frame.checksum_valid()) {
                continue;
            }

            void* info = nullptr;
            memcpy(&info, frame.get_buffer(), head_size + sizeof(data_frame::command_type));

            m_recv_queue.put(std::move(frame));

            if (m_notify) {
                m_notify(RECEIVED, info);
            }
        }

        m_send_queue.put(data_frame());
        m_recv_queue.put(data_frame());

        if (m_notify) {
            m_notify(DISCONNECTED, nullptr);
        }
    });

    if (m_notify) {
        m_notify(CONNECTED, (void*)m_endpoint->remote_info().data());
    }
}
