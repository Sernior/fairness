/**
 * @file mcs_lock.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana
 * @brief This file contains the implementation of a mcs list based lock.
 * @version 0.1
 * @date 2023-10-06
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_MCS_LOCK_HPP
#define BOOST_FAIRNESS_MCS_LOCK_HPP

#include <atomic>
#include <boost/fairness/detail/pause_ops.hpp>
#include <boost/fairness/priority_t.hpp>

namespace boost::fairness::detail{
    #define LOCKED 1
    #define NOT_LOCKED 0

    struct alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) QNode{
        std::atomic<QNode*> next_{nullptr};
        std::atomic<uint32_t> locked_{LOCKED};
        char padding[BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE-sizeof(next_)-sizeof(locked_)];
    };

    // the idea to add priorities is that by having N tails we can exchange with the nth tail.
    // when we want to acquire instead of looking for the predecessor of node the lock
    // will instead check the tails[p] and continue with the aquire from there.
    // the release instead before selecting as successor the next of node should instead
    // check the previous tails if any of them has a node we select that one.

    class mcs_spinlock{ // TODO add priorities

        public:

        void acquire(QNode* node){

            QNode* predecessor = tail.exchange(node, std::memory_order_relaxed);

            if (predecessor != nullptr){

                predecessor->next_.store(node, std::memory_order_release);

                while (node->locked_.load(std::memory_order_acquire) == LOCKED)
                    pause(); // TODO Change with spinwait

            }
        }

        void release(QNode *node) {

            QNode* successor = node->next_.load(std::memory_order_relaxed);

            if (successor == nullptr) {

                QNode* expected = node;

                if (tail.compare_exchange_strong(expected, nullptr, std::memory_order_release, std::memory_order_relaxed)) // no one else is waiting
                    return;

                do {
                    successor = node->next_.load(std::memory_order_relaxed);
                } while (successor == nullptr);
            }

            successor->locked_.store(NOT_LOCKED, std::memory_order_release);
        }

        private:

        std::atomic<QNode*> tail{nullptr};

    };

    #undef LOCKED
    #undef NOT_LOCKED
}
#endif // BOOST_FAIRNESS_MCS_LOCK_HPP

/*
lock(){
    QNode n;
    lock.acquire(&n);

}

*/
