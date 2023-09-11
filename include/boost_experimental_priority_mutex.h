/**
 * @file priority_mutex.h
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author P. Di Giglio
 * @author S. Martorana
 * @brief This file contains the implementation of the priority_mutex.
 * @version 0.1
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#pragma once
#include <thread>
// #include <atomic>
#include <boost/atomic.hpp>
#include <boost/atomic/atomic_flag.hpp>
#include <array>
#include <chrono>
#include <thread>
#include <mutex>
#include "priority_t.h"
namespace PrioSync{// the name has yet to be chosen

    //NO DWCAS

    struct b_control_block_t{ // templarize
        int8_t owned_ = 7;// first bit owned remaining 7 bit is the current priority
        uint8_t priority_[7];
    };

    static_assert(boost::atomic<b_control_block_t>::is_always_lock_free, "?");

    /**
     * @brief The priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * 
     * @tparam N : number of 0 indexed priorities the priority_mutex manages, up to _max_priority.
     */
    template<size_t N = 1>
    requires (N >= 1 && N <= _max_priority)
    class boost_experimental_priority_mutex{

        using Thread_cnt_t = uint32_t;

        public:

        /// @private
        boost_experimental_priority_mutex() = default;

        /// @private
        boost_experimental_priority_mutex(const boost_experimental_priority_mutex&) = delete;

        /// @private
        boost_experimental_priority_mutex& operator=(const boost_experimental_priority_mutex&) = delete;

        /// @private
        boost_experimental_priority_mutex(boost_experimental_priority_mutex&&) = delete;

        /// @private
        boost_experimental_priority_mutex& operator=(boost_experimental_priority_mutex&&) = delete;

        /// @private
        ~boost_experimental_priority_mutex() = default;

        /**
         * @brief Try to acquire the unique ownership of the priority_mutex, blocking the thread if the priority_mutex was already owned or other threads are waiting with higher priority.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * priority_mutex<10> m;
         * m.lock(9);
         * \endcode
         */
        void lock(Priority_t const priority = 0){

            b_control_block_t localCtrl = ctrl_.load();
            b_control_block_t localCtrlModified;

            for(;;){
                localCtrlModified = localCtrl;
                localCtrlModified.priority_[priority]++;
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrlModified)){
                    break;
                }
            }

            localCtrl = ctrl_.load();

            for(;;){
                localCtrlModified = localCtrl;
                localCtrlModified.owned_ |= 0b10000000; 
                if (!lockOwned_(localCtrl.owned_) && priority <= localCtrl.owned_ && ctrl_.compare_exchange_weak(localCtrl, localCtrlModified)){ //benchmark vs strong
                    break;
                }
                waiters_[priority].wait(false); // UB if the cew fails randomly we deadlock maybe we should use strong
                localCtrl = ctrl_.load();
            }

            localCtrl = ctrl_.load();

            for(;;){
                localCtrlModified = localCtrl;
                localCtrlModified.priority_[priority]--;
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrlModified)){
                    break;
                }
            }

        }

        /**
         * @brief Release the priority_mutex from unique ownership.
         * 
         * \code{.cpp}
         * priority_mutex<10> m;
         * m.lock(9);
         * m.unlock();
         * \endcode
         */
        void unlock(){
            Priority_t localFirstPriority;
            b_control_block_t localCtrl = ctrl_.load();
            b_control_block_t localCtrlModified;
            for (;;){
                reset_();
                localCtrlModified = localCtrl;
                localFirstPriority = find_first_priority_(localCtrl);
                localCtrlModified.owned_ = localFirstPriority;
                if (localFirstPriority < N){
                    waiters_[localFirstPriority].test_and_set();
                    waiters_[localFirstPriority].notify_one();
                }
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrlModified))
                    break;
            }
        }

        /**
         * @brief Try to acquire the unique ownership of the priority_mutex, if successful will return true, false otherwise.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * priority_mutex<10> m;
         * m.try_lock(9);
         * \endcode
         * @return bool 
         */
        [[nodiscard]] bool try_lock(Priority_t const priority = 0){

            return true;
            //return (currentPriority_.load(std::memory_order_relaxed) >= priority && !lockOwned_.test_and_set(std::memory_order_acquire));
        }

        private:
        std::array<boost::atomic_flag, N> waiters_;
        boost::atomic<b_control_block_t> ctrl_;
        //std::atomic<Priority_t> currentPriority_{_max_priority};
        //std::atomic_flag lockOwned_;

        void reset_(){
            for (Priority_t i = 0; i < N; i++)
                waiters_[i].clear();
        }

        Priority_t find_first_priority_(b_control_block_t const& ctrl){// try use a non atomic parameter instead
            for (Priority_t i = 0; i < N; i++){
                if (ctrl.priority_[i] > 0)
                    return i;
            }
            return 7;
        }

        bool lockOwned_(int8_t ctrl) const{
            return ctrl < 0;
        }

    };
}
