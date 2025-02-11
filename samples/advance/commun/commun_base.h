/*
  MIT License

  Copyright (c) 2025 Kong Pengsheng

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef COMMUN_BASE_H
#define COMMUN_BASE_H

#include "byte_queue.h"
#include "commun_command.h"
#include "commun_endpoint.h"

class commun_base {
public:
    static const int DEFAULT_SEND_TIMEOUT   = 60;
    static const int DEFAULT_PING_INTERVAL  = 60;
    static const int DEFAULT_MAX_PING_LOSS  = 3;

    struct commun_notify {
        virtual void on_started(const std::string& info) {}
        virtual void on_stopped() {}
        virtual void on_connected(const std::string& remote_info) {}
        virtual void on_disconnected() {}
        virtual void on_sent(data_frame::size_type size, commun_command command) {}
        virtual void on_received(data_frame::size_type size, commun_command command) {}
        virtual void on_req_interact(const commun_interact& interact) {}
        virtual void on_rsp_interact(const commun_interact& interact) {}
    };

public:
    explicit commun_base(commun_notify* notify = nullptr);
    virtual ~commun_base();

public:
    void set_send_timeout(int send_timeout) { m_send_timeout = send_timeout; }
    void set_ping_interval(int ping_interval) { m_ping_interval = ping_interval; }
    void set_max_ping_loss(int max_ping_loss) { m_max_ping_loss = max_ping_loss; }
    void set_recv_dir(const std::string& recv_dir) { m_recv_dir = recv_dir; }

    bool start(const commun_endpoint::endpoint_params& params);
    bool stop();

    const commun_endpoint::endpoint_params& endpoint_params() const { return m_params; }
    bool is_connected() const { return m_endpoint.is_connected(); }

    void send_command(commun_command command);
    void send_command(commun_command command, const data_frame::byte_type* data, int size);
    void send_command(commun_command command, data_frame::byte_type byte);
    void send_command(commun_command command, const std::string& path);
    void req_interact(const commun_interact& interact);
    void rsp_interact(const commun_interact& interact);

protected:
    virtual void on_command(commun_command command, const data_frame::byte_type* data, data_frame::size_type size);
    virtual void on_file(commun_command command, const std::string& name, const data_frame::byte_type* data, int size);

protected:
    bool prepare_send_file(const std::string& path);
    void prepare_recv_file(commun_command command, const std::string& name = std::string());
    bool save_file(const std::string& name, const data_frame::byte_type* data, int size);

private:
    void start_send();
    void start_recv();
    void start_ping();
    void stop_trans();
    void on_rsp_fragment(commun_answer answer);
    void on_req_fragment(const data_frame::byte_type* fragment, data_frame::size_type size);

protected:
    commun_notify* m_notify = nullptr;

private:
    struct sending_object {
        enum {
            INVALID,
            EMPTY,
            BUFFER,
            PATH
        } type = INVALID;
        commun_command command;
        byte_queue bytes;

        sending_object() = default;
        sending_object(const sending_object& other)
            : type(other.type), command(other.command), bytes(other.bytes) {}
        sending_object(sending_object&& other)
            : type(other.type), command(other.command), bytes(std::move(other.bytes)) {}
    };

private:
    commun_endpoint::endpoint_params m_params;
    commun_endpoint m_endpoint;

    std::future<void> m_send_future;
    std::mutex m_send_mutex;
    std::condition_variable m_send_condition;
    int m_send_timeout = DEFAULT_SEND_TIMEOUT;
    bool m_busying = false;
    block_queue<sending_object> m_sending_queue;
    byte_queue m_sending_file;

    std::future<void> m_recv_future;
    std::string m_recv_dir;
    commun_command m_receiving_command;
    std::string m_receiving_name;
    byte_queue m_receiving_file;

    std::future<void> m_ping_future;
    std::mutex m_ping_mutex;
    std::condition_variable m_ping_condition;
    int m_ping_interval = DEFAULT_PING_INTERVAL;
    int m_max_ping_loss = DEFAULT_MAX_PING_LOSS;
    bool m_alive = false;
    bool m_discharge = false;
    int m_ping_loss = 0;
};

#define COMMUN_NOTIFY(method, ...)                                      \
    do {                                                                \
        commun_notify* notify = dynamic_cast<commun_notify*>(m_notify); \
        if (notify) {                                                   \
            notify->method(__VA_ARGS__);                                \
        }                                                               \
    } while (false)

#endif
