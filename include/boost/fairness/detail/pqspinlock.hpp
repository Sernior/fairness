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

#include <exception>
#include <boost/fairness/config.hpp>
#include <boost/fairness/priority_t.hpp>
#include <boost/fairness/detail/coherent_priority_lock.hpp>
#include <boost/fairness/detail/request_pool.hpp>
#include <boost/fairness/detail/thread_pool.hpp>

namespace boost::fairness::detail{

    class pqspinlock{

        public:

        /// @private
        pqspinlock() {
            cpl_.initialize(reqs_.getRequest());
        };

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
            Request* req;

            Thread* t = t_.getThread(this);

            if (t == nullptr)
                throw std::logic_error("Thread Pool returned a nullptr: Increase BOOST_FAIRNESS_MAX_THREADS");

            for(;;){
                req = reqs_.getRequest();
                if (req != nullptr)
                    break;
                std::this_thread::yield(); // there are no free Requests now just yield
            }
            t->prepare(priority, req);
            cpl_.requestLock(t);
        }

        void unlock(){
            Thread* t = t_.reGetThread(this);
            cpl_.grantLock(t);
            reqs_.returnRequest(t->watch_);
            t_.returnThread(t);
        }

        private:

        coherent_priority_lock<WaitMechanism::Spin> cpl_;
        RequestPool<BOOST_FAIRNESS_MAX_PQNODES> reqs_;

    };

}
#endif // BOOST_FAIRNESS_PQSPINLOCK_HPP
