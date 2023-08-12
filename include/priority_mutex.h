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

            if (priority >= N)
                throw std::system_error(std::make_error_code(std::errc::invalid_argument));

            std::unique_lock<std::mutex> lock(_internalMtx);
            auto& myPriority = _priorities[priority];

            while (_lockOwned || _find_first_priority(priority) < priority ){ 
                myPriority.threads_waiting++;
                myPriority.thread_queue.wait(lock);
                myPriority.threads_waiting--;
            }

            _lockOwned = true;
        }

        void unlock(){
            Priority_t p;

            {
            std::lock_guard<std::mutex> lock(_internalMtx);
            _lockOwned = false;
            p = _find_first_priority();
            }

            if (p != _max_priority)
                _priorities[p].thread_queue.notify_one();

        }

        [[nodiscard]] bool try_lock(Priority_t priority = 0){

            if (priority >= N)
                throw std::system_error(std::make_error_code(std::errc::invalid_argument));

            std::lock_guard<std::mutex> lock(_internalMtx);
            auto max_p = _find_first_priority(priority);
            if (_lockOwned || max_p < priority)
                return false;
            return _lockOwned = true;
        }


        private:
        std::mutex _internalMtx;
        std::array<threadPriority, N> _priorities;
        bool _lockOwned{};

        Priority_t _find_first_priority(Priority_t priority = _max_priority){
            for (Priority_t i = 0; i < ((priority == _max_priority) ? N : priority); i++){
                if (_priorities[i].threads_waiting > 0)
                    return i;
            }
            return _max_priority;
        }

    };
}