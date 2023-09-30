/**
 * @file recursive_priority_mutex.h
 * @author S. Martorana
 * @author F. Abrignani (federignoli@hotmail.it)
 * @brief This file contains the implementation of the recursive_priority_mutex.
 * @version 0.1
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */

#ifndef BOOST_FAIRNESS_RECURSIVE_PRIORITY_MUTEX_HPP
#define BOOST_FAIRNESS_RECURSIVE_PRIORITY_MUTEX_HPP
#include <thread>
#include <atomic>
#include <array>
#include <chrono>
#include <thread>
#include <boost/fairness/priority_t.hpp>
#include <mutex>

namespace boost::fairness{

    /*
    TODO: the idea is that, since we can only have 1 thread owning the lock we need only 1 recursiveness counter instead of a problematic thread_local one
    currently a spinlock without any wait/notify mechanism... should test if the logic is sound first
    */

    /**
     * @brief The recursive_priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * 
     * @tparam N : number of 0 indexed priorities the recursive_priority_mutex manages, up to _max_priority.
     */

    template<size_t N = 1>
    requires (N >= 1 && N <= _max_priority)
    class recursive_priority_mutex{

        using Thread_cnt_t = uint32_t;

        public:

        /// @private
        recursive_priority_mutex() = default;

        /// @private
        recursive_priority_mutex(const recursive_priority_mutex&) = delete;

        /// @private
        recursive_priority_mutex& operator=(const recursive_priority_mutex&) = delete;

        /// @private
        recursive_priority_mutex(recursive_priority_mutex&&) = delete;

        /// @private
        recursive_priority_mutex& operator=(recursive_priority_mutex&&) = delete;

        /// @private
        ~recursive_priority_mutex() = default;

        /**
         * @brief Try to acquire the unique ownership of the recursive_priority_mutex, blocking the thread if the recursive_priority_mutex was already owned or other threads are waiting with higher priority.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * recursive_priority_mutex<10> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.lock(prio);
         *      //...some code.
         * }
         * \endcode
         */
        void lock(Priority_t const priority = 0){
            Priority_t localCurrentPriority = currentPriority_.load(std::memory_order_relaxed);
            waiters_[priority].fetch_add(1, std::memory_order_relaxed);
            while ( 
                (localCurrentPriority < priority || !currentPriority_.compare_exchange_weak(localCurrentPriority, priority, std::memory_order_relaxed)) ||
                (!try_take_ownership_weak_())
            ){
                // wait here
                localCurrentPriority = currentPriority_;
            }
            waiters_[priority].fetch_sub(1, std::memory_order_relaxed);
            ++recursionCounter_;
        }

        /**
         * @brief Release the recursive_priority_mutex from unique ownership.
         * 
         * \code{.cpp}
         * recursive_priority_mutex<10> m;
         * 
         * void my_function() {
         *      //...some code.
         *      m.unlock();
         *      //...some code.
         * }
         * \endcode
         */
        void unlock(){
            --recursionCounter_;
            if (recursionCounter_ == 0){
                currentPriority_.store(find_first_priority_(), std::memory_order_relaxed);
                owner_.store(std::thread::id(), std::memory_order_release);
                // notify here
            }
        }

        /**
         * @brief Try to acquire the unique ownership of the recursive_priority_mutex, if successful will return true, false otherwise.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * recursive_priority_mutex<10> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.try_lock(prio);
         *      //...some code.
         * }
         * \endcode
         * @return bool 
         */
        [[nodiscard]] bool try_lock(Priority_t const priority = 0){
            bool lockTaken = currentPriority_.load(std::memory_order_relaxed) >= priority && try_take_ownership_strong_();
            if (lockTaken)
                ++recursionCounter_;
            return lockTaken;
        }

        private:
        std::array<std::atomic<Thread_cnt_t>, N> waiters_;
        std::atomic<Priority_t> currentPriority_{_max_priority};
        std::atomic<std::thread::id> owner_{};
        uint32_t recursionCounter_{}; // does it make sense for you to be an atomic? or you are already protected by the mutex structure itself?

        bool try_take_ownership_weak_(){
            std::thread::id localOwner = owner_.load(std::memory_order_relaxed); // if there is no owner or you are the owner
            return (std::thread::id() == localOwner || localOwner == std::this_thread::get_id()) && owner_.compare_exchange_weak(localOwner, std::this_thread::get_id());
        }

        bool try_take_ownership_strong_(){
            std::thread::id localOwner = owner_.load(std::memory_order_relaxed); // if there is no owner or you are the owner
            return (std::thread::id() == localOwner || localOwner == std::this_thread::get_id()) && owner_.compare_exchange_strong(localOwner, std::this_thread::get_id());
        }

        Priority_t find_first_priority_(){
            for (Priority_t i = 0; i < N; ++i){
                if (waiters_[i] > 0)
                    return i;
            }
            return _max_priority;
        }
    };
}
#endif // BOOST_FAIRNESS_RECURSIVE_PRIORITY_MUTEX_HPP
