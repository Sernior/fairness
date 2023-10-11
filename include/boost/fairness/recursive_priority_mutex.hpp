/**
 * @file recursive_priority_mutex.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
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
#include <boost/fairness/priority_t.hpp>
#include <boost/fairness/spinlock_priority_mutex.hpp>

namespace boost::fairness{

    /**
     * @brief The recursive_priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * 
     * @tparam N : number of 0 indexed priorities the recursive_priority_mutex manages, up to BOOST_FAIRNESS_MAXIMUM_PRIORITY.
     */

    template<size_t N = 1>
    requires (N >= 1 && N <= BOOST_FAIRNESS_MAXIMUM_PRIORITY)
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
            internalMutex_.lock(priority);
            ++waiters_[priority];
            for(;;){
                if ( lock_owned_by_me_() ||
                    (find_first_priority_() >= priority &&
                    lock_not_owned_())){
                        owner_ = std::this_thread::get_id();
                        lockOwned_.test_and_set();
                        break;
                    }
                internalMutex_.unlock();
                lockOwned_.wait(true);
                internalMutex_.lock(priority);
            }
            --waiters_[priority];
            ++recursionCounter_;
            internalMutex_.unlock();
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
            internalMutex_.lock();
            --recursionCounter_;
            if (recursionCounter_ == 0){
                owner_ = std::thread::id();
                lockOwned_.clear();
                lockOwned_.notify_all();
            }
            internalMutex_.unlock();
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

            internalMutex_.lock(priority);

            if (lock_owned_by_me_() ||
             (find_first_priority_() >= priority && lock_not_owned_())){

                owner_ = std::this_thread::get_id();
                lockOwned_.test_and_set();
                ++recursionCounter_;
                internalMutex_.unlock();
                return true;
                
             }

            internalMutex_.unlock();
            return false;
        }

        private:
        spinlock_priority_mutex<N> internalMutex_;
        std::array<Thread_cnt_t, N> waiters_;
        std::thread::id owner_{};
        std::atomic_flag lockOwned_;
        uint32_t recursionCounter_{};

        bool lock_not_owned_(){
            return std::thread::id() == owner_;
        }

        bool lock_owned_by_me_(){
            return owner_ == std::this_thread::get_id();
        }

        Priority_t find_first_priority_(){
            for (Priority_t i = 0; i < N; ++i){
                if (waiters_[i] > 0)
                    return i;
            }
            return BOOST_FAIRNESS_MAXIMUM_PRIORITY;
        }
    };
}
#endif // BOOST_FAIRNESS_RECURSIVE_PRIORITY_MUTEX_HPP
