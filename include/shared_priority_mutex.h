/**
 * @file shared_priority_mutex.h
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
#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <stdexcept>
#include "priority_t.h"

namespace PrioSync{// the name has yet to be chosen

    /**
     * @brief The shared_priority_mutex is an advanced synchronization mechanism that enhances the traditional shared_mutex by introducing a priority-based approach.
     * 
     * @tparam N : number of 0 indexed priorities the shared_priority_mutex manages, up to _max_priority.
     */
    template<Priority_t N = 1, typename = std::enable_if_t<(N >= 1 && N <= _max_priority)>>
    class shared_priority_mutex{

        using Thread_cnt_t = uint32_t;
        static constexpr Thread_cnt_t _max_threads = Thread_cnt_t(-1);

        struct threadPriority{
            Thread_cnt_t writers_waiting{};
            Thread_cnt_t threads_waiting{};
            std::condition_variable thread_queue;
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
         * m.lock(9);
         * \endcode
         */
        void lock(Priority_t priority = 0){

            if (priority >= N)// this check may be wasteful at runtime I might keep this as UB
                throw std::system_error(std::make_error_code(std::errc::invalid_argument));

            std::unique_lock<std::mutex> lock(_internalMtx);
            auto& myPriority = _priorities[priority];

            while (_lock_is_owned() || _totalCurrentReaders > 0 || _find_first_priority(priority) < priority ){ 
                myPriority.threads_waiting++;
                myPriority.writers_waiting++;
                _totalWritersWaiting++;
                myPriority.thread_queue.wait(lock);
                myPriority.threads_waiting--;
                myPriority.writers_waiting--;
                _totalWritersWaiting--;
            }

            _owner = std::this_thread::get_id();
        }

        /**
         * @brief Release the shared_priority_mutex from unique ownership.
         * 
         * \code{.cpp}
         * shared_priority_mutex<10> m;
         * m.lock(9);
         * m.unlock();
         * \endcode
         */
        void unlock(){
            Priority_t p;

            {
            std::lock_guard<std::mutex> lock(_internalMtx);
            if (!_lock_is_owned_by_me())return;// this check may be wasteful yes we define a behavior that otherwise would be undefined by at the cost of 1 check which also call a std::thread::id constructor
            _owner = std::thread::id();
            p = _find_first_priority();
            }

            if (p == _max_priority)
                return;

            if (_totalWritersWaiting == 0){
                _notify_all();
                return;
            }

            _priorities[p].thread_queue.notify_all();


        }

        /**
         * @brief Try to acquire the unique ownership of the shared_priority_mutex, if successful will return true, false otherwise.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * shared_priority_mutex<10> m;
         * m.try_lock(9);
         * \endcode
         * @return bool 
         */
        [[nodiscard]] bool try_lock(Priority_t priority = 0){

            if (priority >= N)// this check may be wasteful at runtime I might keep this as UB
                throw std::system_error(std::make_error_code(std::errc::invalid_argument));

            std::lock_guard<std::mutex> lock(_internalMtx);
            if (_lock_is_owned() || _totalCurrentReaders > 0 || _find_first_priority(priority) < priority)
                return false;
            _owner = std::this_thread::get_id();
            return true;
        }

        /**
         * @brief Try to acquire the shared ownership of the shared_priority_mutex, blocking the thread if the shared_priority_mutex was already uniquely owned or if another thread is waiting for unique ownership with higher priority.
         * 
         * @param priority used to set a priority for this thread to aquire the lock_shared.
         * 
         * \code{.cpp}
         * shared_priority_mutex<10> m;
         * m.lock_shared(9);
         * \endcode
         */
        void lock_shared(Priority_t priority = 0){

            if (priority >= N)// this check may be wasteful at runtime I might keep this as UB
                throw std::system_error(std::make_error_code(std::errc::invalid_argument));

            std::unique_lock<std::mutex> lock(_internalMtx);
            auto& myPriority = _priorities[priority];

            while (_lock_is_owned() ||
                   _totalCurrentReaders == _max_threads ||
                   _find_first_priority_with_writers(priority) < priority )
            {
                myPriority.threads_waiting++;
                myPriority.thread_queue.wait(lock);
                myPriority.threads_waiting--;
            }

            _totalCurrentReaders++;
        }

        /**
         * @brief Try to acquire the shared ownership of the shared_priority_mutex, if successful will return true, false otherwise.
         * 
         * @param priority used to set a priority for this thread to aquire the lock_shared.
         * 
         * \code{.cpp}
         * shared_priority_mutex<10> m;
         * m.try_lock_shared(9);
         * \endcode
         * @return bool 
         */
        [[nodiscard]] bool try_lock_shared(Priority_t priority = 0){

            if (priority >= N)// this check may be wasteful at runtime I might keep this as UB
                throw std::system_error(std::make_error_code(std::errc::invalid_argument));

            std::lock_guard<std::mutex> lock(_internalMtx);
            if (_lock_is_owned() ||
                _totalCurrentReaders == _max_threads ||
                _find_first_priority_with_writers(priority) < priority)
                return false;
            _totalCurrentReaders++;
            return true;
        }

        /**
         * @brief Release the shared_priority_mutex from shared ownership.
         * 
         * \code{.cpp}
         * shared_priority_mutex<10> m;
         * m.lock_shared(9);
         * m.unlock_shared();
         * \endcode
         */
        void unlock_shared(){
            Priority_t p;

            {
            std::lock_guard<std::mutex> lock(_internalMtx);
            // here I need to find a way to say "you have previously taken the lock_shared" or we could leave this behavior
            // semaphore like if I can`t figure out a decent, not costly, way of imposing this condition.
            _totalCurrentReaders--;
            p = _find_first_priority();
            }

            if (p == _max_priority)
                return;

            if (_totalWritersWaiting == 0){
                _notify_all();
                return;
            }

            if (_totalCurrentReaders == 0){
                _priorities[p].thread_queue.notify_all();
            }

        }

        private:

        Thread_cnt_t _totalWritersWaiting{};
        Thread_cnt_t _totalCurrentReaders{};
        std::mutex _internalMtx;
        std::array<threadPriority, N> _priorities;
        std::thread::id _owner{};

        Priority_t _find_first_priority(Priority_t priority = _max_priority){
            for (Priority_t i = 0; i < ((priority == _max_priority) ? N : priority); i++){
                if (_priorities[i].threads_waiting > 0)
                    return i;
            }
            return _max_priority;
        }

        Priority_t _find_first_priority_with_writers(Priority_t priority = _max_priority){
            for (Priority_t i = 0; i < ((priority == _max_priority) ? N : priority); i++){
                if (_priorities[i].writers_waiting > 0)
                    return i;
            }
            return _max_priority;
        }

        void _notify_all(){
            for (auto& p : _priorities)
                p.thread_queue.notify_all();
        }

        bool _lock_is_owned_by_me(){
            return std::this_thread::get_id() == _owner;
        }

        bool _lock_is_owned(){
            return std::thread::id() != _owner;
        }

    };
}
