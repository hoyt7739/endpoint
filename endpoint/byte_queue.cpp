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

#include "byte_queue.h"
#include <cstring>

const int DEFAULT_BUFFER_SIZE = 64;

byte_queue::byte_queue()
    : m_buffer_size(DEFAULT_BUFFER_SIZE),
      m_begin(0),
      m_end(0),
      m_exiting(false) {
    m_buffer = new char[m_buffer_size + 1];
    m_buffer[m_end] = 0;
}

byte_queue::byte_queue(const byte_queue& other)
    : m_buffer_size(other.size()),
      m_begin(0),
      m_end(other.size()),
      m_exiting(false) {
    m_buffer = new char[m_buffer_size + 1];
    memcpy(m_buffer, other.m_buffer + other.m_begin, other.size());
}

byte_queue::byte_queue(byte_queue&& other) noexcept
    : m_buffer(other.m_buffer),
      m_buffer_size(other.m_buffer_size),
      m_begin(other.m_begin),
      m_end(other.m_end),
      m_exiting(false) {
    other.m_buffer = nullptr;
    other.m_buffer_size = other.m_begin = other.m_end = 0;
    other.m_exiting = false;
}

byte_queue::~byte_queue() {
    delete[] m_buffer;
}

byte_queue& byte_queue::operator=(const byte_queue& other) {
    if (this != &other) {
        if (m_buffer_size < other.size()) {
            delete[] m_buffer;
            m_buffer_size = other.size();
            m_buffer = new char[m_buffer_size + 1];
        }
        m_begin = 0;
        m_end = other.size();
        memcpy(m_buffer, other.m_buffer + other.m_begin, other.size());
    }

    return *this;
}

byte_queue& byte_queue::operator=(byte_queue&& other) noexcept {
    if (this != &other) {
        std::swap(m_buffer, other.m_buffer);
        std::swap(m_buffer_size, other.m_buffer_size);
        std::swap(m_begin, other.m_begin);
        std::swap(m_end, other.m_end);
    }

    return *this;
}

int byte_queue::put(bytes_handler handler, int size) {
    if (!handler || size <= 0) {
        return 0;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_exiting) {
        return 0;
    }

    int need_size = m_end - m_begin + size;
    if (m_buffer_size < need_size) {
        m_buffer_size = need_size + need_size / 2;
        char* new_buffer = new char[m_buffer_size + 1];
        if (m_begin < m_end) {
            memcpy(new_buffer, m_buffer + m_begin, m_end - m_begin);
            m_end -= m_begin;
            m_begin = 0;
        }
        delete[] m_buffer;
        m_buffer = new_buffer;
    }

    if (m_begin && m_begin == m_end) {
        m_begin = m_end = 0;
    }

    if (m_buffer_size - m_end < size) {
        memmove(m_buffer, m_buffer + m_begin, m_end - m_begin);
        m_end -= m_begin;
        m_begin = 0;
    }

    size = handler(m_buffer + m_end, m_buffer_size - m_end);
    if (0 < size) {
        m_end += size;
    }

    m_buffer[m_end] = 0;
    m_cond_has_bytes.notify_all();

    return size;
}

int byte_queue::put(const char* src, int size) {
    if (!src || size <= 0) {
        return 0;
    }

    bytes_handler handler = [src, size](char* dest, int dest_size) {
        if (dest_size < size) {
            return 0;
        }
        memcpy(dest, src, size);
        return size;
    };

    return put(handler, size);
}

int byte_queue::take(bytes_handler handler, bool wait) {
    if (!handler) {
        return 0;
    }

    auto condition = [this] { return m_begin < m_end; };
    std::unique_lock<std::mutex> lock(m_mutex);

    if (wait) {
        m_cond_has_bytes.wait(lock, [this, condition] { return m_exiting || condition(); });
    } else if (!condition()) {
        return 0;
    }

    if (m_exiting) {
        return 0;
    }

    int size = handler(m_buffer + m_begin, m_end - m_begin);
    if (0 < size) {
        m_begin += size;
    }

    return size;
}

int byte_queue::take(char* dest, int size, bool wait) {
    if (!dest || size <= 0) {
        return 0;
    }

    bytes_handler handler = [dest, &size](char* src, int src_size) {
        if (src_size < size) {
            size = src_size;
        }
        memcpy(dest, src, size);
        return size;
    };

    return take(handler, wait);
}

int byte_queue::size() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_end - m_begin;
}

void byte_queue::exit() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_exiting = true;
    m_cond_has_bytes.notify_all();
}

void byte_queue::reset() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_begin = m_end = 0;
    m_buffer[m_end] = 0;
    m_exiting = false;
}
