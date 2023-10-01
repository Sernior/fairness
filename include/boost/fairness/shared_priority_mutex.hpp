/**
 * @file shared_priority_mutex.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author P. Di Giglio
 * @author S. Martorana
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
#include <cstring>
#include <boost/fairness/priority_t.hpp>
#include <boost/fairness/spinlock_priority_mutex.hpp>

namespace boost::fairness{

    /**
     * @brief The shared_priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * 
     * @tparam N : number of 0 indexed priorities the shared_priority_mutex manages, up to _max_priority.
     */
    template<size_t N = 1>
    requires (N >= 1 && N <= _max_priority)
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
            intMtx_.lock(priority);
            ++priorities_[priority].writers_waiting;
            ++tot_writers_waiting_;
            for (;;){

                if (
                    tot_current_readers_ == 0 &&
                    !lockOwned_ &&
                    find_first_priority_() >= priority

                ){
                    --priorities_[priority].writers_waiting;
                    --tot_writers_waiting_;
                    lockOwned_ = true;
                    intMtx_.unlock();
                    return;
                }

                intMtx_.unlock();

                writer_waiting_flag[priority].wait(false);

                intMtx_.lock(priority);

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

            intMtx_.lock();

            lockOwned_ = false;

            p = find_first_priority_();

            if (p == _max_priority){
                intMtx_.unlock();
                return;
            }

            if (tot_writers_waiting_ == 0){
                allow_all_readers_();
                intMtx_.unlock();
                notify_all_readers_();
                return;
            }

            reset_(p);

            intMtx_.unlock();

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

            intMtx_.lock(priority);

            if (lockOwned_ ||
            tot_current_readers_ > 0 ||
            find_first_priority_() < priority){

                intMtx_.unlock();

                return false;
            }

            lockOwned_ = true;

            intMtx_.unlock();

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
            intMtx_.lock(priority);

            ++priorities_[priority].readers_waiting;

            for(;;){

                if (!lockOwned_ && find_first_priority_with_writers_() >= priority){
                    ++tot_current_readers_;
                    --priorities_[priority].readers_waiting;
                    intMtx_.unlock();
                    return;
                }

                intMtx_.unlock();

                reader_waiting_flag[priority].wait(false);

                intMtx_.lock(priority);

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

            intMtx_.lock();

            --tot_current_readers_;

            p = find_first_priority_();

            if (p == _max_priority){
                intMtx_.unlock();
                return;
            }

            if (tot_writers_waiting_ == 0){
                allow_all_readers_();
                intMtx_.unlock();
                notify_all_readers_();
                return;
            }

            if (tot_current_readers_ == 0){
                reset_(p);
                intMtx_.unlock();
                notify_priority_(p);
                return;
            }

            intMtx_.unlock();

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
            intMtx_.lock(priority);

            if (lockOwned_ || find_first_priority_with_writers_() < priority){

                intMtx_.unlock();

                return false;
            }

            ++tot_current_readers_;

            intMtx_.unlock();

            return true;
        }

        private:

        spinlock_priority_mutex<N> intMtx_;
        std::array<threadPriority, N> priorities_;
        std::array<std::atomic_flag, N> writer_waiting_flag;
        std::array<std::atomic_flag, N> reader_waiting_flag;
        Thread_cnt_t tot_current_readers_{};
        Thread_cnt_t tot_writers_waiting_{};
        bool lockOwned_{};

        Priority_t find_first_priority_() const {
            for (Priority_t i = 0; i <  N; ++i){
                if (priorities_[i].writers_waiting+priorities_[i].readers_waiting > 0)
                    return i;
            }
            return _max_priority;
        }

        Priority_t find_first_priority_with_writers_() const {
            for (Priority_t i = 0; i <  N; ++i){
                if (priorities_[i].writers_waiting > 0)
                    return i;
            }
            return _max_priority;
        }

        void notify_all_readers_(){
            std::memset(&reader_waiting_flag, 0b00000001, N);
            for (Priority_t i = 0; i < N; ++i)
                reader_waiting_flag[i].notify_all();
        }

        void notify_priority_(Priority_t const p){
            writer_waiting_flag[p].notify_one();
            reader_waiting_flag[p].notify_all();
        }

        void reset_(Priority_t const p){
            std::memset(&writer_waiting_flag, 0b00000000, N);
            std::memset(&reader_waiting_flag, 0b00000000, N);
            writer_waiting_flag[p].test_and_set();
            reader_waiting_flag[p].test_and_set();
        }

        void allow_all_readers_(){
            std::memset(&writer_waiting_flag, 0b00000000, N);
            std::memset(&reader_waiting_flag, 0b00000001, N);
        }

    };
}
#endif // BOOST_FAIRNESS_SHARED_PRIORITY_MUTEX_HPP
