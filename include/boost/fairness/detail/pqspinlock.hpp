/**
 * @file pqspinlock.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana
 * @brief This file contains the implementation of a priority qspinlock.
 * @version 0.1
 * @date 2023-11-19
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_PQSPINLOCK_HPP
#define BOOST_FAIRNESS_PQSPINLOCK_HPP

#include <boost/fairness/config.hpp>
#include <boost/fairness/priority_t.hpp>
#include <boost/fairness/detail/coherent_priority_lock.hpp>

#include <array>///
#include <atomic>///

namespace boost::fairness::detail{

    class pqspinlock{

        public:

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

        void lock(Priority_t const priority = 0){
            boost::fairness::detail::Thread t(priority); // knowing that we have a limited amount of Threads and Requests could allow me to use an array to map these
            cpl_.request_lock(&t);
        }

        void unlock(){
            cpl_.grant_lock(&t);
        }

        private:

        coherent_priority_lock cpl_;

    };

}
#endif // BOOST_FAIRNESS_PQSPINLOCK_HPP
