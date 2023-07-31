#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>

/*
This class is needed to test the fair_shared_mutex... we have to test this class... we have to test the tool used to test
isn`t it ironic?
*/

namespace det_thread_utils {
    class this_thread {
    public:
        explicit this_thread() : mutex_(), tick_tock() {}

        void tock() {
            {
                std::unique_lock<std::mutex> lock(mutex_);
                didTock = true;
            }
            tick_tock.notify_one();
        }

        void wait_for_tick(){
            std::unique_lock<std::mutex> lock(mutex_);
            while (!didTick)
                tick_tock.wait(lock);
        }

        std::mutex mutex_;
        std::condition_variable tick_tock;
        bool didTick{};
        bool didTock{};
    };
}

class DeterministicThread {
public:
    template <typename Func, typename... Args>
    explicit DeterministicThread(Func&& func, Args&&... args, det_thread_utils::this_thread& t)
        : thread_(std::forward<Func>(func), std::forward<Args>(args)...), _this_thread(t) {}

    void join() {
        thread_.join();
    }

    void tick() {
        {
            std::unique_lock<std::mutex> lock(_this_thread.mutex_);
            _this_thread.didTick = true;
        }
        _this_thread.tick_tock.notify_one();
    }

    void wait_for_tock(){
        std::unique_lock<std::mutex> lock(_this_thread.mutex_);
        while (!_this_thread.didTock)
            _this_thread.tick_tock.wait(lock);
    }

    det_thread_utils::this_thread _this_thread;

private:

    std::thread thread_;
};
