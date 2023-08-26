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
#include "priority_t.h"

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
        void lock(Priority_t priority = 0){// 2 1 0
            bool localLockOwned = lockOwned_;
            while (localLockOwned || !lockOwned_.compare_exchange_weak(localLockOwned, true)){
                lockOwned_.wait(localLockOwned);
                localLockOwned = lockOwned_;
            }

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
            lockOwned_.store(false);
            lockOwned_.notify_all();
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
        [[nodiscard]] bool try_lock(Priority_t priority = 0){
            bool FALSE = false;
            return lockOwned_.compare_exchange_strong(FALSE , true);
        }


        private:

        std::atomic<Priority_t> currentPriority_{0};
        std::atomic<bool> lockOwned_{};//?

    };
}
