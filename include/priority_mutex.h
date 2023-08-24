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
#include <mutex>
#include <condition_variable>
#include <array>
#include <stdexcept>
#include "priority_t.h"

namespace PrioSync{// the name has yet to be chosen

    /**
     * @brief The priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * 
     * @tparam N : number of 0 indexed priorities the priority_mutex manages, up to _max_priority.
     */
    template<Priority_t N = 1, typename = std::enable_if_t<(N >= 1 && N <= _max_priority)>>
    class priority_mutex{

        using Thread_cnt_t = uint32_t;
        struct threadPriority{
            Thread_cnt_t threads_waiting{};
            std::condition_variable thread_queue;
        };

        public:

        /// @private
        priority_mutex() = default;

        /// @private
        priority_mutex(const priority_mutex&) = delete;

        /// @private
        priority_mutex& operator=(const priority_mutex&) = delete;

        /// @private
        priority_mutex(priority_mutex&&) = delete;

        /// @private
        priority_mutex& operator=(priority_mutex&&) = delete;

        /// @private
        ~priority_mutex() = default;

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
        void lock(Priority_t priority = 0){

            std::unique_lock<std::mutex> lock(_internalMtx);
            auto& myPriority = _priorities[priority];

            while (_lock_is_owned() || _find_first_priority(priority) < priority ){ 
                myPriority.threads_waiting++;
                myPriority.thread_queue.wait(lock);
                myPriority.threads_waiting--;
            }

            _owner = std::this_thread::get_id();
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
            Priority_t p;

            {
            std::lock_guard<std::mutex> lock(_internalMtx);
            _owner = std::thread::id();
            p = _find_first_priority();
            }

            if (p != _max_priority)
                _priorities[p].thread_queue.notify_one();

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
        [[nodiscard]] bool try_lock(Priority_t priority = 0){

            std::lock_guard<std::mutex> lock(_internalMtx);
            auto max_p = _find_first_priority(priority);
            if (_lock_is_owned() || max_p < priority)
                return false;
            _owner = std::this_thread::get_id();
            return true;
        }


        private:
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

        bool _lock_is_owned_by_me(){
            return std::this_thread::get_id() == _owner;
        }

        bool _lock_is_owned(){
            return std::thread::id() != _owner;
        }

    };
}
