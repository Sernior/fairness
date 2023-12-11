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
// TODO move to old this is unneeded
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

    class mcs_spinlock{

        public:

        void acquire(QNode* node){

            QNode* predecessor = tail.exchange(node, std::memory_order_relaxed);

            if (predecessor != nullptr){

                predecessor->next_.store(node, std::memory_order_release);

                while (node->locked_.load(std::memory_order_acquire) == LOCKED){
                    // if I have higher priority then head or head is nullptr I become head
                    pause(); // TODO Change with spinwait
                }

            }

        }

        void release(QNode *node) {

            QNode* successor = node->next_.load(std::memory_order_relaxed);// head

            if (successor == nullptr) {

                QNode* expected = node;

                // no one else is waiting, you are the last
                if (tail.compare_exchange_strong(expected, nullptr, std::memory_order_release, std::memory_order_relaxed)){
                    return;
                }

                do {
                    successor = node->next_.load(std::memory_order_relaxed);// head
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
