/**
 * @file spinlock_priority_mutex_cpl.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of the spinlock_priority_mutex based on a scalable list base algorithm.
 * @version 0.1
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_SPINLOCK_PRIORITY_MUTEX_CPL_HPP
#define BOOST_FAIRNESS_SPINLOCK_PRIORITY_MUTEX_CPL_HPP
#include <atomic>
#include <array>
#include <boost/fairness/priority_t.hpp>
#include <boost/fairness/detail/wait_ops.hpp>
#include <boost/fairness/detail/pqspinlock.hpp>


namespace boost::fairness{

    /**
     * @brief The spinlock_priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * 
     * @tparam N : number of 0 indexed priorities the spinlock_priority_mutex manages, up to BOOST_FAIRNESS_MAXIMUM_PRIORITY.
     */
    template<size_t N = 1>
    requires (N >= 1 && N <= BOOST_FAIRNESS_MAXIMUM_PRIORITY)
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
         * @brief Try to acquire the unique ownership of the spinlock_priority_mutex, blocking the thread if the spinlock_priority_mutex was already owned or other threads are waiting with higher priority.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * spinlock_priority_mutex<4> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.lock(prio);
         *      //...some code.
         * }
         * \endcode
         */
        void lock(Priority_t const priority = 0){
            lll_.lock(priority);
        }

        /**
         * @brief Release the spinlock_priority_mutex from unique ownership.
         * 
         * \code{.cpp}
         * spinlock_priority_mutex<4> m;
         * 
         * void my_function() {
         *      //...some code.
         *      m.unlock();
         *      //...some code.
         * }
         * \endcode
         */
        void unlock(){
            lll_.unlock();
        }

        /**
         * @brief Try to acquire the unique ownership of the spinlock_priority_mutex, if successful will return true, false otherwise.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * spinlock_priority_mutex<4> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.try_lock(prio);
         *      //...some code.
         * }
         * \endcode
         * @return bool 
         */
        //[[nodiscard]] bool try_lock(Priority_t const priority = 0){ // TODO
        //    return false;
        //}

        private:

        detail::pqspinlock lll_;

    };
}
#endif // BOOST_FAIRNESS_SPINLOCK_PRIORITY_MUTEX_CPL_HPP
