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
