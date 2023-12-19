/**
 * @file coherent_priority_lock.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana
 * @brief This file contains the implementation of a coherent priority lock.
 * @brief Based on the work of Travis S. Craig "Building FIFO and Priority-Queuing Spin Locks from Atomic Swap"
 * @version 0.1
 * @date 2023-10-06
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_COHERENT_PRIORITY_LOCK_HPP
#define BOOST_FAIRNESS_COHERENT_PRIORITY_LOCK_HPP

#include <atomic>
#include <boost/fairness/config.hpp>
#include <boost/fairness/detail/wait_ops.hpp>
#include <boost/fairness/priority_t.hpp>
#include <boost/fairness/detail/request_pool.hpp>


namespace boost::fairness::detail{

    struct Thread{

        Thread() = default;

        void prepare(Priority_t p, Request* req){

            request_ = req;

            req->thread_ = this;

            priority_ = p;

            watch_ = nullptr;
        }

        Priority_t priority_{BOOST_FAIRNESS_INVALID_PRIORITY};
        Request* watch_{nullptr};
        Request* request_{nullptr};
    }; 

    class coherent_priority_lock{

        public:

        coherent_priority_lock() = default;

        void initialize(Request* firstTail){

            firstTail->state_ = GRANTED;

            tail_.store(firstTail);

            head_.store(firstTail);

        }

        void requestLock(Thread* requester){
            requester->watch_ = tail_.exchange(requester->request_);
            requester->watch_->watcher_ = requester;
            for(;;){
                if (requester->watch_->state_ == GRANTED){
                    return;
                }
                spin_wait(requester->watch_->state_, GRANTED);
            }
        }

        void grantLock(Thread* requester) {
            Priority_t localHighestPriority{BOOST_FAIRNESS_MAXIMUM_PRIORITY};
            Thread* currentThread;
            Request* localHighestPriorityReq;

            requester->request_->thread_ = requester->watch_->thread_;

            if (requester->request_->thread_ != nullptr)
                requester->request_->thread_->request_ = (requester->request_);
            else
                head_.store(requester->request_);

            localHighestPriorityReq = head_.load();
            currentThread = head_.load()->watcher_;

            while (currentThread != nullptr){
                if (currentThread->priority_ < localHighestPriority){
                    localHighestPriority = currentThread->priority_;
                    localHighestPriorityReq = currentThread->watch_;
                }
                currentThread = currentThread->request_->watcher_;
            }

            localHighestPriorityReq->state_.store(GRANTED);

            requester->request_ = requester->watch_;
            requester->request_->thread_ = requester;
            
        }

        private:
        std::atomic<Request*> tail_{nullptr};
        std::atomic<Request*> head_{nullptr};

    };

    #undef PENDING
    #undef GRANTED
}
#endif // BOOST_FAIRNESS_COHERENT_PRIORITY_LOCK_HPP
