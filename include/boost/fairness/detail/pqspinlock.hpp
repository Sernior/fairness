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

        struct pqspinlockStatus{
            uint16_t lockers_{};
            bool isCriticallyResetting{};

            pqspinlockStatus increment(){
                pqspinlockStatus new_status = *this;
                ++new_status.lockers_;
                return new_status;
            }

            pqspinlockStatus decrement(){
                pqspinlockStatus new_status = *this;
                --new_status.lockers_;
                return new_status;
            }

            pqspinlockStatus enterCriticalReset(){
                pqspinlockStatus new_status = *this;
                new_status.isCriticallyResetting = true;
                return new_status;
            }

            pqspinlockStatus exitCriticalReset(){
                pqspinlockStatus new_status = *this;
                new_status.isCriticallyResetting = false;
                return new_status;
            }
        };

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
            Request* req;

            pqspinlockStatus localStatus = status_.load();

            for (;;){
                if (!localStatus.isCriticallyResetting && status_.compare_exchange_weak(localStatus, localStatus.increment()))
                    break;
                pause();
                localStatus = status_.load();
            }

            for(;;){
                req = reqPool_.getRequest();
                if (req != nullptr)
                    break;
                pause();
            }
            t_.prepare(priority, req);

            cpl_.requestLock(&t_);


        }

        void unlock(){
            pqspinlockStatus localStatus = status_.load();

            cpl_.grantLock(&t_);

            if (!t_.watch_->isFirstTail_)
                reqPool_.returnRequest(t_.watch_);


            for (;;){
                if (status_.compare_exchange_weak(localStatus, localStatus.decrement()))
                    break;
                pause();
            }

            if (localStatus.lockers_ == 0 && status_.compare_exchange_strong(localStatus, localStatus.enterCriticalReset())){
                if (cpl_.head_.load() == cpl_.tail_.load())
                    cpl_.reset_();
                for (;;){
                    if (status_.compare_exchange_weak(localStatus, localStatus.exitCriticalReset()))
                        break;
                    pause();
                }
            }
        }
        private:
        std::atomic<pqspinlockStatus> status_{};
        coherent_priority_lock cpl_;

    };

}
#endif // BOOST_FAIRNESS_PQSPINLOCK_HPP
