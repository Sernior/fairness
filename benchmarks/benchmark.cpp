#include <benchmark/benchmark.h>
#include <priority_mutex.h>
#include <shared_priority_mutex.h>
#include <mutex>
#include <shared_mutex>
#include "pipeline_benchmark.cpp"
#include <BS_thread_pool.hpp>

static void PM_LockUnlock(benchmark::State& state) {
    PrioSync::priority_mutex<10> m;
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

static void PM_pipeline_benchmark(benchmark::State& state) {
    BS::thread_pool pool(8);
    for (auto _ : state){
        pool.push_task(_PM_pipeline_benchmark::thread_function, 0, 20, 1, 50);
        pool.push_task(_PM_pipeline_benchmark::thread_function, 2, 15, 1, 30);
        pool.push_task(_PM_pipeline_benchmark::thread_function, 2, 20, 1, 20);
        pool.push_task(_PM_pipeline_benchmark::thread_function, 1, 30, 1, 25);
        pool.push_task(_PM_pipeline_benchmark::thread_function, 1, 25, 1, 30);
        pool.push_task(_PM_pipeline_benchmark::thread_function, 3, 10, 1, 10);
        pool.push_task(_PM_pipeline_benchmark::thread_function, 3, 5,  1, 15);
        pool.push_task(_PM_pipeline_benchmark::thread_function, 0, 20, 1, 45);
        pool.wait_for_tasks();
    }
}

static void STD_pipeline_benchmark(benchmark::State& state) {
    BS::thread_pool pool(8);
    for (auto _ : state){
        pool.push_task(_STD_pipeline_benchmark::thread_function, 20, 1, 50);
        pool.push_task(_STD_pipeline_benchmark::thread_function, 15, 1, 30);
        pool.push_task(_STD_pipeline_benchmark::thread_function, 20, 1, 20);
        pool.push_task(_STD_pipeline_benchmark::thread_function, 30, 1, 25);
        pool.push_task(_STD_pipeline_benchmark::thread_function, 25, 1, 30);
        pool.push_task(_STD_pipeline_benchmark::thread_function, 10, 1, 10);
        pool.push_task(_STD_pipeline_benchmark::thread_function, 5,  1, 15);
        pool.push_task(_STD_pipeline_benchmark::thread_function, 20, 1, 45);
        pool.wait_for_tasks();
    }
}

BENCHMARK(PM_LockUnlock);
BENCHMARK(STD_LockUnlock);

BENCHMARK(PM_S_LockUnlock);
BENCHMARK(STD_S_LockUnlock);

BENCHMARK(PM_S_SLockSUnlock);
BENCHMARK(STD_S_SLockSUnlock);

BENCHMARK(PM_pipeline_benchmark);
BENCHMARK(STD_pipeline_benchmark);

BENCHMARK_MAIN();