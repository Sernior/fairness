#ifndef BOOST_FAIRNESS_THREAD_UTILS
#define BOOST_FAIRNESS_THREAD_UTILS
#include "waiting_utils.hpp"

#define NOW std::chrono::high_resolution_clock::now()

namespace utils::thread{
    void thread_function_milli(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_milli(preCriticalTime);
        // m.lock(p);
        utils::waiting::busy_wait_milli(criticalTime);
        // m.unlock();
        utils::waiting::busy_wait_milli(postCriticalTime);
    }

    void thread_function_micro(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_micro(preCriticalTime);
        // m.lock(p);
        utils::waiting::busy_wait_micro(criticalTime);
        // m.unlock();
        utils::waiting::busy_wait_micro(postCriticalTime);
    }

    void thread_function_nano(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_nano(preCriticalTime);
        // m.lock(p);
        utils::waiting::busy_wait_nano(criticalTime);
        // m.unlock();
        utils::waiting::busy_wait_nano(postCriticalTime);
    }

    void shared_thread_function_milli(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_milli(preCriticalTime);
        // m.lock_shared(p);
        utils::waiting::busy_wait_milli(criticalTime);
        // m.unlock_shared();
        utils::waiting::busy_wait_milli(postCriticalTime);
    }

    void shared_thread_function_micro(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_micro(preCriticalTime);
        // m.lock_shared(p);
        utils::waiting::busy_wait_micro(criticalTime);
        // m.unlock_shared();
        utils::waiting::busy_wait_micro(postCriticalTime);
    }

    void shared_thread_function_nano(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_nano(preCriticalTime);
        // m.lock_shared(p);
        utils::waiting::busy_wait_nano(criticalTime);
        // m.unlock_shared();
        utils::waiting::busy_wait_nano(postCriticalTime);
    }

    void try_thread_function_milli(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_milli(preCriticalTime);
        // m.try_lock(p);
        utils::waiting::busy_wait_milli(criticalTime);
        // m.unlock();
        utils::waiting::busy_wait_milli(postCriticalTime);
    }

    void try_thread_function_micro(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_micro(preCriticalTime);
        // m.try_lock(p);
        utils::waiting::busy_wait_micro(criticalTime);
        // m.unlock();
        utils::waiting::busy_wait_micro(postCriticalTime);
    }

    void try_thread_function_nano(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_nano(preCriticalTime);
        // m.try_lock(p);
        utils::waiting::busy_wait_nano(criticalTime);
        // m.unlock();
        utils::waiting::busy_wait_nano(postCriticalTime);
    }

    void try_shared_thread_function_milli(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_milli(preCriticalTime);
        // m.try_lock_shared(p);
        utils::waiting::busy_wait_milli(criticalTime);
        // m.unlock_shared();
        utils::waiting::busy_wait_milli(postCriticalTime);
    }

    void try_shared_thread_function_micro(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_micro(preCriticalTime);
        // m.try_lock_shared(p);
        utils::waiting::busy_wait_micro(criticalTime);
        // m.unlock_shared();
        utils::waiting::busy_wait_micro(postCriticalTime);
    }

    void try_shared_thread_function_nano(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        utils::waiting::busy_wait_nano(preCriticalTime);
        // m.try_lock_shared(p);
        utils::waiting::busy_wait_nano(criticalTime);
        // m.unlock_shared();
        utils::waiting::busy_wait_nano(postCriticalTime);
    }
}

#undef NOW
#endif // BOOST_FAIRNESS_THREAD_UTILS