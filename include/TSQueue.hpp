#pragma once

#include <thread>
#include <condition_variable>
#include <queue>

// Implement a fairly proper threadsafe queue...
template <typename T> class TSQueue {
    public:
        void push(const T& item)
        {
            {
                std::lock_guard lock(mutex);
                queue.push(item);
            }

            cond_var.notify_one();
        };

        T& front()
        {
            std::unique_lock lock(mutex);
            cond_var.wait(lock, [&]{ return !queue.empty(); });
            return queue.front();
        };

        void pop()
        {
            std::lock_guard lock(mutex);
            queue.pop();
        };

        bool empty()
        {
            return queue.empty();
        }

        size_t size()
        {
            return queue.size();
        }

    private:
        std::mutex mutex;
        std::condition_variable cond_var;
        std::queue<T> queue;
};