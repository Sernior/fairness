/**
 * @file mcs_priority_lock.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana
 * @brief This file contains the implementation of a mcs list based priority lock.
 * @version 0.1
 * @date 2023-10-06
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_MCS_PRIORITY_LOCK_HPP
#define BOOST_FAIRNESS_MCS_PRIORITY_LOCK_HPP

#include <atomic>
#include <array>
#include <boost/fairness/detail/pause_ops.hpp>
#include <boost/fairness/priority_t.hpp>

namespace boost::fairness::detail{
    #define LOCKED 1
    #define NOT_LOCKED 0

    struct alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) PNode{
        std::atomic<PNode*> next_{nullptr};
        std::atomic<uint32_t> locked_{LOCKED};
        std::atomic<Priority_t> priority_{};
        char padding[BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE-sizeof(next_)-sizeof(locked_)-sizeof(priority_)];
    };

    template<size_t N = 1>
    requires (N >= 1 && N <= BOOST_FAIRNESS_MAXIMUM_PRIORITY)
    class mcs_priority_spinlock{

        public:

        // this is still wrong there is a chance for 2 threads to both acquire as find_first_priority_ can return BOOST_FAIRNESS_INVALID_PRIORITY for 2 threads
        // the definition of predecessor and priorityPredecessor has to be merged //
        // seems to be working now but the question now is: would the implementation with a single tail be better?
        void acquire(PNode* node){

            repeat:

            Priority_t firstPriority = find_first_priority_();

            PNode* predecessor = tails[node->priority_.load(std::memory_order_relaxed)].exchange(node, std::memory_order_relaxed);

            PNode* priorityPredecessor = nullptr;

            PNode* noHead = nullptr;

            heads[node->priority_.load(std::memory_order_relaxed)].compare_exchange_strong(noHead, node);

            if (firstPriority != BOOST_FAIRNESS_INVALID_PRIORITY)
                priorityPredecessor = tails[firstPriority].load();

            if (priorityPredecessor == node)
                goto exit;

            if ((predecessor != nullptr) | (priorityPredecessor != nullptr)){

                if (predecessor != nullptr)
                    predecessor->next_.store(node, std::memory_order_release);//

                while (node->locked_.load(std::memory_order_acquire) == LOCKED)
                    pause(); // TODO Change with spinwait

                //
                goto exit;

            }

            if (firstPriority == BOOST_FAIRNESS_INVALID_PRIORITY && firstPriority != find_first_priority_())
                goto repeat;

            exit:

            heads[node->priority_.load(std::memory_order_relaxed)].exchange(node->next_);
        }

        void release(PNode *node) {

            PNode* localNode = nullptr;

            PNode* successor = get_successor_();

            if (successor == nullptr) {
                do 
                {
                    localNode = node;

                    if (tails[node->priority_.load()].compare_exchange_strong(localNode, nullptr, std::memory_order_release, std::memory_order_relaxed) &&
                        find_first_priority_waiters_() == BOOST_FAIRNESS_INVALID_PRIORITY)
                        return;

                    successor = get_successor_();
                } 
                while (successor == nullptr);
            }

            // now we must update all the tails not just the case we are the last one as an exit condition
            // also I do not want to risk to change node so maybe use expected also here
            localNode = node;

            tails[node->priority_.load()].compare_exchange_strong(localNode, nullptr, std::memory_order_release, std::memory_order_relaxed);

            successor->locked_.store(NOT_LOCKED, std::memory_order_release);
        }

        private:

        PNode* get_successor_(){
            Priority_t firstPriority = find_first_priority_waiters_();
            return heads[firstPriority].load();
        }

        Priority_t inline find_first_priority_(){
            for (Priority_t i = 0; i < N; ++i){
                if (tails[i].load() != nullptr)
                    return i;
            }
            return BOOST_FAIRNESS_INVALID_PRIORITY;
        }

        Priority_t inline find_first_priority_waiters_(){
            for (Priority_t i = 0; i < N; ++i){
                if (heads[i].load() != nullptr)
                    return i;
            }
            return BOOST_FAIRNESS_INVALID_PRIORITY;
        }

        std::array<std::atomic<PNode*>, N> heads{nullptr};
        std::array<std::atomic<PNode*>, N> tails{nullptr};

    };

    #undef LOCKED
    #undef NOT_LOCKED
}
#endif // BOOST_FAIRNESS_MCS_PRIORITY_LOCK_HPP
