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

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "endpoint.h"

class bluetooth : public endpoint {
public:
    explicit bluetooth(const std::string& info = std::string());
    virtual ~bluetooth();

public:
    virtual bool listen(connected_notify notify) override;
    virtual bool connect(const std::string& remote_info, connected_notify notify) override;
    virtual bool is_listening() const override;
    virtual bool is_connected() const override;
    virtual void disconnect() override;
    virtual void close() override;
    virtual int send(const char* buffer, int size) override;
    virtual int recv(char* buffer, int size) override;

private:
    bool init_service();
    void uninit_service();

private:
    struct bluetooth_data* m_data;
};

#endif
