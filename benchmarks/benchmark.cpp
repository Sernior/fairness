#include <boost/fairness.hpp>
#include <benchmark/benchmark.h>
#include <mutex>
#include <shared_mutex>
#include "pipeline_benchmark.hpp"
#include <DeterministicConcurrency>


static void PM_LockUnlock(benchmark::State& state) {
    boost::fairness::priority_mutex m;
    for (auto _ : state){
        m.lock();
        m.unlock();
    }
}

static void SPNLC_PM_LockUnlock(benchmark::State& state) { /* much better for spinlocking on linux than the slim version */
    boost::fairness::spinlock_priority_mutex m;
    for (auto _ : state){
        m.lock();
        m.unlock();
    }
}

static void SLMPM_LockUnlock(benchmark::State& state) {
    boost::fairness::slim_priority_mutex<7> m;
    for (auto _ : state){
        m.lock();
        m.unlock();
    }
}
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
static void SPNLC_SLM_PM_LockUnlock(benchmark::State& state) { /* the non slim version is better for spinlocking */
    boost::fairness::experimental::slim_spinlock_priority_mutex<7> m;
    for (auto _ : state){
        m.lock();
        m.unlock();
    }
}
#endif

static void R_PM_LockUnlock(benchmark::State& state) {
    boost::fairness::recursive_priority_mutex<7> m;
    for (auto _ : state){
        m.lock();
        m.unlock();
    }
}
static void STD_LockUnlock(benchmark::State& state) {
    std::mutex m;
    for (auto _ : state){
        m.lock();
        m.unlock();
    }
}

static void PM_S_LockUnlock(benchmark::State& state) {
    boost::fairness::shared_priority_mutex<10> m;
    for (auto _ : state){
        m.lock();
        m.unlock();
    }
}

static void STD_S_LockUnlock(benchmark::State& state) {
    std::shared_mutex m;
    for (auto _ : state){
        m.lock();
        m.unlock();
    }
}

static void PM_S_SLockSUnlock(benchmark::State& state) {
    boost::fairness::shared_priority_mutex<10> m;
    for (auto _ : state){
        m.lock_shared();
        m.unlock_shared();
    }
}

static void STD_S_SLockSUnlock(benchmark::State& state) {
    std::shared_mutex m;
    for (auto _ : state){
        m.lock_shared();
        m.unlock_shared();
    }
}

static void PM_pipeline_benchmark_long(benchmark::State& state) {// order of 1/10th of a second (PM faster)
    std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
    std::array<int, 8> preCT {20, 15, 20, 30, 10, 5, 5, 20};
    int CT = 10;
    std::array<int, 8> postCT {50, 30, 20, 25, 10, 15, 15, 45};

    for (auto _ : state) {
        _PM_pipeline_benchmark::thread_function(prios[state.thread_index()] ,preCT[state.thread_index()], CT, postCT[state.thread_index()]);
    }
}

static void STD_pipeline_benchmark_long(benchmark::State& state) {
    std::array<int, 8> preCT {20, 15, 20, 30, 10, 5, 5, 20};
    int CT = 10;
    std::array<int, 8> postCT {50, 30, 20, 25, 10, 15, 15, 45};

    for (auto _ : state) {
        _STD_pipeline_benchmark::thread_function(preCT[state.thread_index()], CT, postCT[state.thread_index()]);
    }
}

static void PM_pipeline_benchmark_gaming(benchmark::State& state) {// order of 10 to 15 milliseconds (PM faster)
    std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
    std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
    int CT = 1000;
    std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

    for (auto _ : state) {
        _PM_pipeline_benchmark::thread_function_micro(prios[state.thread_index()] ,preCT[state.thread_index()], CT, postCT[state.thread_index()]);
    }
}

static void STD_pipeline_benchmark_gaming(benchmark::State& state) {
    std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
    int CT = 1000;
    std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

    for (auto _ : state) {
        _STD_pipeline_benchmark::thread_function_micro(preCT[state.thread_index()], CT, postCT[state.thread_index()]);
    }
}

static void PM_pipeline_benchmark_audio(benchmark::State& state) {// order of 1 to 1.5 millisec (PM faster)
    std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
    std::array<int, 8> preCT {200, 150, 200, 300, 100, 50, 50, 200};
    int CT = 100;
    std::array<int, 8> postCT {500, 300, 200, 250, 100, 150, 150, 450};

    for (auto _ : state) {
        _PM_pipeline_benchmark::thread_function_micro(prios[state.thread_index()] ,preCT[state.thread_index()], CT, postCT[state.thread_index()]);
    }
}

static void STD_pipeline_benchmark_audio(benchmark::State& state) {
    std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
    std::array<int, 8> preCT {200, 150, 200, 300, 100, 50, 50, 200};
    int CT = 100;
    std::array<int, 8> postCT {500, 300, 200, 250, 100, 150, 150, 450};

    for (auto _ : state) {
        _STD_pipeline_benchmark::thread_function_micro(preCT[state.thread_index()], CT, postCT[state.thread_index()]);
    }
}
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
static void SPNLC_SLM_PM_pipeline_benchmark_audio(benchmark::State& state) {
    std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
    std::array<int, 8> preCT {200, 150, 200, 300, 100, 50, 50, 200};
    int CT = 100;
    std::array<int, 8> postCT {500, 300, 200, 250, 100, 150, 150, 450};

    for (auto _ : state) {
      _SPNLC_SLM_PM_pipeline_benchmark::thread_function_micro(prios[state.thread_index()] ,preCT[state.thread_index()], CT, postCT[state.thread_index()]);
    }
}
#endif
static void SLM_pipeline_benchmark_audio(benchmark::State& state) {
    std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
    std::array<int, 8> preCT {200, 150, 200, 300, 100, 50, 50, 200};
    int CT = 100;
    std::array<int, 8> postCT {500, 300, 200, 250, 100, 150, 150, 450};

    for (auto _ : state) {
      _SLM_PM_pipeline_benchmark::thread_function_micro(prios[state.thread_index()] ,preCT[state.thread_index()], CT, postCT[state.thread_index()]);
    }
}

static void STD_pipeline_benchmark_fast(benchmark::State& state) { /*  */
    std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
    std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
    int CT = 1000;
    std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

    for (auto _ : state) {
      _STD_pipeline_benchmark::thread_function_nano(preCT[state.thread_index()], CT, postCT[state.thread_index()]);
    }

}

static void PM_pipeline_benchmark_fast(benchmark::State& state) {
    std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
    std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
    int CT = 1000;
    std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

    for (auto _ : state) {
      _PM_pipeline_benchmark::thread_function_nano(prios[state.thread_index()] ,preCT[state.thread_index()], CT, postCT[state.thread_index()]);
    }

}

static void SLM_pipeline_benchmark_fast(benchmark::State& state) {
    std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
    std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
    int CT = 1000;
    std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

    for (auto _ : state) {
      _SLM_PM_pipeline_benchmark::thread_function_nano(prios[state.thread_index()] ,preCT[state.thread_index()], CT, postCT[state.thread_index()]);
    }

}

#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
static void SPNLC_SLM_pipeline_benchmark_fast(benchmark::State& state) {
    std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
    std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
    int CT = 1000;
    std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

    for (auto _ : state) {
      _SPNLC_SLM_PM_pipeline_benchmark::thread_function_nano(prios[state.thread_index()] ,preCT[state.thread_index()], CT, postCT[state.thread_index()]);
    }

}
#endif

static void SPNLC_PM_pipeline_benchmark_fast(benchmark::State& state) {
    std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
    std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
    int CT = 1000;
    std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

    for (auto _ : state) {
      _SPNLC_PM_pipeline_benchmark::thread_function_nano(prios[state.thread_index()] ,preCT[state.thread_index()], CT, postCT[state.thread_index()]);
    }

}

BENCHMARK(PM_LockUnlock)->Threads(8);
BENCHMARK(STD_LockUnlock)->Threads(8);
BENCHMARK(SLMPM_LockUnlock)->Threads(8);
BENCHMARK(SPNLC_PM_LockUnlock)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
BENCHMARK(SPNLC_SLM_PM_LockUnlock)->Threads(8);
#endif
BENCHMARK(R_PM_LockUnlock)->Threads(8);
BENCHMARK(PM_S_LockUnlock)->Threads(8);
BENCHMARK(STD_S_LockUnlock)->Threads(8);

BENCHMARK(PM_S_SLockSUnlock)->Threads(8);
BENCHMARK(STD_S_SLockSUnlock)->Threads(8);

BENCHMARK(PM_pipeline_benchmark_long)->Threads(8);
BENCHMARK(STD_pipeline_benchmark_long)->Threads(8);

BENCHMARK(PM_pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(STD_pipeline_benchmark_gaming)->Threads(8);

BENCHMARK(PM_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(STD_pipeline_benchmark_audio)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
BENCHMARK(SPNLC_SLM_PM_pipeline_benchmark_audio)->Threads(8);
#endif
BENCHMARK(SLM_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(PM_pipeline_benchmark_fast)->Threads(8);

BENCHMARK(SLM_pipeline_benchmark_fast)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
BENCHMARK(SPNLC_SLM_pipeline_benchmark_fast)->Threads(8);
#endif
BENCHMARK(SPNLC_PM_pipeline_benchmark_fast)->Threads(8);
BENCHMARK(STD_pipeline_benchmark_fast)->Threads(8);

BENCHMARK_MAIN();