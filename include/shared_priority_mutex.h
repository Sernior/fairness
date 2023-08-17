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

        shared_priority_mutex() = default;

        shared_priority_mutex(const shared_priority_mutex&) = delete;
        shared_priority_mutex& operator=(const shared_priority_mutex&) = delete;

        shared_priority_mutex(shared_priority_mutex&&) = delete;
        shared_priority_mutex& operator=(shared_priority_mutex&&) = delete;

        ~shared_priority_mutex() = default;

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

        [[nodiscard]] bool try_lock(Priority_t priority = 0){

            if (priority >= N)
                throw std::system_error(std::make_error_code(std::errc::invalid_argument));

            std::lock_guard<std::mutex> lock(_internalMtx);
            if (_lock_is_owned() || _totalCurrentReaders > 0 || _find_first_priority(priority) < priority)
                return false;
            _owner = std::this_thread::get_id();
            return true;
        }

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
