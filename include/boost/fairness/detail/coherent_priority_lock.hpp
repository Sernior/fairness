/**
 * @file mcs_priority_lock.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana
 * @brief This file contains the implementation of a coherent priority lock.
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
#include <boost/fairness/detail/pause_ops.hpp>
#include <boost/fairness/priority_t.hpp>


namespace boost::fairness::detail{
    #define PENDING 1
    #define GRANTED 0

    struct Thread;

    struct alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) Request{
        std::atomic<uint32_t> state_{PENDING};
        std::atomic<Thread*> watcher_{nullptr};
        std::atomic<Thread*> thread_{nullptr};
        // TODO maybe add padding? ??
    };

    struct Thread{ // most likely not atomic
        Thread(){
            request_ = new Request; // TODO find a better way !!! this is a memory leak
            request_.load()->thread_.store(this);
        }
        Priority_t priority_;
        std::atomic<Request*> watch_{nullptr}; // TODO try not to make these atomic
        std::atomic<Request*> request_;
    };

    class coherent_priority_lock{

        public:

        coherent_priority_lock(){
            static Request firstTail; // TODO find a better way this can go once the first req is resolved
            firstTail.state_ = GRANTED;
            tail_.store(&firstTail);
            head_.store(&firstTail);
        }

        void request_lock(Thread* requester){
            requester->watch_.store(tail_.exchange(requester->request_.load()));
            requester->watch_.load()->watcher_.store(requester);
            for(;;){
                if (requester->watch_.load()->state_.load() == GRANTED)
                    return;
            }
        }

        void grant_lock(Thread* requester) {
            Priority_t localHighestPriority{BOOST_FAIRNESS_MAXIMUM_PRIORITY};
            Thread* currentThread;
            Request* localHighestPriorityReq;

            requester->request_.load()->thread_.store(requester->watch_.load()->thread_.load());

            if (requester->request_.load()->thread_.load() != nullptr)
                requester->request_.load()->thread_.load()->request_.store(requester->request_.load());
            else
                head_.store(requester->request_.load());

            localHighestPriorityReq = head_.load(); 
            currentThread = head_.load()->watcher_.load();

            while (currentThread != nullptr){
                if (currentThread->priority_ < localHighestPriority){
                    localHighestPriority = currentThread->priority_;
                    localHighestPriorityReq = currentThread->watch_.load();
                }
                currentThread = currentThread->request_.load()->watcher_.load();
            }

            localHighestPriorityReq->state_.store(GRANTED);

            requester->request_.store(requester->watch_.load());
            requester->request_.load()->thread_.store(requester);
        }

        private:
        std::atomic<Request*> tail_{nullptr};
        std::atomic<Request*> head_{nullptr};

    };

    #undef PENDING
    #undef GRANTED
}
#endif // BOOST_FAIRNESS_COHERENT_PRIORITY_LOCK_HPP
