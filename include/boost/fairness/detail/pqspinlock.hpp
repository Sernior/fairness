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
#include <array>
#include <atomic>

namespace boost::fairness::detail{

    struct PQNode{
        Request r;
        Thread t;
        std::atomic_flag inUse{};
    };

    static std::array<PQNode, BOOST_FAIRNESS_MAX_PQNODES> pqnodes;

    void inline find_first_free_pqnode(){
        for (int i = 0; i < BOOST_FAIRNESS_MAX_PQNODES; ++i){
            if (!pqnodes[i].inUse.test()){
                return i;
            }
        }
        return BOOST_FAIRNESS_INVALID_PQNODE_INDEX;
    }

    class pqspinlock{// should I be using 4 pqnodes per mutex? Or the entry PQNode should be separate probably.

        public:

        pqspinlock()
        : firstTail_()
        , cpl_(firstTail_) {}

        /// @private
        spinlock_priority_mutex(const spinlock_priority_mutex&) = delete;

        /// @private
        spinlock_priority_mutex& operator=(const spinlock_priority_mutex&) = delete;

        /// @private
        spinlock_priority_mutex(spinlock_priority_mutex&&) = delete;

        /// @private
        spinlock_priority_mutex& operator=(spinlock_priority_mutex&&) = delete;

        /// @private
        ~spinlock_priority_mutex() = default;

        void lock(Priority_t const priority = 0){
            
        }

        private:

        Request firstTail_;

        coherent_priority_lock cpl_;

    };

}
#endif // BOOST_FAIRNESS_PQSPINLOCK_HPP
