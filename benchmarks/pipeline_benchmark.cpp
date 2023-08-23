#include <priority_mutex.h>
#include <vector>
#include <chrono>
#include <thread>
#include <tuple>
#include <BS_thread_pool.hpp>

namespace _PM_pipeline_benchmark{

    static PrioSync::priority_mutex<4> m;

    static void thread_function(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        std::this_thread::sleep_for(std::chrono::milliseconds(preCriticalTime));
        m.lock(p);
        std::this_thread::sleep_for(std::chrono::milliseconds(criticalTime));
        m.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(postCriticalTime));
    }

}

namespace _STD_pipeline_benchmark{

    static std::mutex m;

    static void thread_function(int preCriticalTime, int criticalTime, int postCriticalTime){
        std::this_thread::sleep_for(std::chrono::milliseconds(preCriticalTime));
        m.lock();
        std::this_thread::sleep_for(std::chrono::milliseconds(criticalTime));
        m.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(postCriticalTime));
    }

}
