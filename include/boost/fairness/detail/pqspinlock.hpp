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

            Thread* t = detail::t_.getThread(this);

            for(;;){
                req = reqs_.getRequest();
                if (req != nullptr)
                    break;
                //spin_wait(); // maybe just yield here
                std::this_thread::yield();
            }
            t->prepare(priority, req);
            cpl_.requestLock(t);
        }

        void unlock(){
            Thread* t = detail::t_.reGetThread(this);
            cpl_.grantLock(t);
            reqs_.returnRequest(t->watch_);
            detail::t_.returnThread(t);
        }

        private:

        coherent_priority_lock cpl_;
        RequestPool<BOOST_FAIRNESS_MAX_PQNODES> reqs_;

    };

}
#endif // BOOST_FAIRNESS_PQSPINLOCK_HPP
