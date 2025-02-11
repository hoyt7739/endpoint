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
