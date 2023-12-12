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
#include <boost/fairness/detail/request_pool.hpp>

namespace boost::fairness::detail{

    /* TODO
    A few notes on this:
    This is partially correct but not quite yet as having a static thread_local Thread means that if we create multiple pqspinlocks
    then the same thread could use the same Thread object to do multiple aquires or releases which would obviously break
    the coherent_priority_lock.
    The right way of doing this would be to know how many of these are needed at compile time and thus instantiating
    a thread_local array of threads 1 per pqspinlock required.
    Or maybe all the mutexes instances could use only 1 pqspinlock and since I must use a limited number of preallocated Requests I dont think
    this would even be the bottle neck of the lib.
    */
    static thread_local Thread t_;

    class pqspinlock{

        public:

        /// @private
        pqspinlock() = default;

        /// @private
        pqspinlock(const pqspinlock&) = delete;

        /// @private
        pqspinlock& operator=(const pqspinlock&) = delete;

        /// @private
        pqspinlock(pqspinlock&&) = delete;

        /// @private
        pqspinlock& operator=(pqspinlock&&) = delete;

        /// @private
        ~pqspinlock() = default;

        void lock(Priority_t const priority = 0){
            Request* req;// = reqs_.getRequest();

            for(;;){
                req = reqs_.getRequest();
                if (req != nullptr)
                    break;
                pause();
            }
            t_.prepare(priority, req);
            cpl_.request_lock(&t_);
        }

        void unlock(){
            cpl_.grant_lock(&t_);
            if (!t_.watch_->isFirstTail_)
                reqs_.returnRequest(t_.watch_);
        }

        private:

        coherent_priority_lock cpl_;
        RequestPool<BOOST_FAIRNESS_MAX_PQNODES> reqs_;

    };

}
#endif // BOOST_FAIRNESS_PQSPINLOCK_HPP
