#include <priority_mutex.h>
#include <DeterministicConcurrency>
#include <vector>
#include <chrono>
#include <thread>
#include <tuple>
using namespace DeterministicConcurrency;
template<typename Lockable>
class PM_pipeline_benchmark_helper{
    private:

    Lockable* m;

    static void threadFunction(thread_context* c, Lockable* m, int prio, uint32_t preCriticalTime, uint32_t criticalTime, uint32_t postCriticalTime) {
        std::this_thread::sleep_for(std::chrono::milliseconds(preCriticalTime));
        m->lock(prio);
        std::this_thread::sleep_for(std::chrono::milliseconds(criticalTime));
        m->unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(postCriticalTime));
    }

    public:

    PM_pipeline_benchmark_helper(Lockable* p) : m(p) {};

    auto init_scheduler(){
        auto thread_0 = std::tuple{&threadFunction, m, 0, 20, 1, 50};
        auto thread_1 = std::tuple{&threadFunction, m, 2, 15, 1, 30};
        auto thread_2 = std::tuple{&threadFunction, m, 2, 20, 1, 20};
        auto thread_3 = std::tuple{&threadFunction, m, 1, 30, 1, 25};
        auto thread_4 = std::tuple{&threadFunction, m, 1, 25, 1, 30};
        auto thread_5 = std::tuple{&threadFunction, m, 3, 10, 1, 10};
        auto thread_6 = std::tuple{&threadFunction, m, 3, 5,  1, 15};
        auto thread_7 = std::tuple{&threadFunction, m, 0, 20, 1, 45};

        return make_UserControlledScheduler(
            thread_0, thread_1, thread_2, thread_3, thread_4, thread_5, thread_6, thread_7
        );
    }

    template<typename SCH>
    void startPipeline (SCH& sch){
        sch.proceed(0, 1, 2, 3, 4, 5, 6, 7);
        sch.joinAll();
    };
};

template<typename Lockable>
class STD_pipeline_benchmark_helper{
    private:

    Lockable* m;

    static void threadFunction(thread_context* c, Lockable* m, int prio, uint32_t preCriticalTime, uint32_t criticalTime, uint32_t postCriticalTime) {
        std::this_thread::sleep_for(std::chrono::milliseconds(preCriticalTime));
        m->lock();
        std::this_thread::sleep_for(std::chrono::milliseconds(criticalTime));
        m->unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(postCriticalTime));
    }

    public:

    STD_pipeline_benchmark_helper(Lockable* p) : m(p) {};

    auto init_scheduler(){
        auto thread_0 = std::tuple{&threadFunction, m, 0, 20, 1, 50};
        auto thread_1 = std::tuple{&threadFunction, m, 2, 15, 1, 30};
        auto thread_2 = std::tuple{&threadFunction, m, 2, 20, 1, 20};
        auto thread_3 = std::tuple{&threadFunction, m, 1, 30, 1, 25};
        auto thread_4 = std::tuple{&threadFunction, m, 1, 25, 1, 30};
        auto thread_5 = std::tuple{&threadFunction, m, 3, 10, 1, 10};
        auto thread_6 = std::tuple{&threadFunction, m, 3, 5,  1, 15};
        auto thread_7 = std::tuple{&threadFunction, m, 0, 20, 1, 45};

        return make_UserControlledScheduler(
            thread_0, thread_1, thread_2, thread_3, thread_4, thread_5, thread_6, thread_7
        );
    }

    template<typename SCH>
    void startPipeline (SCH& sch){
        sch.proceed(0, 1, 2, 3, 4, 5, 6, 7);
        sch.joinAll();
    };
};
