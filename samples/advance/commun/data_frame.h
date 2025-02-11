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

#ifndef DATA_FRAME_H
#define DATA_FRAME_H

#include <cstring>
#include <utility>

class data_frame {
public:
    using byte_type = char;
    using size_type = short;
    using command_type = short;

    static const size_type BUFFER_SIZE = 4096;
    static const size_type FIXED_PART_SIZE = sizeof(size_type) + sizeof(command_type) + sizeof(byte_type);
    static const size_type MAX_DATA_SIZE = BUFFER_SIZE - FIXED_PART_SIZE;

public:
    data_frame();
    data_frame(const data_frame& other);
    data_frame(data_frame&& other) noexcept;
    data_frame(command_type command, const byte_type* data = nullptr, size_type data_size = 0);
    ~data_frame();

    data_frame& operator=(const data_frame& other);
    data_frame& operator=(data_frame&& other) noexcept;

public:
    byte_type* get_buffer();
    size_type set_buffer(command_type command, const byte_type* data, size_type data_size);

    size_type get_size() const;
    void set_size(size_type size);
    bool size_valid() const;

    command_type get_command() const;
    void set_command(command_type command);

    const byte_type* get_data() const;
    size_type set_data(const byte_type* data, size_type data_size);
    size_type get_data_size() const;

    byte_type get_tail() const;
    void set_tail(byte_type tail);

    byte_type checksum() const;
    bool checksum_valid() const;

private:
    byte_type* m_buffer = nullptr;
};

inline data_frame::data_frame() {
    m_buffer = new byte_type[BUFFER_SIZE];
    memset(m_buffer, 0, BUFFER_SIZE);
}

inline data_frame::data_frame(const data_frame& other) {
    m_buffer = new byte_type[BUFFER_SIZE];
    memcpy(m_buffer, other.m_buffer, BUFFER_SIZE);
}

inline data_frame::data_frame(data_frame&& other) noexcept {
    m_buffer = other.m_buffer;
    other.m_buffer = nullptr;
}

inline data_frame::data_frame(command_type command, const byte_type* data, size_type data_size) {
    m_buffer = new byte_type[BUFFER_SIZE];
    set_buffer(command, data, data_size);
}

inline data_frame::~data_frame() {
    delete[] m_buffer;
}

inline data_frame& data_frame::operator=(const data_frame& other) {
    if (this != &other) {
        memcpy(m_buffer, other.m_buffer, BUFFER_SIZE);
    }

    return *this;
}

inline data_frame& data_frame::operator=(data_frame&& other) noexcept {
    if (this != &other) {
        std::swap(m_buffer, other.m_buffer);
    }

    return *this;
}

inline data_frame::byte_type* data_frame::get_buffer() {
    return m_buffer;
}

inline data_frame::size_type data_frame::set_buffer(command_type command, const byte_type* data, size_type data_size) {
    set_command(command);
    data_size = set_data(data, data_size);
    set_size(FIXED_PART_SIZE + data_size);
    set_tail(checksum());
    return data_size;
}

inline data_frame::size_type data_frame::get_size() const {
    size_type size = 0;
    memcpy(&size, m_buffer, sizeof(size_type));
    return size;
}

inline void data_frame::set_size(size_type size) {
    memcpy(m_buffer, &size, sizeof(size_type));
}

inline bool data_frame::size_valid() const {
    size_type size = get_size();
    return FIXED_PART_SIZE <= size && size <= BUFFER_SIZE;
}

inline data_frame::command_type data_frame::get_command() const {
    command_type command = 0;
    memcpy(&command, m_buffer + sizeof(size_type), sizeof(command_type));
    return command;
}

inline void data_frame::set_command(command_type command) {
    memcpy(m_buffer + sizeof(size_type), &command, sizeof(command_type));
}

inline const data_frame::byte_type* data_frame::get_data() const {
    return m_buffer + sizeof(size_type) + sizeof(command_type);
}

inline data_frame::size_type data_frame::set_data(const byte_type* data, size_type data_size) {
    if (MAX_DATA_SIZE < data_size) {
        data_size = MAX_DATA_SIZE;
    }

    if (!data) {
        data_size = 0;
    }

    if (0 < data_size) {
        memcpy(m_buffer + sizeof(size_type) + sizeof(command_type), data, data_size);
    }

    return data_size;
}

inline data_frame::size_type data_frame::get_data_size() const {
    return get_size() - FIXED_PART_SIZE;
}

inline data_frame::byte_type data_frame::get_tail() const {
    return m_buffer[get_size() - sizeof(byte_type)];
}

inline void data_frame::set_tail(byte_type tail) {
    m_buffer[get_size() - sizeof(byte_type)] = tail;
}

inline data_frame::byte_type data_frame::checksum() const {
    byte_type value = 0;
    size_type count = get_size() - sizeof(byte_type);
    for (size_type index = 0; index < count; ++index) {
        value += m_buffer[index];
    }

    return (~value + 1) & 0xFF;
}

inline bool data_frame::checksum_valid() const {
    return get_tail() == checksum();
}

#endif
