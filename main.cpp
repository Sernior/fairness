#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <algorithm>
#include <BS_thread_pool.hpp>

// #include <boost/atomic.hpp>
#define BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
#include <boost/fairness.hpp>
#undef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES

static boost::fairness::priority_mutex<4> pm;
static boost::fairness::experimental::experimental_priority_mutex<4> ms;
static boost::fairness::spinlock_priority_mutex<4> sms;
static std::mutex m;

#define NOW std::chrono::steady_clock::now()


static void busy_wait_nano(uint64_t nanoseconds){
    auto begin = NOW;
    for(;std::chrono::duration_cast<std::chrono::nanoseconds>(NOW - begin).count() < nanoseconds;)
        continue;
}

static void thread_function_nano(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
    busy_wait_nano(preCriticalTime);
    {
        boost::fairness::unique_lock l1(ms, p);
        busy_wait_nano(criticalTime);
    }
    busy_wait_nano(postCriticalTime);
}

static void thread_function_nano_std(int preCriticalTime, int criticalTime, int postCriticalTime){
    busy_wait_nano(preCriticalTime);
    {
        std::lock_guard l1(m);
        busy_wait_nano(criticalTime);
    }
    busy_wait_nano(postCriticalTime);
}

int main()
{
    std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
    std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
    int CT = 1000;
    std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};
    {
        BS::thread_pool pool(8);

        auto start_time = std::chrono::high_resolution_clock::now();

        for (int j = 0; j < 200000; ++j) {
            for (int i = 0; i < 8; ++i) {
                pool.push_task(thread_function_nano, prios[i], preCT[i], CT, postCT[i]);
            }
            pool.wait_for_tasks();
            if (j % 500 == 0) {
                std::cout << j << std::endl;
            }
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << "Time taken PRIO: " << duration.count() << " milliseconds" << std::endl;
    }

    {
        BS::thread_pool pool(8);

        auto start_time = std::chrono::high_resolution_clock::now();

        for (int j = 0; j < 200000; ++j) {
            for (int i = 0; i < 8; ++i) {
                pool.push_task(thread_function_nano_std, preCT[i], CT, postCT[i]);
            }
            pool.wait_for_tasks();
            if (j % 500 == 0) {
                std::cout << j << std::endl;
            }
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << "Time taken STD: " << duration.count() << " milliseconds" << std::endl;
    }


    return 0;
}