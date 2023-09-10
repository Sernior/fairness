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
#include <atomic>
#include <array>
#include <chrono>
#include <thread>
#include <mutex>

namespace PrioSync{// the name has yet to be chosen

    //NO DWCAS
    struct control_block_t{ // templarize
        int8_t owned_;// first bit owned remaining 7 bit is the current priority
        bool priority_[7];
    };
    using priority_t = uint8_t;
    static constexpr priority_t max_priority_ = 7; // NO DWCAS
    

    /**
     * @brief The priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * 
     * @tparam N : number of 0 indexed priorities the priority_mutex manages, up to _max_priority.
     */
    template<size_t N = 1>
    requires (N >= 1 && N <= _max_priority)
    class experimental_priority_mutex{

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
            control_block_t localCtrlOwned;
            waiters_[priority].fetch_add(1, std::memory_order_relaxed);
            for(;;){
                localCtrlOwned = localCtrl;
                localCtrlOwned.owned_ |= 0b10000000; 
                if (!lockOwned_(localCtrl.owned_) && priority <= localCtrl.owned_ && ctrl_.compare_exchange_weak(localCtrl, localCtrlOwned)){
                    break;
                }
                ((std::atomic_flag*)(&(ctrl_.load().priority_[priority])))->wait(false);
                localCtrl = ctrl_.load();
            }
            waiters_[priority].fetch_sub(1, std::memory_order_relaxed);
            /*
            Priority_t localCurrentPriority = currentPriority_.load(std::memory_order_relaxed);
            waiters_[priority].fetch_add(1, std::memory_order_relaxed);
            while ( 
                (localCurrentPriority < priority || !currentPriority_.compare_exchange_weak(localCurrentPriority, priority, std::memory_order_relaxed)) ||
                (lockOwned_.test_and_set(std::memory_order_acquire))
            ){
                lockOwned_.wait(true);
                localCurrentPriority = currentPriority_;
            }
            waiters_[priority].fetch_sub(1, std::memory_order_relaxed);
            */
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
            reset_();
            priority_t localFirstPriority = find_first_priority_();
            ((std::atomic<int8_t>*)(&(ctrl_.load().owned_))).store(localFirstPriority);
            ((std::atomic_flag*)(&(ctrl_.load().priority_[localFirstPriority])))->test_and_set();
            ((std::atomic_flag*)(&(ctrl_.load().priority_[localFirstPriority])))->notify_one();
            /*
            lockOwned_.clear(std::memory_order_relaxed);
            currentPriority_.store(find_first_priority_(), std::memory_order_release);
            lockOwned_.notify_all();//P2616R3 https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2616r3.html this shouldnt be a problem with mutex semantics
            */
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
        std::array<std::atomic<Thread_cnt_t>, N> waiters_;
        std::atomic<control_block_t> ctrl_;
        //std::atomic<Priority_t> currentPriority_{_max_priority};
        //std::atomic_flag lockOwned_;


        Priority_t find_first_priority_(){
            for (Priority_t i = 0; i < N; i++){
                if (waiters_[i] > 0)
                    return i;
            }
            return _max_priority;
        }

        bool lockOwned_(int8_t ctrl1) const{
            return ctrl1 < 0;
        }

        void reset_(){
            for (Priority_t i = 0; i < N; i++)
                ((std::atomic_flag*)(&(ctrl_.load().priority_[i])))->clear();
        }

    };
}
