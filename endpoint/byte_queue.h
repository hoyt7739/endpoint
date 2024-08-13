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
