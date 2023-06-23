#pragma once

#include <cstddef>
#include <memory>
#include <array>
#include <optional>
#include <mutex>
#include <condition_variable>

#define QUEUE_IS_FULL ((m_size + 1) >= Capacity)
#define QUEUE_IS_EMPTY (m_size <= 0)

namespace vui {

    class Command;

    template<size_t Capacity>
    class CommandQueue {
    public:
        CommandQueue();

        ~CommandQueue();

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
        std::array<std::unique_ptr<Command>, Capacity> m_data{};

    };

    template<size_t Capacity>
    CommandQueue<Capacity>::CommandQueue() {
        m_lock = std::unique_lock<std::mutex>(m_mutex);
    }

    template<size_t Capacity>
    CommandQueue<Capacity>::~CommandQueue(){
        m_lock.unlock();
    }

    template<size_t Capacity>
    bool CommandQueue<Capacity>::offer(std::unique_ptr<Command> command) {
        std::lock_guard<std::mutex> llk_guard{m_mutex};
        if(QUEUE_IS_FULL){
            return false;
        }
        m_data[m_size] = std::move(command);
        m_size++;
        return true;
    }

    template<size_t Capacity>
    void CommandQueue<Capacity>::put(std::unique_ptr<Command> command) {
        m_lock.lock();
        if(QUEUE_IS_FULL){
            m_queue_full_cv.wait(m_lock);
        }
        m_data[m_size] = std::move(command);
        m_size++;
        m_lock.unlock();
        m_queue_empty_cv.notify_all();
    }

    template<size_t Capacity>
    std::unique_ptr<Command> CommandQueue<Capacity>::take() {
        m_lock.lock();
        if(QUEUE_IS_EMPTY){
            m_queue_empty_cv.wait(m_lock);
        }
        m_size--;
        auto data = std::move(m_data[m_size]);
        m_lock.unlock();
        m_queue_full_cv.notify_all();

        return data;
    }

    template<size_t Capacity>
    std::unique_ptr<Command> CommandQueue<Capacity>::poll() {
        std::lock_guard<std::mutex> llk_guard{m_mutex};
        if(QUEUE_IS_EMPTY){
            return nullptr;
        }
        m_size--;
        auto data = std::move(m_data[m_size]);
        return data;
    }
}