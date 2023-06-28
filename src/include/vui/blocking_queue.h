#pragma once

#include <cstddef>
#include <array>
#include <optional>
#include <mutex>
#include <condition_variable>
#include <optional>

#define QUEUE_IS_FULL (m_size >= Capacity)
#define QUEUE_IS_EMPTY (m_size <= 0)

namespace vui {
    
    template<typename Entry, size_t Capacity>
    class BlockingQueue {
    public:
        using entry_type = Entry;
        
        BlockingQueue() = default;

        ~BlockingQueue() = default;

        BlockingQueue(const BlockingQueue<Entry, Capacity>& other) = delete;

        BlockingQueue(BlockingQueue<Entry, Capacity>&& other) noexcept {
            operator=(other);
        }

        BlockingQueue& operator=(const BlockingQueue<Entry, Capacity>& other) = delete;

        BlockingQueue& operator=(BlockingQueue<Entry, Capacity> other){
            if(&other == this){
                return *this;
            }
            std::exchange(m_size, other.m_size);
            std::exchange(m_mutex, other.m_mutex);
            std::exchange(m_queue_empty_cv, other.m_queue_empty_cv);
            std::exchange(m_queue_full_cv, other.m_queue_full_cv);
            std::exchange(m_data, other.m_data);

            return *this;
        }

        bool offer(entry_type entry);

        void put(entry_type entry);

        entry_type take();

        std::optional<entry_type> poll();

    private:
        size_t m_size{0};
        std::mutex m_mutex;
        std::condition_variable m_queue_empty_cv;
        std::condition_variable m_queue_full_cv;
        std::array<entry_type, Capacity> m_data{};

    };

    template<typename Entry, size_t Capacity>
    bool BlockingQueue<Entry, Capacity>::offer(entry_type entry) {
        {
            std::lock_guard<std::mutex> llk_guard{m_mutex};
            if (QUEUE_IS_FULL) {
                return false;
            }
            m_data[m_size] = entry;
            m_size++;
        }
        m_queue_empty_cv.notify_all();
        return true;
    }

    template<typename Entry, size_t Capacity>
    void BlockingQueue<Entry, Capacity>::put(entry_type entry) {
        std::unique_lock<std::mutex> lock{m_mutex};
        if(QUEUE_IS_FULL){
            m_queue_full_cv.wait(lock);
        }
        m_data[m_size] = entry;
        m_size++;
        lock.unlock();
        m_queue_empty_cv.notify_all();
    }

    template<typename Entry, size_t Capacity>
    Entry BlockingQueue<Entry, Capacity>::take() {
        std::unique_lock<std::mutex> lock{m_mutex};
        if(QUEUE_IS_EMPTY){
            m_queue_empty_cv.wait(lock);
        }
        m_size--;
        auto data = std::move(m_data[m_size]);
        lock.unlock();
        m_queue_full_cv.notify_all();

        return data;
    }

    template<typename Entry, size_t Capacity>
    std::optional<Entry> BlockingQueue<Entry, Capacity>::poll() {
        entry_type data;
        {
            std::lock_guard<std::mutex> llk_guard{m_mutex};
            if (QUEUE_IS_EMPTY) {
                return {};
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