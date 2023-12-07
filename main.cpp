#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <algorithm>
#include <BS_thread_pool.hpp>
#define BOOST_FAIRNESS_USE_EXPERIMENTAL_WAIT_NOTIFY
// #include <boost/atomic.hpp>
#include <boost/fairness.hpp>
#include <boost/fairness/detail/coherent_priority_lock.hpp>

static boost::fairness::shared_priority_mutex<4> spm;
static boost::fairness::priority_mutex<4> pm;
static boost::fairness::spinlock_priority_mutex<4> sms;
static std::mutex m;
//static boost::fairness::detail::mcs_spinlock mcs;
static boost::fairness::detail::coherent_priority_lock pmcs;

#define NOW std::chrono::high_resolution_clock::now()

static std::vector<int> ret;
static std::vector<int> ret2;
/*

static void busy_wait_nano(uint64_t nanoseconds){
    auto begin = NOW;
    for(;std::chrono::duration_cast<std::chrono::nanoseconds>(NOW - begin).count() < nanoseconds;)
        continue;
}

static void thread_function_nano(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
    busy_wait_nano(preCriticalTime);
    {
        boost::fairness::unique_lock l1(sms, p);
        busy_wait_nano(criticalTime);
    }
    busy_wait_nano(postCriticalTime);
}

static void thread_function_nano_std(int preCriticalTime, int criticalTime, int postCriticalTime){
    busy_wait_nano(preCriticalTime);
    {
        std::unique_lock l1(m);
        busy_wait_nano(criticalTime);
    }
    busy_wait_nano(postCriticalTime);
}*/
/*
static void mcs_test(int i){
    boost::fairness::detail::QNode p;
    mcs.acquire(&p);
    ret.push_back(i);
    mcs.release(&p);
}*/

static void pmcs_test(int i){
    boost::fairness::detail::Thread t(i);
    pmcs.request_lock(&t);
    ret.push_back(i);
    pmcs.grant_lock(&t);

}

static void pmcs_test2(int i){
    boost::fairness::detail::Thread t(i);
    pmcs.request_lock(&t);
    ret2.push_back(i);
    pmcs.grant_lock(&t);

}

int main()
{/*
    std::array<int, 8> prios {0, 1, 2, 1, 3, 2, 2, 0};
    std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
    int CT = 1000;
    std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};
    {
        BS::thread_pool pool(8);

        auto start_time = NOW;

        for (int j = 0; j < 200000; ++j) {
            for (int i = 0; i < 8; ++i) {
                pool.push_task(thread_function_nano, prios[i], preCT[i], CT, postCT[i]);
            }
            pool.wait_for_tasks();
            if (j % 50000 == 0) {
                std::cout << j << std::endl;
            }
        }

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(NOW - start_time);

        std::cout << "Time taken PRIO: " << duration.count() << " milliseconds" << std::endl;
    }

    {
        BS::thread_pool pool(8);

        auto start_time = NOW;

        for (int j = 0; j < 200000; ++j) {
            for (int i = 0; i < 8; ++i) {
                pool.push_task(thread_function_nano_std, preCT[i], CT, postCT[i]);
            }
            pool.wait_for_tasks();
            if (j % 50000 == 0) {
                std::cout << j << std::endl;
            }
        }

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(NOW - start_time);

        std::cout << "Time taken STD: " << duration.count() << " milliseconds" << std::endl;
    }
*/

    BS::thread_pool pool(8);

    for (int i = 0; i < 8; ++i) {
        pool.push_task(pmcs_test, int(i/2));
    }
    pool.wait_for_tasks();

    for (int i = 0; i < 8; ++i) {
        pool.push_task(pmcs_test2, int(i));
    }
    pool.wait_for_tasks();

    for(auto i : ret)
        std::cout << i << " ";

    std::cout << '\n';

    for(auto i : ret2)
        std::cout << i << " ";

    return 0;
}