#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <stdexcept>

namespace PrioSync{// the name has yet to be chosen

    using Priority_t = uint8_t;
    static constexpr Priority_t _max_priority = Priority_t(-1);

    template<Priority_t N = 1, typename = std::enable_if_t<(N >= 1 && N <= _max_priority)>>
    class priority_mutex{

        using Thread_cnt_t = uint32_t;
        static constexpr Thread_cnt_t _Max_threads = Thread_cnt_t(-1);

        struct threadPriority{
            Thread_cnt_t waiting{};
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
            if (priority > N-1)
                throw std::system_error(std::make_error_code(std::errc::invalid_argument));

            std::unique_lock<std::mutex> lock(_internalMtx);
            auto& myPriority = _priorities[priority];

            while (_lockOwned || _find_first_priority() < priority ){ 
                myPriority.waiting++;
                myPriority.thread_queue.wait(lock);
                myPriority.waiting--;
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

            if (p == _max_priority) return;

            _priorities[p].thread_queue.notify_one();

        }

        [[nodiscard]] bool try_lock(Priority_t priority = 0){
            std::lock_guard<std::mutex> lock(_internalMtx);
            auto max_p = _find_first_priority();
            if (_lockOwned || max_p < priority)
                return false;
            return _lockOwned = true;
        }


        private:
        std::mutex _internalMtx;
        std::array<threadPriority, N> _priorities;
        bool _lockOwned{};

        Priority_t _find_first_priority(){
            for (Priority_t i = 0; i < N; i++){
                if (_priorities[i].waiting > 0)
                    return i;
            }
            return _max_priority;
        }

    };
}