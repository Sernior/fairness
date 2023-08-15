#include <shared_priority_mutex.h>
#include <DeterministicConcurrency>
#include <vector>
namespace SPMscenario5{
    using namespace DeterministicConcurrency;

    PrioSync::shared_priority_mutex<5> m;

    std::vector<int> ret;

    std::vector<int> expected{
        0, 1
    };


    void threadFunction(thread_context* c ,int i) {
        c->lock(&m);
        c->switchContext();
        ret.push_back(i);
        m.unlock();
    }

    void threadFunction2(thread_context* c ,int i) {
        m.unlock();// this unlock should do nothing if you are a mutex and not a semaphore
        c->switchContext();
        c->lock(&m);
        ret.push_back(i);
        m.unlock();
    }


    static auto thread_0 = std::tuple{&threadFunction, 0};
    static auto thread_1 = std::tuple{&threadFunction2, 1};

    static size_t THREAD0 = 0;
    static size_t THREAD1 = 1;
    
    static auto sch = make_UserControlledScheduler(
        thread_0, thread_1
    );
    
    static constexpr auto executeSchedulingSequence = []{
        sch.switchContextTo(THREAD0);
        sch.switchContextTo(THREAD1);
        sch.proceed(THREAD1, THREAD0);// thread 1 here should be stuck on the lock assuming my mutex is a mutex and not a semaphore
        sch.joinAll();
    };
}