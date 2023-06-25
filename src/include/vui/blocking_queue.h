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
    
    template<typename Entry, size_t Capacity>
    class BlockingQueue {
    public:
        using entry_type = Entry;
        
        BlockingQueue();

        ~BlockingQueue();

        bool offer(std::unique_ptr<entry_type> element);

        void put(std::unique_ptr<entry_type> element);

        std::unique_ptr<entry_type> take();

        std::unique_ptr<entry_type> poll();

    private:
        size_t m_size{0};
        std::mutex m_mutex;
        std::unique_lock<std::mutex> m_lock;
        std::condition_variable m_queue_empty_cv;
        std::condition_variable m_queue_full_cv;
        std::array<std::unique_ptr<entry_type>, Capacity> m_data{};

    };

    template<typename Entry, size_t Capacity>
    BlockingQueue<Entry, Capacity>::BlockingQueue() {
        m_lock = std::unique_lock<std::mutex>(m_mutex);
    }

    template<typename Entry, size_t Capacity>
    BlockingQueue<Entry, Capacity>::~BlockingQueue(){
        m_lock.unlock();
    }

    template<typename Entry, size_t Capacity>
    bool BlockingQueue<Entry, Capacity>::offer(std::unique_ptr<entry_type> command) {
        {
            std::lock_guard<std::mutex> llk_guard{m_mutex};
            if (QUEUE_IS_FULL) {
                return false;
            }
            m_data[m_size] = std::move(command);
            m_size++;
        }
        m_queue_empty_cv.notify_all();
        return true;
    }

    template<typename Entry, size_t Capacity>
    void BlockingQueue<Entry, Capacity>::put(std::unique_ptr<entry_type> command) {
        m_lock.lock();
        if(QUEUE_IS_FULL){
            m_queue_full_cv.wait(m_lock);
        }
        m_data[m_size] = std::move(command);
        m_size++;
        m_lock.unlock();
        m_queue_empty_cv.notify_all();
    }

    template<typename Entry, size_t Capacity>
    std::unique_ptr<Entry> BlockingQueue<Entry, Capacity>::take() {
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

    template<typename Entry, size_t Capacity>
    std::unique_ptr<Entry> BlockingQueue<Entry, Capacity>::poll() {
        std::unique_ptr<entry_type> data;
        {
            std::lock_guard<std::mutex> llk_guard{m_mutex};
            if (QUEUE_IS_EMPTY) {
                return nullptr;
            }
            m_size--;
            data = std::move(m_data[m_size]);
        }
        m_queue_full_cv.notify_one();
        return data;
    }
    
    template<typename Entry>
    using SingleEntryBlockingQueue = BlockingQueue<Entry, 1>;
}