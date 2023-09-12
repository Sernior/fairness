#include <benchmark/benchmark.h>
#include <priority_mutex.h>
#include <shared_priority_mutex.h>
#include <mutex>
#include <shared_mutex>
#include "pipeline_benchmark.cpp"
#include <BS_thread_pool.hpp>
#include <DeterministicConcurrency>

#ifdef EXPERIMENTAL_MUTEXES
#include <experimental_priority_mutex.h>
#endif

static void PM_LockUnlock(benchmark::State& state) {
    PrioSync::priority_mutex<10> m;
    for (auto _ : state){
        m.lock();
        m.unlock();
    }
}
#ifdef EXPERIMENTAL_MUTEXES
static void EXPPM_LockUnlock(benchmark::State& state) {
    PrioSync::experimental_priority_mutex m;
    for (auto _ : state){
        m.lock();
        m.unlock();
    }
}
#endif
static void STD_LockUnlock(benchmark::State& state) {
    std::mutex m;
    for (auto _ : state){
        m.lock();
        m.unlock();
    }
}

static void PM_S_LockUnlock(benchmark::State& state) {
    PrioSync::shared_priority_mutex<10> m;
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
    PrioSync::shared_priority_mutex<10> m;
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
    BS::thread_pool pool(8);
    for (auto _ : state){
        pool.push_task(_PM_pipeline_benchmark::thread_function, 0, 20, 10, 50);
        pool.push_task(_PM_pipeline_benchmark::thread_function, 2, 15, 10, 30);
        pool.push_task(_PM_pipeline_benchmark::thread_function, 2, 20, 10, 20);
        pool.push_task(_PM_pipeline_benchmark::thread_function, 1, 30, 10, 25);
        pool.push_task(_PM_pipeline_benchmark::thread_function, 1, 25, 10, 30);
        pool.push_task(_PM_pipeline_benchmark::thread_function, 3, 10, 10, 10);
        pool.push_task(_PM_pipeline_benchmark::thread_function, 3, 5,  10, 15);
        pool.push_task(_PM_pipeline_benchmark::thread_function, 0, 20, 10, 45);
        pool.wait_for_tasks();
    }
}

static void STD_pipeline_benchmark_long(benchmark::State& state) {
    BS::thread_pool pool(8);
    for (auto _ : state){
        pool.push_task(_STD_pipeline_benchmark::thread_function, 20, 10, 50);
        pool.push_task(_STD_pipeline_benchmark::thread_function, 15, 10, 30);
        pool.push_task(_STD_pipeline_benchmark::thread_function, 20, 10, 20);
        pool.push_task(_STD_pipeline_benchmark::thread_function, 30, 10, 25);
        pool.push_task(_STD_pipeline_benchmark::thread_function, 25, 10, 30);
        pool.push_task(_STD_pipeline_benchmark::thread_function, 10, 10, 10);
        pool.push_task(_STD_pipeline_benchmark::thread_function, 5,  10, 15);
        pool.push_task(_STD_pipeline_benchmark::thread_function, 20, 10, 45);
        pool.wait_for_tasks();
    }
}

static void PM_pipeline_benchmark_gaming(benchmark::State& state) {// order of 10 to 15 milliseconds (PM faster)
    BS::thread_pool pool(8);
    for (auto _ : state){
        pool.push_task(_PM_pipeline_benchmark::thread_function_micro, 0, 2000, 1000, 5000);
        pool.push_task(_PM_pipeline_benchmark::thread_function_micro, 2, 1500, 1000, 3000);
        pool.push_task(_PM_pipeline_benchmark::thread_function_micro, 2, 2000, 1000, 2000);
        pool.push_task(_PM_pipeline_benchmark::thread_function_micro, 1, 3000, 1000, 2500);
        pool.push_task(_PM_pipeline_benchmark::thread_function_micro, 1, 1000, 1000, 1000);
        pool.push_task(_PM_pipeline_benchmark::thread_function_micro, 3, 500, 1000, 1500);
        pool.push_task(_PM_pipeline_benchmark::thread_function_micro, 3, 500, 1000, 1500);
        pool.push_task(_PM_pipeline_benchmark::thread_function_micro, 0, 2000, 1000, 4500);
        pool.wait_for_tasks();
    }
}

static void STD_pipeline_benchmark_gaming(benchmark::State& state) {
    BS::thread_pool pool(8);
    for (auto _ : state){
        pool.push_task(_STD_pipeline_benchmark::thread_function_micro, 2000, 1000, 5000);
        pool.push_task(_STD_pipeline_benchmark::thread_function_micro, 1500, 1000, 3000);
        pool.push_task(_STD_pipeline_benchmark::thread_function_micro, 2000, 1000, 2000);
        pool.push_task(_STD_pipeline_benchmark::thread_function_micro, 3000, 1000, 2500);
        pool.push_task(_STD_pipeline_benchmark::thread_function_micro, 1000, 1000, 1000);
        pool.push_task(_STD_pipeline_benchmark::thread_function_micro, 500, 1000, 1500);
        pool.push_task(_STD_pipeline_benchmark::thread_function_micro, 500, 1000, 1500);
        pool.push_task(_STD_pipeline_benchmark::thread_function_micro, 2000, 1000, 4500);
        pool.wait_for_tasks();
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
#ifdef EXPERIMENTAL_MUTEXES
static void EXP_pipeline_benchmark_audio(benchmark::State& state) {
    std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
    std::array<int, 8> preCT {200, 150, 200, 300, 100, 50, 50, 200};
    int CT = 100;
    std::array<int, 8> postCT {500, 300, 200, 250, 100, 150, 150, 450};

    for (auto _ : state) {
      _EXP_PM_pipeline_benchmark::thread_function_micro(prios[state.thread_index()] ,preCT[state.thread_index()], CT, postCT[state.thread_index()]);
    }
}
#endif
static void STD_pipeline_benchmark_fast(benchmark::State& state) {
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
#ifdef EXPERIMENTAL_MUTEXES
static void EXP_pipeline_benchmark_fast(benchmark::State& state) {
    std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
    std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
    int CT = 1000;
    std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

    for (auto _ : state) {
      _EXP_PM_pipeline_benchmark::thread_function_nano(prios[state.thread_index()] ,preCT[state.thread_index()], CT, postCT[state.thread_index()]);
    }

}
#endif

BENCHMARK(PM_LockUnlock)->Threads(8);
#ifdef EXPERIMENTAL_MUTEXES
BENCHMARK(EXPPM_LockUnlock)->Threads(8);
#endif
BENCHMARK(STD_LockUnlock)->Threads(8);

BENCHMARK(PM_S_LockUnlock);
BENCHMARK(STD_S_LockUnlock);

BENCHMARK(PM_S_SLockSUnlock);
BENCHMARK(STD_S_SLockSUnlock);

BENCHMARK(PM_pipeline_benchmark_long)->UseRealTime();
BENCHMARK(STD_pipeline_benchmark_long)->UseRealTime();

BENCHMARK(PM_pipeline_benchmark_gaming)->UseRealTime();
BENCHMARK(STD_pipeline_benchmark_gaming)->UseRealTime();

BENCHMARK(PM_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(STD_pipeline_benchmark_audio)->Threads(8);
#ifdef EXPERIMENTAL_MUTEXES
BENCHMARK(EXP_pipeline_benchmark_audio)->Threads(8);
#endif
BENCHMARK(PM_pipeline_benchmark_fast)->Threads(8);
#ifdef EXPERIMENTAL_MUTEXES
BENCHMARK(EXP_pipeline_benchmark_fast)->Threads(8);
#endif
BENCHMARK(STD_pipeline_benchmark_fast)->Threads(8);

BENCHMARK_MAIN();