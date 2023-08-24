#include <benchmark/benchmark.h>
#include <priority_mutex.h>
#include <shared_priority_mutex.h>
#include <mutex>
#include <shared_mutex>
#include "pipeline_benchmark.cpp"
#include <BS_thread_pool.hpp>
#include <semaphore>
#include <DeterministicConcurrency>

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
    BS::thread_pool pool(8);
    for (auto _ : state){
        pool.push_task(_PM_pipeline_benchmark::thread_function_micro, 0, 200, 100, 500);
        pool.push_task(_PM_pipeline_benchmark::thread_function_micro, 2, 150, 100, 300);
        pool.push_task(_PM_pipeline_benchmark::thread_function_micro, 2, 200, 100, 200);
        pool.push_task(_PM_pipeline_benchmark::thread_function_micro, 1, 300, 100, 250);
        pool.push_task(_PM_pipeline_benchmark::thread_function_micro, 1, 100, 100, 100);
        pool.push_task(_PM_pipeline_benchmark::thread_function_micro, 3, 50, 100, 150);
        pool.push_task(_PM_pipeline_benchmark::thread_function_micro, 3, 50, 100, 150);
        pool.push_task(_PM_pipeline_benchmark::thread_function_micro, 0, 200, 100, 450);
        pool.wait_for_tasks();
    }
}

static void STD_pipeline_benchmark_audio(benchmark::State& state) {
    BS::thread_pool pool(8);
    for (auto _ : state){
        pool.push_task(_STD_pipeline_benchmark::thread_function_micro, 200, 100, 500);
        pool.push_task(_STD_pipeline_benchmark::thread_function_micro, 150, 100, 300);
        pool.push_task(_STD_pipeline_benchmark::thread_function_micro, 200, 100, 200);
        pool.push_task(_STD_pipeline_benchmark::thread_function_micro, 300, 100, 250);
        pool.push_task(_STD_pipeline_benchmark::thread_function_micro, 100, 100, 100);
        pool.push_task(_STD_pipeline_benchmark::thread_function_micro, 50, 100, 150);
        pool.push_task(_STD_pipeline_benchmark::thread_function_micro, 50, 100, 150);
        pool.push_task(_STD_pipeline_benchmark::thread_function_micro, 200, 100, 450);
        pool.wait_for_tasks();
    }
}

static void PM_pipeline_benchmark_fast(benchmark::State& state) { // order of 10 microseconds (Chaos on avarage seems PM is slightly slower at this level)

    int iterations = 100;

    auto t0 = std::tuple(_PM_pipeline_benchmark::thread_loop, iterations, 0, 2000, 1000, 5000);
    auto t1 = std::tuple(_PM_pipeline_benchmark::thread_loop, iterations, 2, 1500, 1000, 3000);
    auto t2 = std::tuple(_PM_pipeline_benchmark::thread_loop, iterations, 2, 2000, 1000, 2000);
    auto t3 = std::tuple(_PM_pipeline_benchmark::thread_loop, iterations, 1, 3000, 1000, 2500);
    auto t4 = std::tuple(_PM_pipeline_benchmark::thread_loop, iterations, 1, 1000, 1000, 1000);
    auto t5 = std::tuple(_PM_pipeline_benchmark::thread_loop, iterations, 3, 500, 1000, 1500);
    auto t6 = std::tuple(_PM_pipeline_benchmark::thread_loop, iterations, 3, 500, 1000, 1500);
    auto t7 = std::tuple(_PM_pipeline_benchmark::thread_loop, iterations, 0, 2000, 1000, 4500);

    static auto sch = make_UserControlledScheduler(
        t0, t1, t2, t3, t4, t5, t6, t7
    );

    sch.switchContextAll();

    for (auto _ : state){
        for(;iterations > 0; iterations--){
            sch.proceed(0,1,2,3,4,5,6,7);
            sch.wait(0,1,2,3,4,5,6,7);
        }
    }
    sch.joinAll();
}

static void STD_pipeline_benchmark_fast(benchmark::State& state) {
    int iterations = 100;

    auto t0 = std::tuple(_STD_pipeline_benchmark::thread_loop, iterations, 2000, 1000, 5000);
    auto t1 = std::tuple(_STD_pipeline_benchmark::thread_loop, iterations, 1500, 1000, 3000);
    auto t2 = std::tuple(_STD_pipeline_benchmark::thread_loop, iterations, 2000, 1000, 2000);
    auto t3 = std::tuple(_STD_pipeline_benchmark::thread_loop, iterations, 3000, 1000, 2500);
    auto t4 = std::tuple(_STD_pipeline_benchmark::thread_loop, iterations, 1000, 1000, 1000);
    auto t5 = std::tuple(_STD_pipeline_benchmark::thread_loop, iterations, 500, 1000, 1500);
    auto t6 = std::tuple(_STD_pipeline_benchmark::thread_loop, iterations, 500, 1000, 1500);
    auto t7 = std::tuple(_STD_pipeline_benchmark::thread_loop, iterations, 2000, 1000, 4500);

    static auto sch = make_UserControlledScheduler(
        t0, t1, t2, t3, t4, t5, t6, t7
    );

    sch.switchContextAll();

    for (auto _ : state){
        for(;iterations > 0; iterations--){
            sch.proceed(0,1,2,3,4,5,6,7);
            sch.wait(0,1,2,3,4,5,6,7);
        }
    }
    sch.joinAll();
}

BENCHMARK(PM_LockUnlock);
BENCHMARK(STD_LockUnlock);

BENCHMARK(PM_S_LockUnlock);
BENCHMARK(STD_S_LockUnlock);

BENCHMARK(PM_S_SLockSUnlock);
BENCHMARK(STD_S_SLockSUnlock);

BENCHMARK(PM_pipeline_benchmark_long)->UseRealTime();
BENCHMARK(STD_pipeline_benchmark_long)->UseRealTime();

BENCHMARK(PM_pipeline_benchmark_gaming)->UseRealTime();
BENCHMARK(STD_pipeline_benchmark_gaming)->UseRealTime();

BENCHMARK(PM_pipeline_benchmark_audio)->UseRealTime();
BENCHMARK(STD_pipeline_benchmark_audio)->UseRealTime();

BENCHMARK(PM_pipeline_benchmark_fast)->Iterations(1);
BENCHMARK(STD_pipeline_benchmark_fast)->Iterations(1);

BENCHMARK_MAIN();