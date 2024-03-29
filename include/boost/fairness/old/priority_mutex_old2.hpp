/**
 * @file priority_mutex.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana
 * @brief This file contains the implementation of the priority_mutex.
 * @version 0.1
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_PRIORITY_MUTEX_HPP
#define BOOST_FAIRNESS_PRIORITY_MUTEX_HPP
#include <atomic>
#include <array>
#include <boost/fairness/priority_t.hpp>
#include <boost/fairness/detail/wait_ops.hpp>

namespace boost::fairness::old2{

    #define LOCK_OWNED 1
    #define LOCK_NOT_OWNED 0

    /**
     * @brief The priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * 
     * @tparam N : number of 0 indexed priorities the priority_mutex manages, up to BOOST_FAIRNESS_MAXIMUM_PRIORITY.
     */
    template<size_t N = 1>
    requires (N >= 1 && N <= BOOST_FAIRNESS_MAXIMUM_PRIORITY)
    class priority_mutex{

        using Thread_cnt_t = uint32_t;

        public:

        /// @private
        priority_mutex() = default;

        /// @private
        priority_mutex(const priority_mutex&) = delete;

        /// @private
        priority_mutex& operator=(const priority_mutex&) = delete;

        /// @private
        priority_mutex(priority_mutex&&) = delete;

        /// @private
        priority_mutex& operator=(priority_mutex&&) = delete;

        /// @private
        ~priority_mutex() = default;

        /**
         * @brief Try to acquire the unique ownership of the priority_mutex, blocking the thread if the priority_mutex was already owned or other threads are waiting with higher priority.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * priority_mutex<10> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.lock(prio);
         *      //...some code.
         * }
         * \endcode
         */
        void lock(Priority_t const priority = 0){
            uint32_t localOwned = lockOwned_.load(std::memory_order_relaxed);
            Priority_t localCurrentPriority = currentPriority_.load(std::memory_order_relaxed);
            waiters_[priority].fetch_add(1, std::memory_order_relaxed);
            while ( 
                localCurrentPriority < priority ||
                localOwned == LOCK_OWNED ||
                !currentPriority_.compare_exchange_weak(localCurrentPriority, priority, std::memory_order_relaxed) ||
                !lockOwned_.compare_exchange_weak(localOwned, LOCK_OWNED, std::memory_order_acquire)
            ){
                detail::wait(lockOwned_, LOCK_OWNED);
                localCurrentPriority = currentPriority_;
                localOwned = lockOwned_;
            }
            waiters_[priority].fetch_sub(1, std::memory_order_relaxed);
        }

        /**
         * @brief Release the priority_mutex from unique ownership.
         * 
         * \code{.cpp}
         * priority_mutex<10> m;
         * 
         * void my_function() {
         *      //...some code.
         *      m.unlock();
         *      //...some code.
         * }
         * \endcode
         */
        void unlock(){
            currentPriority_.store(find_first_priority_(), std::memory_order_relaxed);
            lockOwned_.store(LOCK_NOT_OWNED, std::memory_order_release);
            //lockOwned_.notify_all();
            detail::notify_all(lockOwned_);
        }

        /**
         * @brief Try to acquire the unique ownership of the priority_mutex, if successful will return true, false otherwise.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * priority_mutex<10> m;
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
            uint32_t localOwned = lockOwned_.load(std::memory_order_relaxed);
            return (currentPriority_.load(std::memory_order_relaxed) >= priority &&
                    localOwned == LOCK_NOT_OWNED &&
                    lockOwned_.compare_exchange_strong(localOwned, LOCK_OWNED, std::memory_order_acquire)
                    );
        }

        private:
        std::array<std::atomic<Thread_cnt_t>, N> waiters_{};
        std::atomic<Priority_t> currentPriority_{BOOST_FAIRNESS_MAXIMUM_PRIORITY};
        std::atomic<uint32_t> lockOwned_{LOCK_NOT_OWNED};

        Priority_t find_first_priority_(){
            for (Priority_t i = 0; i < N; ++i){
                if (waiters_[i] > 0)
                    return i;
            }
            return BOOST_FAIRNESS_MAXIMUM_PRIORITY;
        }
    };

    #undef LOCK_OWNED
    #undef LOCK_NOT_OWNED

}
#endif // BOOST_FAIRNESS_PRIORITY_MUTEX_HPP
