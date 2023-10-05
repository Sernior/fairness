#include <boost/fairness.hpp>
#include "../utils/waiting_utils.hpp"

namespace _SPNLC_PM_pipeline_benchmark{

    boost::fairness::spinlock_priority_mutex<4> m;

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

    void SPNLC_PM_LockUnlock(benchmark::State& state) { /* much better for spinlocking on linux than the slim version */
        for (auto _ : state){
            m.lock();
            m.unlock();
        }
    }

    void SPNLC_PM_pipeline_benchmark_fast(benchmark::State& state) {
        std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
        std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
        int CT = 1000;
        std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

        for (auto _ : state) {
        thread_function_nano(prios[state.thread_index()] ,preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }
}