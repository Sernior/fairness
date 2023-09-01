/**
 * @file priority_mutex.h
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author P. Di Giglio
 * @author S. Martorana
 * @brief This file contains the implementation of the priority_mutex.
 * @version 0.1
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#pragma once
#include <thread>
#include <atomic>
#include <array>
#include <chrono>
#include <thread>
#include "priority_t.h"
#include <mutex>

namespace PrioSync{// the name has yet to be chosen

    /**
     * @brief The priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * 
     * @tparam N : number of 0 indexed priorities the priority_mutex manages, up to _max_priority.
     */
    template<Priority_t N = 1, typename = std::enable_if_t<(N >= 1 && N <= _max_priority)>>
    class spinlock_priority_mutex{

        using Thread_cnt_t = uint32_t;

        public:

        /// @private
        spinlock_priority_mutex() = default;

        /// @private
        spinlock_priority_mutex(const spinlock_priority_mutex&) = delete;

        /// @private
        spinlock_priority_mutex& operator=(const spinlock_priority_mutex&) = delete;

        /// @private
        spinlock_priority_mutex(spinlock_priority_mutex&&) = delete;

        /// @private
        spinlock_priority_mutex& operator=(spinlock_priority_mutex&&) = delete;

        /// @private
        ~spinlock_priority_mutex() = default;

        /**
         * @brief Try to acquire the unique ownership of the priority_mutex, blocking the thread if the priority_mutex was already owned or other threads are waiting with higher priority.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * priority_mutex<10> m;
         * m.lock(9);
         * \endcode
         */
        void lock(Priority_t const priority = 0){
            Priority_t localCurrentPriority = currentPriority_.load(std::memory_order_relaxed);
            waiters_[priority].fetch_add(1, std::memory_order_relaxed);
            while ( 
                (localCurrentPriority < priority || !currentPriority_.compare_exchange_weak(localCurrentPriority, priority, std::memory_order_relaxed)) ||
                (lockOwned_.test_and_set(std::memory_order_acquire))
            ){
                lockOwned_.wait(true);
                localCurrentPriority = currentPriority_;
            }
            waiters_[priority].fetch_sub(1, std::memory_order_relaxed);
        }

        /**
         * @brief Release the priority_mutex from unique ownership.
         * 
         * \code{.cpp}
         * priority_mutex<10> m;
         * m.lock(9);
         * m.unlock();
         * \endcode
         */
        void unlock(){
            currentPriority_.store(find_first_priority_(), std::memory_order_relaxed);
            lockOwned_.clear(std::memory_order_release);
            if(currentPriority_.load(std::memory_order_relaxed) != _max_priority)
                lockOwned_.notify_all();//P2616R3 https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2616r3.html this shouldnt be a problem with mutex semantics
        }

        /**
         * @brief Try to acquire the unique ownership of the priority_mutex, if successful will return true, false otherwise.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * priority_mutex<10> m;
         * m.try_lock(9);
         * \endcode
         * @return bool 
         */
        [[nodiscard]] bool try_lock(Priority_t const priority = 0){
            return (currentPriority_.load(std::memory_order_relaxed) >= priority && !lockOwned_.test_and_set(std::memory_order_acquire));
        }

        private:
        std::array<std::atomic<uint32_t>, N> waiters_;
        std::atomic<Priority_t> currentPriority_{_max_priority};
        std::atomic_flag lockOwned_;

        Priority_t find_first_priority_(){
            for (Priority_t i = 0; i < N; i++){
                if (waiters_[i] > 0)
                    return i;
            }
            return _max_priority;
        }
    };
}
