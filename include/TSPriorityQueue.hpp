#include <thread>
#include <queue>

#pragma once

// Implement a fairly proper threadsafe queue...
template <typename T> class TSPriorityQueue {
    public:
        /// Pushes an item onto the queue, and notifies one waiting thread.
        void push(const T& item)
        {
            {
                std::lock_guard lock(mutex);
                queue.push(item);
            }

            cond_var.notify_one();
        };

        /// Provides the entry from the front of the queue
        T& front()
        {
            std::unique_lock lock(mutex);
            cond_var.wait(lock, [&]{ return !queue.empty(); });
            return queue.front();
        };

        /// Removes the front entry from the queue, so that the next front
        /// entry will be the next-highest priority entry.
        void pop()
        {
            std::lock_guard lock(mutex);
            queue.pop();
        };

        /// Check if the queue is empty.
        bool empty()
        {
            return queue.empty();
        }

        /// Returns the number of elements in the queue.

        size_t size()
        {
            return queue.size();
        }

    private:
        std::mutex mutex;
        std::condition_variable cond_var;
        std::priority_queue<T> queue;
};