/**
 * @file spinlock_shared_priority_mutex.h
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author P. Di Giglio
 * @author S. Martorana
 * @brief This file contains the implementation of the spinlock_shared_priority_mutex.
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
#include <boost/fairness/priority_t.hpp>
#include <mutex>

/*

THIS WILL BE DELETED AND ITS CODE WILL REPLACE THE shared_priority_mutex once it is completed WIP

*/

namespace boost::fairness{

    using Thr_cnt_t = uint32_t;

    /**
     * @brief The spinlock_shared_priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * 
     * @tparam N : number of 0 indexed priorities the spinlock_shared_priority_mutex manages, up to _max_priority.
     */
    template<size_t N = 1>
    requires (N >= 1 && N <= _max_priority)
    class spinlock_shared_priority_mutex{

        using Thread_cnt_t = uint32_t;

        public:

        /// @private
        spinlock_shared_priority_mutex() = default;

        /// @private
        spinlock_shared_priority_mutex(const spinlock_shared_priority_mutex&) = delete;

        /// @private
        spinlock_shared_priority_mutex& operator=(const spinlock_shared_priority_mutex&) = delete;

        /// @private
        spinlock_shared_priority_mutex(spinlock_shared_priority_mutex&&) = delete;

        /// @private
        spinlock_shared_priority_mutex& operator=(spinlock_shared_priority_mutex&&) = delete;

        /// @private
        ~spinlock_shared_priority_mutex() = default;

        /**
         * @brief Try to acquire the unique ownership of the spinlock_shared_priority_mutex, blocking the thread if the spinlock_shared_priority_mutex was already owned or other threads are waiting with higher priority.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * spinlock_shared_priority_mutex<10> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.lock(prio);
         *      //...some code.
         * }
         * \endcode
         */
        void lock(Priority_t const priority = 0){
            Thr_cnt_t localTotalCurrentReaders = tot_current_readers_.load(std::memory_order_relaxed);;
            internalMtx_.lock(priority);
            waiters_waiters_[priority]++;
            for (;;){

                if (
                    localTotalCurrentReaders == 0 && tot_current_readers_.compare_exchange_weak(localTotalCurrentReaders, 0, std::memory_order_relaxed, std::memory_order_relaxed) &&
                    lockOwned_.test_and_set(std::memory_order_relaxed)
                ){
                    waiters_waiters_[priority]--;
                    break;
                }

                internalMtx_.unlock();

                lockOwned_.wait(true);

                if (localTotalCurrentReaders != 0)
                    tot_current_readers_.wait(localTotalCurrentReaders);// maybe should be a flag

                internalMtx_.lock(priority);

            }
            internalMtx_.unlock();
            /*
            Priority_t localCurrentPriority = currentPriority_.load(std::memory_order_relaxed);
            Thr_cnt_t localTotalCurrentReaders = tot_current_readers_.load(std::memory_order_relaxed);
            waiters_[priority].fetch_add(1, std::memory_order_relaxed);
            waiters_waiters_[priority].fetch_add(1, std::memory_order_relaxed);
            while ( 
                (localTotalCurrentReaders != 0 || !tot_current_readers_.compare_exchange_weak(localTotalCurrentReaders, 0, std::memory_order_relaxed)) ||
                (localCurrentPriority < priority || !currentPriority_.compare_exchange_weak(localCurrentPriority, priority, std::memory_order_relaxed)) ||
                (lockOwned_.test_and_set(std::memory_order_acquire))
            ){
                lockOwned_.wait(true);
                if (localTotalCurrentReaders != 0)
                    tot_current_readers_.wait(localTotalCurrentReaders);
                localCurrentPriority = currentPriority_;
                localTotalCurrentReaders = tot_current_readers_;
            }
            waiters_waiters_[priority].fetch_sub(1, std::memory_order_relaxed);
            waiters_[priority].fetch_sub(1, std::memory_order_relaxed);*/
        }

        /**
         * @brief Release the spinlock_shared_priority_mutex from unique ownership.
         * 
         * \code{.cpp}
         * spinlock_shared_priority_mutex<10> m;
         * 
         * void my_function() {
         *      //...some code.
         *      m.unlock();
         *      //...some code.
         * }
         * \endcode
         */
        void unlock(){
            currentPriority_.store(find_first_priority_(), std::memory_order_relaxed);
            lockOwned_.clear(std::memory_order_release);
            lockOwned_.notify_all();//P2616R3 https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2616r3.html this shouldnt be a problem with mutex semantics
        }

        /**
         * @brief Try to acquire the unique ownership of the spinlock_shared_priority_mutex, if successful will return true, false otherwise.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * spinlock_shared_priority_mutex<10> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.try_lock(prio);
         *      //...some code.
         * }
         * \endcode
         * @return bool 
         */
        [[nodiscard]] bool try_lock(Priority_t const priority = 0){ // friend spinlock_priority
            return (tot_current_readers_.load(std::memory_order_relaxed) == 0 && currentPriority_.load(std::memory_order_relaxed) >= priority && !lockOwned_.test_and_set(std::memory_order_acquire));
        }

        /**
         * @brief Try to acquire the shared ownership of the shared_priority_mutex, blocking the thread if the shared_priority_mutex was already uniquely owned or if another thread is waiting for unique ownership with higher priority.
         * 
         * @param priority used to set a priority for this thread to aquire the lock_shared.
         * 
         * \code{.cpp}
         * spinlock_shared_priority_mutex<10> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.lock_shared(prio);
         *      //...some code.
         * }
         * \endcode
         */
        void lock_shared(Priority_t priority = 0){
            Priority_t localMinWriterPriority = minWriterPriority_.load(std::memory_order_relaxed);
            while (
                (localMinWriterPriority < priority) ||
                lockOwned_.test(std::memory_order_acquire)
            ){
                
            }
        }

        /**
         * @brief Release the shared_priority_mutex from shared ownership.
         * 
         * \code{.cpp}
         * spinlock_shared_priority_mutex<10> m;
         * 
         * void my_function() {
         *      //...some code.
         *      m.unlock_shared();
         *      //...some code.
         * }
         * \endcode
         */
        void unlock_shared(){
                tot_current_readers_.fetch_sub(1);
                minWriterPriority_.store(_find_first_priority_with_writers());
                minWriterPriority_.notify_all();
                tot_current_readers_.notify_all();
        }

        /**
         * @brief Try to acquire the shared ownership of the shared_priority_mutex, if successful will return true, false otherwise.
         * 
         * @param priority used to set a priority for this thread to aquire the lock_shared.
         * 
         * \code{.cpp}
         * spinlock_shared_priority_mutex<10> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.try_lock_shared(prio);
         *      //...some code.
         * }
         * \endcode
         * @return bool 
         */
        [[nodiscard]] bool try_lock_shared(Priority_t priority = 0){
            tot_current_readers_.fetch_add(1);
            bool ret = minWriterPriority_ >= priority && !lockOwned_.test();
            if (!ret) tot_current_readers_.fetch_sub(1);
            return ret;
        }

        private:
        std::array<Thr_cnt_t, N> writer_waiters_;
        std::atomic<Thr_cnt_t> tot_current_readers_;//
        std::atomic<Priority_t> minWriterPriority_{_max_priority};//
        std::atomic_flag lockOwned_;//

        spinlock_priority_mutex<N> internalMtx_;


        Priority_t _find_first_priority_with_writers(){
            for (Priority_t i = 0; i <  N; i++){
                if (_priorities[i].writer_waiters_ > 0)
                    return i;
            }
            return _max_priority;
        }

    };
}

