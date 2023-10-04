#include <boost/fairness.hpp>
#include "../waiting_utils.hpp"

namespace _SPNLC_PM_pipeline_benchmark{

    static boost::fairness::spinlock_priority_mutex<4> m;

    static void thread_function(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        waiting::utils::busy_wait_milli(preCriticalTime);
        m.lock(p);
        waiting::utils::busy_wait_milli(criticalTime);
        m.unlock();
        waiting::utils::busy_wait_milli(postCriticalTime);
    }

    static void thread_function_micro(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        waiting::utils::busy_wait_micro(preCriticalTime);
        m.lock(p);
        waiting::utils::busy_wait_micro(criticalTime);
        m.unlock();
        waiting::utils::busy_wait_micro(postCriticalTime);
    }

    static void thread_function_nano(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        waiting::utils::busy_wait_nano(preCriticalTime);
        m.lock(p);
        waiting::utils::busy_wait_nano(criticalTime);
        m.unlock();
        waiting::utils::busy_wait_nano(postCriticalTime);
    }

}