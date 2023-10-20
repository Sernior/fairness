/**
 * @file shared_priority_mutex.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of the shared_priority_mutex.
 * @version 0.1
 * @date 2023-08-19
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_SHARED_PRIORITY_MUTEX_HPP
#define BOOST_FAIRNESS_SHARED_PRIORITY_MUTEX_HPP
#include <array>
#include <atomic>
#include <boost/fairness/priority_t.hpp>
#include <boost/fairness/spinlock_priority_mutex.hpp>

namespace boost::fairness{

    /**
     * @brief The shared_priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * 
     * @tparam N : number of 0 indexed priorities the shared_priority_mutex manages, up to BOOST_FAIRNESS_MAXIMUM_PRIORITY.
     */
    template<size_t N = 1>
    requires (N >= 1 && N <= BOOST_FAIRNESS_MAXIMUM_PRIORITY)
    class shared_priority_mutex{

        using Thread_cnt_t = uint32_t;

        struct threadPriority{
            Thread_cnt_t writers_waiting{};
            Thread_cnt_t readers_waiting{};
        };

        public:

        /// @private
        shared_priority_mutex() = default;

        /// @private
        shared_priority_mutex(const shared_priority_mutex&) = delete;

        /// @private
        shared_priority_mutex& operator=(const shared_priority_mutex&) = delete;

        /// @private
        shared_priority_mutex(shared_priority_mutex&&) = delete;

        /// @private
        shared_priority_mutex& operator=(shared_priority_mutex&&) = delete;

        /// @private
        ~shared_priority_mutex() = default;

        /**
         * @brief Try to acquire the unique ownership of the shared_priority_mutex, blocking the thread if the shared_priority_mutex was already owned or other threads are waiting with higher priority.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * shared_priority_mutex<10> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.lock(prio);
         *      //...some code.
         * }
         * \endcode
         */
        void lock(Priority_t const priority = 0){

            internalMutex_.lock(priority);

            ++priorities_[priority].writers_waiting;

            ++totalWritersWaiting_;

            for (;;)
            {

                if (
                    totalCurrentReaders_ == 0 &&
                    !lockOwned_ &&
                    find_first_priority_() >= priority
                ){
                    --priorities_[priority].writers_waiting;

                    --totalWritersWaiting_;

                    lockOwned_ = true;

                    internalMutex_.unlock();

                    return;
                }

                internalMutex_.unlock();

                writerWaitingFlags_[priority].wait(false);

                internalMutex_.lock(priority);

            }
        }

        /**
         * @brief Release the shared_priority_mutex from unique ownership.
         * 
         * \code{.cpp}
         * shared_priority_mutex<10> m;
         * 
         * void my_function() {
         *      //...some code.
         *      m.unlock();
         *      //...some code.
         * }
         * \endcode
         */
        void unlock(){

            Priority_t p;

            internalMutex_.lock();

            lockOwned_ = false;

            p = find_first_priority_();

            if (p == BOOST_FAIRNESS_MAXIMUM_PRIORITY)
            {
                internalMutex_.unlock();

                return;
            }

            if (totalWritersWaiting_ == 0)
            {
                allow_all_readers_();

                internalMutex_.unlock();

                notify_all_readers_();

                return;
            }

            reset_(p);

            internalMutex_.unlock();

            notify_priority_(p);

        }

        /**
         * @brief Try to acquire the unique ownership of the shared_priority_mutex, if successful will return true, false otherwise.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * shared_priority_mutex<10> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.try_lock(prio);
         *      //...some code.
         * }
         * \endcode
         * @return bool 
         */
        [[nodiscard]] bool try_lock(Priority_t const priority = 0){

            internalMutex_.lock(priority);

            if (lockOwned_ ||
            totalCurrentReaders_ > 0 ||
            find_first_priority_() < priority)
            {

                internalMutex_.unlock();

                return false;
            }

            lockOwned_ = true;

            internalMutex_.unlock();

            return true;
        }

        /**
         * @brief Try to acquire the shared ownership of the shared_priority_mutex, blocking the thread if the shared_priority_mutex was already uniquely owned or if another thread is waiting for unique ownership with higher priority.
         * 
         * @param priority used to set a priority for this thread to aquire the lock_shared.
         * 
         * \code{.cpp}
         * shared_priority_mutex<10> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.lock_shared(prio);
         *      //...some code.
         * }
         * \endcode
         */
        void lock_shared(Priority_t priority = 0){
            internalMutex_.lock(priority);

            ++priorities_[priority].readers_waiting;

            for(;;){

                if (!lockOwned_ && find_first_priority_with_writers_() >= priority)
                {
                    ++totalCurrentReaders_;

                    --priorities_[priority].readers_waiting;

                    internalMutex_.unlock();

                    return;
                }

                internalMutex_.unlock();

                readerWaitingFlags_[priority].wait(false);

                internalMutex_.lock(priority);

            }
        }

        /**
         * @brief Release the shared_priority_mutex from shared ownership.
         * 
         * \code{.cpp}
         * shared_priority_mutex<10> m;
         * 
         * void my_function() {
         *      //...some code.
         *      m.unlock_shared();
         *      //...some code.
         * }
         * \endcode
         */
        void unlock_shared(){

            Priority_t p;

            internalMutex_.lock();

            --totalCurrentReaders_;

            p = find_first_priority_();

            if (p == BOOST_FAIRNESS_MAXIMUM_PRIORITY)
            {
                internalMutex_.unlock();

                return;
            }

            if (totalWritersWaiting_ == 0)
            {
                allow_all_readers_();

                internalMutex_.unlock();

                notify_all_readers_();

                return;
            }

            if (totalCurrentReaders_ == 0)
            {
                reset_(p);

                internalMutex_.unlock();

                notify_priority_(p);

                return;
            }

            internalMutex_.unlock();

        }

        /**
         * @brief Try to acquire the shared ownership of the shared_priority_mutex, if successful will return true, false otherwise.
         * 
         * @param priority used to set a priority for this thread to aquire the lock_shared.
         * 
         * \code{.cpp}
         * shared_priority_mutex<10> m;
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
            internalMutex_.lock(priority);

            if (lockOwned_ || find_first_priority_with_writers_() < priority){

                internalMutex_.unlock();

                return false;
            }

            ++totalCurrentReaders_;

            internalMutex_.unlock();

            return true;
        }

        private:

        alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) spinlock_priority_mutex<N> internalMutex_;
        std::array<threadPriority, N> priorities_;
        alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) std::array<std::atomic_flag, N> writerWaitingFlags_; // there should be a struct like threadPriority for the flags so I can align both with 1 alignas
        std::array<std::atomic_flag, N> readerWaitingFlags_; // I should also change these to be 4 bytes so other systems other than windows can also wait on this memory
         Thread_cnt_t totalCurrentReaders_{};
        Thread_cnt_t totalWritersWaiting_{};
        bool lockOwned_{};

        Priority_t find_first_priority_() const {
            for (Priority_t i = 0; i <  N; ++i){
                if (priorities_[i].writers_waiting+priorities_[i].readers_waiting > 0)
                    return i;
            }
            return BOOST_FAIRNESS_MAXIMUM_PRIORITY;
        }

        Priority_t find_first_priority_with_writers_() const {
            for (Priority_t i = 0; i <  N; ++i){
                if (priorities_[i].writers_waiting > 0)
                    return i;
            }
            return BOOST_FAIRNESS_MAXIMUM_PRIORITY;
        }

        void notify_all_readers_(){
            for (Priority_t i = 0; i < N; ++i){
                readerWaitingFlags_[i].test_and_set();
                readerWaitingFlags_[i].notify_all();
            }
        }

        void notify_priority_(Priority_t const p){
            writerWaitingFlags_[p].notify_one();
            readerWaitingFlags_[p].notify_all();
        }

        void reset_(Priority_t const p){
            for (Priority_t i = 0; i <  N; ++i){
                readerWaitingFlags_[i].clear();
                readerWaitingFlags_[i].clear();
            }
            writerWaitingFlags_[p].test_and_set();
            readerWaitingFlags_[p].test_and_set();
        }

        void allow_all_readers_(){
            for (Priority_t i = 0; i < N; ++i){
                readerWaitingFlags_[i].test_and_set();
                writerWaitingFlags_[i].clear();
            }
        }

    };
}
#endif // BOOST_FAIRNESS_SHARED_PRIORITY_MUTEX_HPP
