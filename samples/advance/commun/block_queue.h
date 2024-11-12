#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>

template<class type>
class block_queue {
public:
    block_queue() = default;
    block_queue(const block_queue&) = delete;
    block_queue& operator=(const block_queue&) = delete;

public:
    template<class... arguments>
    void put(arguments&&... args);
    void put(const type& obj);
    void put(type&& obj);
    type take();

    size_t size();
    void clear();

private:
    std::deque<type> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond_has;
};

template<class type>
template<class... arguments>
inline void block_queue<type>::put(arguments&&... args) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.emplace_back(std::forward<arguments>(args)...);
    m_cond_has.notify_all();
}

template<class type>
inline void block_queue<type>::put(const type& obj) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push_back(obj);
    m_cond_has.notify_all();
}

template<class type>
inline void block_queue<type>::put(type&& obj) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push_back(obj);
    m_cond_has.notify_all();
}

template<class type>
inline type block_queue<type>::take() {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cond_has.wait(lock, [this]() { return !m_queue.empty(); });
    type obj(std::move(m_queue.front()));
    m_queue.pop_front();
    return std::move(obj);
}

template<class type>
inline size_t block_queue<type>::size() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.size();
}

template<class type>
inline void block_queue<type>::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.clear();
}

#endif
