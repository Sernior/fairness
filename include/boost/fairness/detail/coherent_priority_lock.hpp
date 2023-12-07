/**
 * @file mcs_priority_lock.hpp
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
#include <boost/fairness/detail/pause_ops.hpp>
#include <boost/fairness/priority_t.hpp>


namespace boost::fairness::detail{
    #define PENDING 1
    #define GRANTED 0

    struct Thread;

    struct alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) Request{ // this also requires an inUse bool to determine if it is being used by the pool or not
        std::atomic<uint32_t> state_{PENDING};
        std::atomic<Thread*> watcher_{nullptr};
        std::atomic<Thread*> thread_{nullptr};
        
        char padding[BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE-sizeof(state_)-sizeof(watcher_)-sizeof(thread_)]; // test without padding... I am not sure it is necessary

    };

   struct Thread{ // would this be better aligned aswell?
        Thread(){ // this constructor should take a Request taken by a static array of requests used as a Pool.


            request_.store(new Request); // TODO find a better way !!! this is a memory leak READ ABOVE
            // I need a pool of Requests to manage this allocation efficiently

            request_.load()->thread_.store(this);
        }

        Priority_t priority_;
        std::atomic<Request*> watch_{nullptr};
        std::atomic<Request*> request_;
    }; 

    class coherent_priority_lock{

        public:

        coherent_priority_lock(){
            auto firstTail = new Request; // this new is wrong it should be taken by a pool of preallocated requests
            firstTail->state_ = GRANTED;
            tail_.store(firstTail);
            head_.store(firstTail);
        }

        coherent_priority_lock(Request* firstTail){ // let`s keep this for now but I think I ll just have the lock itself allocate its own firstTail
            firstTail->state_ = GRANTED;
            tail_.store(firstTail);
            head_.store(firstTail);
        }

        void request_lock(Thread* requester){
            requester->watch_.store(tail_.exchange(requester->request_.load()));
            requester->watch_.load()->watcher_.store(requester);
            for(;;){
                if (requester->watch_.load()->state_.load() == GRANTED){
                    delete requester->watch_.load(); // this deleted is wrong we should return the requests to a pool
                    return;
                }
                pause(); // change with spinwait later
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
