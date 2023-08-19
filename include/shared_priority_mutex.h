/**
 * @file shared_priority_mutex.h
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author P. Di Giglio
 * @author S. Martorana
 * @brief #TODO
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
         * @brief #TODO like a `std::mutex.lock()`, plus a priority.
         * 
         * @param priority used to set a priority of a lock.
         * 
         * \code{.cpp}
         * void my_function(PrioSync::priority_mutex pm, uint8_t priority) {
         *     //...do something
         *     pm.lock(priority);
         *     //...do something
         * };
         * \endcode
         */
        void lock(Priority_t priority = 0){

            if (priority >= N)
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
         * @brief #TODO like a `std::mutex.unlock()`, plus a priority.
         * 
         * \code{.cpp}
         * void my_function(PrioSync::priority_mutex pm, uint8_t priority) {
         *     //...do something
         *     pm.lock(priority);
         *     //...do something
         *     pm.unlock();
         *     //...do something
         * };
         * \endcode
         */
        void unlock(){
            Priority_t p;

            {
            std::lock_guard<std::mutex> lock(_internalMtx);
            if (!_lock_is_owned_by_me())return;
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
         * @brief #TODO like a `std::mutex.try_lock()`, plus a priority.
         * 
         * @param priority used to set a priority of a lock.
         * 
         * \code{.cpp}
         * void my_function(PrioSync::priority_mutex pm, uint8_t priority) {
         *     //...do something
         *     pm.try_lock(priority);
         *     //...do something
         * };
         * \endcode
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool try_lock(Priority_t priority = 0){

            if (priority >= N)
                throw std::system_error(std::make_error_code(std::errc::invalid_argument));

            std::lock_guard<std::mutex> lock(_internalMtx);
            if (_lock_is_owned() || _totalCurrentReaders > 0 || _find_first_priority(priority) < priority)
                return false;
            _owner = std::this_thread::get_id();
            return true;
        }

        /**
         * @brief #TODO like a `std::mutex.lock()` in shared mode, plus a priority.
         * 
         * @param priority used to set a priority of a lock.
         * 
         * \code{.cpp}
         * void my_function(PrioSync::priority_mutex pm, uint8_t priority) {
         *     //...do something
         *     pm.shared_lock(priority);
         *     //...do something
         * };
         * \endcode
         */
        void lock_shared(Priority_t priority = 0){

            if (priority >= N)
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
         * @brief #TODO like a `std::mutex.try_lock()` in shared mode, plus a priority.
         * 
         * @param priority used to set a priority of a lock.
         * 
         * \code{.cpp}
         * void my_function(PrioSync::priority_mutex pm, uint8_t priority) {
         *     //...do something
         *     pm.try_lock_shared(priority);
         *     //...do something
         * };
         * \endcode
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool try_lock_shared(Priority_t priority = 0){

            if (priority >= N)
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
         * @brief #TODO like a `std::mutex.unlock()`, plus a priority.
         * 
         * \code{.cpp}
         * void my_function(PrioSync::priority_mutex pm, uint8_t priority) {
         *     //...do something
         *     pm.lock(priority);
         *     //...do something
         *     pm.unlock();
         *     //...do something
         * };
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
