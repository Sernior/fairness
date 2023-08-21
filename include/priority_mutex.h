//
// Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <stdexcept>
#include "priority_t.h"

namespace PrioSync{// the name has yet to be chosen

    template<Priority_t N = 1, typename = std::enable_if_t<(N >= 1 && N <= _max_priority)>>
    class priority_mutex{

        using Thread_cnt_t = uint32_t;
        struct threadPriority{
            Thread_cnt_t threads_waiting{};
            std::condition_variable thread_queue;
        };

        public:

        priority_mutex() = default;

        priority_mutex(const priority_mutex&) = delete;
        priority_mutex& operator=(const priority_mutex&) = delete;

        priority_mutex(priority_mutex&&) = delete;
        priority_mutex& operator=(priority_mutex&&) = delete;

        ~priority_mutex() = default;

        void lock(Priority_t priority = 0){

            if (priority >= N)// this check may be wasteful at runtime I might keep this as UB
                throw std::system_error(std::make_error_code(std::errc::invalid_argument));

            std::unique_lock<std::mutex> lock(_internalMtx);
            auto& myPriority = _priorities[priority];

            while (_lock_is_owned() || _find_first_priority(priority) < priority ){ 
                myPriority.threads_waiting++;
                myPriority.thread_queue.wait(lock);
                myPriority.threads_waiting--;
            }

            _owner = std::this_thread::get_id();
        }

        void unlock(){
            Priority_t p;

            {
            std::lock_guard<std::mutex> lock(_internalMtx);
            if (!_lock_is_owned_by_me())return;// this check may be wasteful yes we define a behavior that otherwise would be undefined by at the cost of 1 check which also call a std::thread::id constructor
            _owner = std::thread::id();
            p = _find_first_priority();
            }

            if (p != _max_priority)
                _priorities[p].thread_queue.notify_one();

        }

        [[nodiscard]] bool try_lock(Priority_t priority = 0){

            if (priority >= N)// this check may be wasteful at runtime I might keep this as UB
                throw std::system_error(std::make_error_code(std::errc::invalid_argument));

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
