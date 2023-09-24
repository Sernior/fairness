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

/*
    ACCEPTED AND RENAMED slim_priority_mutex.h marked for deletion
*/

#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
#ifndef EXPERIMENTAL_PRIORITY_MUTEX_HPP
#define EXPERIMENTAL_PRIORITY_MUTEX_HPP
#include <thread>
#include <atomic>
#include <array>
#include <chrono>
#include <thread>
#include <mutex>
#include <boost/fairness/priority_t.hpp>
#include <cstring>
namespace boost::fairness{

    /**
     * @brief The priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * 
     * @tparam N : number of 0 indexed priorities the priority_mutex manages, up to _max_priority.
     */
    template<size_t N = 1>
    requires (N >= 1 && N <= _max_priority)
    class experimental_priority_mutex{

            //NO DWCAS
        struct control_block_t{ 
            int8_t owned_ = 7;// first bit owned remaining 7 bit is the current priority
            uint8_t priority_[7];
            control_block_t setOwned() const {
                control_block_t new_ctrl = *this;
                new_ctrl.owned_ |= 0b10000000;
                return new_ctrl;
            }
            control_block_t increasePriority(Priority_t const priority) const {
                control_block_t new_ctrl = *this;
                new_ctrl.priority_[priority]++;
                return new_ctrl;
            }
            control_block_t decreasePriority(Priority_t const priority) const {
                control_block_t new_ctrl = *this;
                new_ctrl.priority_[priority]--;
                return new_ctrl;
            }
            control_block_t setPriority(Priority_t const priority) const {
                control_block_t new_ctrl = *this;
                new_ctrl.owned_ = priority;
                return new_ctrl;
            }
            Priority_t getPriority() const {
                return owned_ & 0b01111111;
            }
        };

        static_assert(std::atomic<control_block_t>::is_always_lock_free, "control_block_t is not lock free");

        using Thread_cnt_t = uint32_t;

        public:

        /// @private
        experimental_priority_mutex() = default;

        /// @private
        experimental_priority_mutex(const experimental_priority_mutex&) = delete;

        /// @private
        experimental_priority_mutex& operator=(const experimental_priority_mutex&) = delete;

        /// @private
        experimental_priority_mutex(experimental_priority_mutex&&) = delete;

        /// @private
        experimental_priority_mutex& operator=(experimental_priority_mutex&&) = delete;

        /// @private
        ~experimental_priority_mutex() = default;

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

            control_block_t localCtrl = ctrl_.load();

            for(;;){
                if (localCtrl.priority_[priority] == uint8_t(-1)){
                    localCtrl = ctrl_.load();
                    continue;
                }
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrl.increasePriority(priority))){
                    break;
                }
            }

            for(;;){
                localCtrl = ctrl_.load();
                if (!lockOwned_(localCtrl.owned_) && priority <= localCtrl.owned_ && ctrl_.compare_exchange_strong(localCtrl, localCtrl.setOwned())){
                    break;
                }
                waiters_[priority].wait(false);
            }

            localCtrl = ctrl_.load();

            for(;;){
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrl.decreasePriority(priority))){
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
            control_block_t localCtrl;
            for (;;){
                reset_();
                localCtrl = ctrl_.load();
                localFirstPriority = find_first_priority_(localCtrl);
                if (localFirstPriority < N){
                    waiters_[localFirstPriority].test_and_set();
                    waiters_[localFirstPriority].notify_one();
                }
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrl.setPriority(localFirstPriority)))
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
            control_block_t localCtrl = ctrl_.load();
            return !lockOwned_(localCtrl.owned_) && priority <= localCtrl.owned_ && ctrl_.compare_exchange_strong(localCtrl, localCtrl.setOwned());
        }

        private:
        std::array<std::atomic_flag, N> waiters_;
        std::atomic<control_block_t> ctrl_;

        void reset_(){ // there probably is a much better way to do this
            std::memset(&waiters_, 0b00000000, N); // maybe undefined because lock is reading on the waits? Should be ok
        }

        Priority_t find_first_priority_(control_block_t const& ctrl){// try use a non atomic parameter instead
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
#endif // EXPERIMENTAL_PRIORITY_MUTEX_HPP
#endif // BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
