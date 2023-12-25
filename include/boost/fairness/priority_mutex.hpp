/**
 * @file priority_mutex.hpp
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
#ifndef BOOST_FAIRNESS_PRIORITY_MUTEX_HPP
#define BOOST_FAIRNESS_PRIORITY_MUTEX_HPP
#include <atomic>
#include <array>
#include <boost/fairness/priority_t.hpp>
#include <boost/fairness/detail/wait_ops.hpp>
#include <boost/fairness/spinlock_priority_mutex.hpp>
#include <boost/fairness/detail/wait_pool.hpp>

namespace boost::fairness{

    #define WAIT 0
    #define PROCEED 1

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

            internalMutex_.lock(priority);

            ++waiters_[priority];

            for (;;){

                if (
                    !lockOwned_ &&
                    find_first_priority_() >= priority
                ){

                    --waiters_[priority];

                    lockOwned_ = true;

                    internalMutex_.unlock();

                    return;
                }

                internalMutex_.unlock();

                detail::wait(waitingFlag_[priority], detail::wait_flag);

                internalMutex_.lock(priority);

            }
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

            Priority_t p;

            internalMutex_.lock();

            lockOwned_ = false;

            p = find_first_priority_();

            if (p == BOOST_FAIRNESS_MAXIMUM_PRIORITY){
                internalMutex_.unlock();
                return;
            }

            //reset_(p);
            waitingFlag_.reset_(p);

            internalMutex_.unlock();

            detail::notify_one(waitingFlag_[p]);
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
            internalMutex_.lock(priority);

            if (lockOwned_ ||
                find_first_priority_() < priority){

                internalMutex_.unlock();

                return false;
            }

            lockOwned_ = true;

            internalMutex_.unlock();

            return true;
        }

        private:

        alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) spinlock_priority_mutex<N> internalMutex_;

        detail::WaitPool<N> waitingFlag_{};

        std::array<Thread_cnt_t, N> waiters_{};
        
        bool lockOwned_{};

        Priority_t find_first_priority_(){
            for (Priority_t i = 0; i < N; ++i){
                if (waiters_[i] > 0)
                    return i;
            }
            return BOOST_FAIRNESS_MAXIMUM_PRIORITY;
        }

    };

    #undef WAIT
    #undef PROCEED

}
#endif // BOOST_FAIRNESS_PRIORITY_MUTEX_HPP
