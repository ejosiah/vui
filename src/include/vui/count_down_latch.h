#pragma once

#include <cstddef>
#include <mutex>
#include <condition_variable>

namespace vui {

    template<size_t Count>
    class CountDownLatch {
    public:
        CountDownLatch(){
            count = Count;
        };

        void await() {
            std::unique_lock<std::mutex> lock{m_mutex};
            lock.lock();
            if(count > 0){
                cv.wait(lock);
            }
            lock.unlock();
        }

        void countDown() {
            std::unique_lock<std::mutex> lock{m_mutex};
            lock.lock();
            count--;
            lock.unlock();
            cv.notify_one();
        }

    private:
        std::mutex m_mutex;
        std::condition_variable cv;
        int count{0};
    };
}