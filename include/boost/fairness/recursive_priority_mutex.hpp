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
            return false;
        }

        private:
        std::array<std::atomic<Thread_cnt_t>, N> waiters_;
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
#endif // BOOST_FAIRNESS_RECURSIVE_PRIORITY_MUTEX_HPP
