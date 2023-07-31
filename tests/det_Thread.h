#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>

/*
This class is needed to test the fair_shared_mutex... we have to test this class... we have to test the tool used to test
isn`t it ironic?
*/

namespace det_thread_utils {
    enum class tick_tock_t{
        TICK,
        TOCK
    };

    class this_thread {
    public:
        this_thread() noexcept : mutex_(), tick_tock(), tick_tock_v(tick_tock_t::TOCK) {}

        void tock() {
            {
                std::unique_lock<std::mutex> lock(mutex_);
                tick_tock_v = tick_tock_t::TOCK;
            }
            tick_tock.notify_one();
        }

        void wait_for_tick(){
            std::unique_lock<std::mutex> lock(mutex_);
            while (tick_tock_v == tick_tock_t::TOCK) // while it is tock the other thread is going
                tick_tock.wait(lock);
        }

        std::mutex mutex_;
        std::condition_variable tick_tock;
        tick_tock_t tick_tock_v;
    };
}

class DeterministicThread {
public:
    template <typename Func, typename... Args>
    explicit DeterministicThread(Func&& func, Args&&... args, det_thread_utils::this_thread* t)
        : thread_(std::forward<Func>(func), std::forward<Args>(args)..., t), _this_thread(t) {}

    void join() {
        thread_.join();
    }

    void tick() {
        {
            std::unique_lock<std::mutex> lock(_this_thread->mutex_);
            _this_thread->tick_tock_v = det_thread_utils::tick_tock_t::TICK;
        }
        _this_thread->tick_tock.notify_one();
    }

    void wait_for_tock(){
        std::unique_lock<std::mutex> lock(_this_thread->mutex_);
        while (_this_thread->tick_tock_v == det_thread_utils::tick_tock_t::TICK)// while it is tick the other thread is going
            _this_thread->tick_tock.wait(lock);
    }

private:
    det_thread_utils::this_thread* _this_thread;
    std::thread thread_;
};
