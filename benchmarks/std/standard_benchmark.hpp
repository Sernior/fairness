#include "../waiting_utils.hpp"

namespace _STD_pipeline_benchmark{

    static std::mutex m;

    static void thread_function(int preCriticalTime, int criticalTime, int postCriticalTime){
        waiting::utils::busy_wait_milli(preCriticalTime);
        m.lock();
        waiting::utils::busy_wait_milli(criticalTime);
        m.unlock();
        waiting::utils::busy_wait_milli(postCriticalTime);
    }

    static void thread_function_micro(int preCriticalTime, int criticalTime, int postCriticalTime){
        waiting::utils::busy_wait_micro(preCriticalTime);
        m.lock();
        waiting::utils::busy_wait_micro(criticalTime);
        m.unlock();
        waiting::utils::busy_wait_micro(postCriticalTime);
    }

    static void thread_function_nano(int preCriticalTime, int criticalTime, int postCriticalTime){
        waiting::utils::busy_wait_nano(preCriticalTime);
        m.lock();
        waiting::utils::busy_wait_nano(criticalTime);
        m.unlock();
        waiting::utils::busy_wait_nano(postCriticalTime);
    }

}