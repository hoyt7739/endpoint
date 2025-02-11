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

#include "commun_client.h"

bool commun_client::start(endpoint::endpoint_type type, const std::string& server_info) {
    return commun_base::start({type, commun_endpoint::CLIENT, std::string(), server_info});
}

void commun_client::req_state() {
    send_command(COMMAND_REQ_STATE);
}

void commun_client::req_assign(const std::string& path) {
    send_command(COMMAND_REQ_ASSIGN, path);
}

void commun_client::req_report() {
    send_command(COMMAND_REQ_REPORT);
}

void commun_client::on_command(commun_command command, const data_frame::byte_type* data, data_frame::size_type size) {
    switch (command) {
    case COMMAND_RSP_STATE:
        COMMUN_NOTIFY(on_rsp_state, (commun_state)*data);
        return;
    case COMMAND_RSP_ASSIGN:
        COMMUN_NOTIFY(on_rsp_assign, (commun_answer)*data);
        return;
    case COMMAND_RSP_REPORT:
        prepare_recv_file(command, data);
        return;
    }

    commun_base::on_command(command, data, size);
}

void commun_client::on_file(commun_command command, const std::string& name, const data_frame::byte_type* data, int size) {
    switch (command) {
    case COMMAND_RSP_REPORT:
        save_file(name, data, size);
        COMMUN_NOTIFY(on_rsp_report, name);
        return;
    }

    commun_base::on_file(command, name, data, size);
}
