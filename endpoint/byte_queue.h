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

#ifndef BYTE_QUEUE_H
#define BYTE_QUEUE_H

#include <functional>
#include <mutex>
#include <condition_variable>

using bytes_handler = std::function<int(char* buffer, int size)>;

class byte_queue {
public:
    byte_queue();
    byte_queue(const byte_queue& other);
    byte_queue(byte_queue&& other) noexcept;
    ~byte_queue();

    byte_queue& operator=(const byte_queue& other);
    byte_queue& operator=(byte_queue&& other) noexcept;

public:
    int put(bytes_handler handler, int size);
    int put(const char* src, int size);
    int take(bytes_handler handler, bool wait = true);
    int take(char* dest, int size, bool wait = true);
    int size() const;
    void exit();
    void reset();

private:
    char* m_buffer;
    int m_buffer_size, m_begin, m_end;
    bool m_exiting;
    mutable std::mutex m_mutex;
    std::condition_variable m_cond_has_bytes;
};

#endif
