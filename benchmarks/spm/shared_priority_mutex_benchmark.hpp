#include <boost/fairness.hpp>
#include "../utils/waiting_utils.hpp"


namespace _PM_S_pipeline_benchmark{

    boost::fairness::shared_priority_mutex<10> m;

    void thread_function(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_milli(preCriticalTime);
        m.lock(p);
        utils::waiting::busy_wait_milli(criticalTime);
        m.unlock();
        utils::waiting::busy_wait_milli(postCriticalTime);
    }

    void thread_function_micro(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_micro(preCriticalTime);
        m.lock(p);
        utils::waiting::busy_wait_micro(criticalTime);
        m.unlock();
        utils::waiting::busy_wait_micro(postCriticalTime);
    }

    void thread_function_nano(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_nano(preCriticalTime);
        m.lock(p);
        utils::waiting::busy_wait_nano(criticalTime);
        m.unlock();
        utils::waiting::busy_wait_nano(postCriticalTime);
    }

    void PM_S_LockUnlock(benchmark::State& state) {
        for (auto _ : state){
            m.lock();
            m.unlock();
        }
    }

    void PM_S_SLockSUnlock(benchmark::State& state) {
        for (auto _ : state){
            m.lock_shared();
            m.unlock_shared();
        }
    }

    void PM_pipeline_benchmark_long(benchmark::State& state) {// order of 1/10th of a second (PM faster)
        std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
        std::array<int, 8> preCT {20, 15, 20, 30, 10, 5, 5, 20};
        int CT = 10;
        std::array<int, 8> postCT {50, 30, 20, 25, 10, 15, 15, 45};

        for (auto _ : state) {
            thread_function(prios[state.thread_index()] ,preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }

    void PM_pipeline_benchmark_gaming(benchmark::State& state) {// order of 10 to 15 milliseconds (PM faster)
        std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
        std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
        int CT = 1000;
        std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

        for (auto _ : state) {
            thread_function_micro(prios[state.thread_index()] ,preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }

    void PM_pipeline_benchmark_audio(benchmark::State& state) {// order of 1 to 1.5 millisec (PM faster)
        std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
        std::array<int, 8> preCT {200, 150, 200, 300, 100, 50, 50, 200};
        int CT = 100;
        std::array<int, 8> postCT {500, 300, 200, 250, 100, 150, 150, 450};

        for (auto _ : state) {
            thread_function_micro(prios[state.thread_index()] ,preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }

    void PM_pipeline_benchmark_fast(benchmark::State& state) {
        std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
        std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
        int CT = 1000;
        std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

        for (auto _ : state) {
            thread_function_nano(prios[state.thread_index()] ,preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }
}