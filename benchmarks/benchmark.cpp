#include <benchmark/benchmark.h>
#include <priority_mutex.h>
#include <shared_priority_mutex.h>
#include <mutex>
#include <shared_mutex>
#include "pipeline_benchmark.cpp"

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
    PrioSync::priority_mutex<4> m;
    PM_pipeline_benchmark_helper b(&m);
    for (auto _ : state){
        auto sch = b.init_scheduler();
        b.startPipeline(sch);
    }
}

static void STD_pipeline_benchmark(benchmark::State& state) {
    std::mutex m;
    STD_pipeline_benchmark_helper b(&m);
    for (auto _ : state){
        auto sch = b.init_scheduler();
        b.startPipeline(sch);
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