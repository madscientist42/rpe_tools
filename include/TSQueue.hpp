#pragma once

#include <thread>
#include <condition_variable>
#include <queue>

// Implement a fairly proper threadsafe queue...
template <typename T> class TSQueue {
    public:
        /**
         * Constructor
         */
        TSQueue(size_t size = 512, bool blocking = true) : m_blocking(false), m_size(size) {};

        /**
         * Adds an item to the end of the queue.
         * This method is thread-safe and will notify one waiting thread
         * that an item has been added.
         *
         * @param item The item to be added to the queue.
         */
        void push(const T& item)
        {
            {
                std::lock_guard lock(mutex);
                if (queue.size() >= m_size)
                {
                    if (m_blocking)
                    {
                        // Wait until there is room
                        cond_var.wait(lock, [&]{ return queue.size() < m_size; });
                    }
                    else
                    {
                        // Burn the front.  Throw it on the floor.  We're full and this isn't blocking.
                        queue.pop();
                    }
                }
                queue.push(item);
            }

            cond_var.notify_one();
        };

        /**
         * Returns the item at the front of the queue.
         * This method is thread-safe.
         *
         * @return The item at the front of the queue.
         */
        T& front()
        {
            std::unique_lock lock(mutex);
            cond_var.wait(lock, [&]{ return !queue.empty(); });
            return queue.front();
        };

        /**
         * Removes the item at the front of the queue.
         * This method is thread-safe.
         */
        void pop()
        {
            std::lock_guard lock(mutex);
            queue.pop();
            cond_var.notify_one();
        };

        /**
         * Checks if the queue is empty.
         * This method is thread-safe.
         *
         * @return True if the queue is empty, otherwise false.
         */
        bool empty()
        {
            return queue.empty();
        }

        /**
         * Returns the current size of the queue.
         * This method is thread-safe.
         *
         * @return The current size of the queue.
         */
        size_t size()
        {
            return queue.size();
        }

    private:
        bool m_blocking;
        size_t m_size;
        std::mutex mutex;
        std::condition_variable cond_var;
        std::queue<T> queue;
};