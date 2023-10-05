#include "../utils/waiting_utils.hpp"

namespace _STD_S_pipeline_benchmark{

    std::shared_mutex m;

    void thread_function(int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_milli(preCriticalTime);
        m.lock();
        utils::waiting::busy_wait_milli(criticalTime);
        m.unlock();
        utils::waiting::busy_wait_milli(postCriticalTime);
    }

    void thread_function_micro(int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_micro(preCriticalTime);
        m.lock();
        utils::waiting::busy_wait_micro(criticalTime);
        m.unlock();
        utils::waiting::busy_wait_micro(postCriticalTime);
    }

    void thread_function_nano(int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_nano(preCriticalTime);
        m.lock();
        utils::waiting::busy_wait_nano(criticalTime);
        m.unlock();
        utils::waiting::busy_wait_nano(postCriticalTime);
    }

    void STD_S_LockUnlock(benchmark::State& state) {
        for (auto _ : state){
            m.lock();
            m.unlock();
        }
    }

    void STD_S_SLockSUnlock(benchmark::State& state) {
        for (auto _ : state){
            m.lock_shared();
            m.unlock_shared();
        }
    }

    void STD_S_pipeline_benchmark_long(benchmark::State& state) {
        std::array<int, 8> preCT {20, 15, 20, 30, 10, 5, 5, 20};
        int CT = 10;
        std::array<int, 8> postCT {50, 30, 20, 25, 10, 15, 15, 45};

        for (auto _ : state) {
            thread_function(preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }

    void STD_S_pipeline_benchmark_gaming(benchmark::State& state) {
        std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
        int CT = 1000;
        std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

        for (auto _ : state) {
            thread_function_micro(preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }

    void STD_S_pipeline_benchmark_audio(benchmark::State& state) {
        std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
        std::array<int, 8> preCT {200, 150, 200, 300, 100, 50, 50, 200};
        int CT = 100;
        std::array<int, 8> postCT {500, 300, 200, 250, 100, 150, 150, 450};

        for (auto _ : state) {
            _STD_pipeline_benchmark::thread_function_micro(preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }

    void STD_S_pipeline_benchmark_fast(benchmark::State& state) { /*  */
        std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
        std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
        int CT = 1000;
        std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

        for (auto _ : state) {
        _STD_pipeline_benchmark::thread_function_nano(preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }
}