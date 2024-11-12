#ifndef COMMUN_ENDPOINT_H
#define COMMUN_ENDPOINT_H

#include <future>
#include "block_queue.h"
#include "data_frame.h"
#include "endpoint.h"

class commun_endpoint {
public:
    enum endpoint_role {
        SERVER,
        CLIENT
    };

    enum endpoint_event {
        STARTED,
        STOPPED,
        CONNECTED,
        DISCONNECTED,
        SENT,
        RECEIVED
    };

    using endpoint_notify = std::function<void(endpoint_event event, void* info)>;

    struct endpoint_params {
        endpoint::endpoint_type type;
        endpoint_role role;
        std::string info;
        std::string remote_info;
    };

public:
    commun_endpoint();
    commun_endpoint(const commun_endpoint&) = delete;
    commun_endpoint(commun_endpoint&&) = delete;
    ~commun_endpoint();

    commun_endpoint& operator=(const commun_endpoint&) = delete;
    commun_endpoint& operator=(commun_endpoint&&) = delete;

public:
    void set_notify(endpoint_notify notify) { m_notify = notify; }
    bool is_connected() const { return m_endpoint && m_endpoint->is_connected(); }
    bool start(const endpoint_params& params);
    bool stop();
    void disconnect();
    void send(const data_frame& frame);
    void send(data_frame&& frame);
    data_frame recv();

private:
    void on_connected();

private:
    endpoint_notify m_notify = nullptr;
    endpoint* m_endpoint = nullptr;
    std::future<void> m_send_future, m_recv_future;
    block_queue<data_frame> m_send_queue, m_recv_queue;
};

#endif
