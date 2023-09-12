#include <priority_mutex.h>
#include <vector>
#include <chrono>
#include <thread>
#include <tuple>
#include <BS_thread_pool.hpp>
#include <DeterministicConcurrency>
#define NOW std::chrono::steady_clock::now()

#ifdef EXPERIMENTAL_MUTEXES
#include <experimental_priority_mutex.h>
#endif

/*
std::this_thread::sleep_for is too imprecise, and also I could make a bunch of variables during busy waiting to simulate heated cpu caches.
*/

static void busy_wait_micro(uint32_t microseconds){
    auto begin = NOW;
    for(;std::chrono::duration_cast<std::chrono::microseconds>(NOW - begin).count() < microseconds;)
        continue;
}

static void busy_wait_milli(uint32_t milliseconds){
    auto begin = NOW;
    for(;std::chrono::duration_cast<std::chrono::milliseconds>(NOW - begin).count() < milliseconds;)
        continue;
}

static void busy_wait_nano(uint32_t nanoseconds){
    auto begin = NOW;
    for(;std::chrono::duration_cast<std::chrono::nanoseconds>(NOW - begin).count() < nanoseconds;)
        continue;
}

namespace _PM_pipeline_benchmark{

    static PrioSync::priority_mutex<4> m;

    static void thread_function(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        busy_wait_milli(preCriticalTime);
        m.lock(p);
        busy_wait_milli(criticalTime);
        m.unlock();
        busy_wait_milli(postCriticalTime);
    }

    static void thread_function_micro(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        busy_wait_micro(preCriticalTime);
        m.lock(p);
        busy_wait_micro(criticalTime);
        m.unlock();
        busy_wait_micro(postCriticalTime);
    }

    static void thread_function_nano(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        busy_wait_nano(preCriticalTime);
        m.lock(p);
        busy_wait_nano(criticalTime);
        m.unlock();
        busy_wait_nano(postCriticalTime);
    }

    static void thread_loop(DeterministicConcurrency::thread_context* c, int iterations, int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        for (;iterations > 0; iterations--){
            c->switchContext();
            busy_wait_nano(preCriticalTime);
            m.lock(p);
            busy_wait_nano(criticalTime);
            m.unlock();
            busy_wait_nano(postCriticalTime);
        }
    }

}
#ifdef EXPERIMENTAL_MUTEXES
namespace _EXP_PM_pipeline_benchmark{

    static PrioSync::experimental_priority_mutex<4> m;

    static void thread_function(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        busy_wait_milli(preCriticalTime);
        m.lock(p);
        busy_wait_milli(criticalTime);
        m.unlock();
        busy_wait_milli(postCriticalTime);
    }

    static void thread_function_micro(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        busy_wait_micro(preCriticalTime);
        m.lock(p);
        busy_wait_micro(criticalTime);
        m.unlock();
        busy_wait_micro(postCriticalTime);
    }

    static void thread_function_nano(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        busy_wait_nano(preCriticalTime);
        m.lock(p);
        busy_wait_nano(criticalTime);
        m.unlock();
        busy_wait_nano(postCriticalTime);
    }

    static void thread_loop(DeterministicConcurrency::thread_context* c, int iterations, int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        for (;iterations > 0; iterations--){
            c->switchContext();
            busy_wait_nano(preCriticalTime);
            m.lock(p);
            busy_wait_nano(criticalTime);
            m.unlock();
            busy_wait_nano(postCriticalTime);
        }
    }

}
#endif
namespace _STD_pipeline_benchmark{

    static std::mutex m;

    static void thread_function(int preCriticalTime, int criticalTime, int postCriticalTime){
        busy_wait_milli(preCriticalTime);
        m.lock();
        busy_wait_milli(criticalTime);
        m.unlock();
        busy_wait_milli(postCriticalTime);
    }

    static void thread_function_micro(int preCriticalTime, int criticalTime, int postCriticalTime){
        busy_wait_micro(preCriticalTime);
        m.lock();
        busy_wait_micro(criticalTime);
        m.unlock();
        busy_wait_micro(postCriticalTime);
    }

    static void thread_function_nano(int preCriticalTime, int criticalTime, int postCriticalTime){
        busy_wait_nano(preCriticalTime);
        m.lock();
        busy_wait_nano(criticalTime);
        m.unlock();
        busy_wait_nano(postCriticalTime);
    }

    static void thread_loop(DeterministicConcurrency::thread_context* c, int iterations, int preCriticalTime, int criticalTime, int postCriticalTime){
        for (;iterations > 0; iterations--){
            c->switchContext();
            busy_wait_nano(preCriticalTime);
            m.lock();
            busy_wait_nano(criticalTime);
            m.unlock();
            busy_wait_nano(postCriticalTime);
        }
    }

}

#undef NOW
