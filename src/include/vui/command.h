#pragma once

#include <cstddef>
#include <memory>
#include <array>
#include <optional>
#include <mutex>
#include <condition_variable>

class Command;

template<size_t Capacity>
class CommandQueue{
public:
    CommandQueue();

    bool offer(std::unique_ptr<Command> command);

    void put(std::unique_ptr<Command> command);

    std::unique_ptr<Command> take();

    std::unique_ptr<Command> poll();

private:
    size_t m_size{0};
    std::mutex m_mutex;
    std::unique_lock<std::mutex> m_lock;
    std::condition_variable m_queue_empty_cv;
    std::condition_variable m_queue_full_cv;

};

template<size_t Capacity>
CommandQueue<Capacity>::CommandQueue(){
    m_lock = std::unique_lock<std::mutex>(m_mutex);
}