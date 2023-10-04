#include <boost/fairness.hpp>
#include <vector>
#include <thread>
#include <tuple>
#include <BS_thread_pool.hpp>
#include <DeterministicConcurrency>
#include "waiting_utils.hpp"

namespace _PM_pipeline_benchmark{

    static boost::fairness::priority_mutex<4> m;

    static void thread_function(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        _waiting_utils_::busy_wait_milli(preCriticalTime);
        m.lock(p);
        _waiting_utils_::busy_wait_milli(criticalTime);
        m.unlock();
        _waiting_utils_::busy_wait_milli(postCriticalTime);
    }

    static void thread_function_micro(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        _waiting_utils_::busy_wait_micro(preCriticalTime);
        m.lock(p);
        _waiting_utils_::busy_wait_micro(criticalTime);
        m.unlock();
        _waiting_utils_::busy_wait_micro(postCriticalTime);
    }

    static void thread_function_nano(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        _waiting_utils_::busy_wait_nano(preCriticalTime);
        m.lock(p);
        _waiting_utils_::busy_wait_nano(criticalTime);
        m.unlock();
        _waiting_utils_::busy_wait_nano(postCriticalTime);
    }

}

namespace _SLM_PM_pipeline_benchmark{

    static boost::fairness::slim_priority_mutex<4> m;

    static void thread_function(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        _waiting_utils_::busy_wait_milli(preCriticalTime);
        m.lock(p);
        _waiting_utils_::busy_wait_milli(criticalTime);
        m.unlock();
        _waiting_utils_::busy_wait_milli(postCriticalTime);
    }

    static void thread_function_micro(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        _waiting_utils_::busy_wait_micro(preCriticalTime);
        m.lock(p);
        _waiting_utils_::busy_wait_micro(criticalTime);
        m.unlock();
        _waiting_utils_::busy_wait_micro(postCriticalTime);
    }

    static void thread_function_nano(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        _waiting_utils_::busy_wait_nano(preCriticalTime);
        m.lock(p);
        _waiting_utils_::busy_wait_nano(criticalTime);
        m.unlock();
        _waiting_utils_::busy_wait_nano(postCriticalTime);
    }

}
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
namespace _SPNLC_SLM_PM_pipeline_benchmark{

    static boost::fairness::experimental::slim_spinlock_priority_mutex<4> m;

    static void thread_function(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        _waiting_utils_::busy_wait_milli(preCriticalTime);
        m.lock(p);
        _waiting_utils_::busy_wait_milli(criticalTime);
        m.unlock();
        _waiting_utils_::busy_wait_milli(postCriticalTime);
    }

    static void thread_function_micro(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        _waiting_utils_::busy_wait_micro(preCriticalTime);
        m.lock(p);
        _waiting_utils_::busy_wait_micro(criticalTime);
        m.unlock();
        _waiting_utils_::busy_wait_micro(postCriticalTime);
    }

    static void thread_function_nano(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        _waiting_utils_::busy_wait_nano(preCriticalTime);
        m.lock(p);
        _waiting_utils_::busy_wait_nano(criticalTime);
        m.unlock();
        _waiting_utils_::busy_wait_nano(postCriticalTime);
    }

}
#endif
namespace _SPNLC_PM_pipeline_benchmark{

    static boost::fairness::spinlock_priority_mutex<4> m;

    static void thread_function(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        _waiting_utils_::busy_wait_milli(preCriticalTime);
        m.lock(p);
        _waiting_utils_::busy_wait_milli(criticalTime);
        m.unlock();
        _waiting_utils_::busy_wait_milli(postCriticalTime);
    }

    static void thread_function_micro(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        _waiting_utils_::busy_wait_micro(preCriticalTime);
        m.lock(p);
        _waiting_utils_::busy_wait_micro(criticalTime);
        m.unlock();
        _waiting_utils_::busy_wait_micro(postCriticalTime);
    }

    static void thread_function_nano(int p, int preCriticalTime, int criticalTime, int postCriticalTime){
        _waiting_utils_::busy_wait_nano(preCriticalTime);
        m.lock(p);
        _waiting_utils_::busy_wait_nano(criticalTime);
        m.unlock();
        _waiting_utils_::busy_wait_nano(postCriticalTime);
    }

}

namespace _STD_pipeline_benchmark{

    static std::mutex m;

    static void thread_function(int preCriticalTime, int criticalTime, int postCriticalTime){
        _waiting_utils_::busy_wait_milli(preCriticalTime);
        m.lock();
        _waiting_utils_::busy_wait_milli(criticalTime);
        m.unlock();
        _waiting_utils_::busy_wait_milli(postCriticalTime);
    }

    static void thread_function_micro(int preCriticalTime, int criticalTime, int postCriticalTime){
        _waiting_utils_::busy_wait_micro(preCriticalTime);
        m.lock();
        _waiting_utils_::busy_wait_micro(criticalTime);
        m.unlock();
        _waiting_utils_::busy_wait_micro(postCriticalTime);
    }

    static void thread_function_nano(int preCriticalTime, int criticalTime, int postCriticalTime){
        _waiting_utils_::busy_wait_nano(preCriticalTime);
        m.lock();
        _waiting_utils_::busy_wait_nano(criticalTime);
        m.unlock();
        _waiting_utils_::busy_wait_nano(postCriticalTime);
    }

}

