#include <shared_priority_mutex.h>
#include <DeterministicConcurrency>
#include <vector>
namespace SPMscenario4{
    using namespace DeterministicConcurrency;

    PrioSync::shared_priority_mutex<5> m;

    std::vector<int> ret;

    std::vector<int> expected{
        0, 4, 1, 2, 3
    };

    void threadFunction(thread_context* c ,int i) {
        c->lock_shared(&m, i);
        c->switchContext();
        ret.push_back(i);
        m.unlock_shared();
    }

    void threadFunction2(thread_context* c ,int i) {
        c->lock(&m, i);
        c->switchContext();
        ret.push_back(i);
        m.unlock();
    }


    void controlThread(thread_context* c) {
        c->lock(&m);
        c->switchContext();
        m.unlock();
    }

    static auto thread_0 = std::tuple{&threadFunction, 0};
    static auto thread_1 = std::tuple{&threadFunction, 1};
    static auto thread_2 = std::tuple{&threadFunction, 2};
    static auto thread_3 = std::tuple{&threadFunction2, 3};
    static auto thread_4 = std::tuple{&threadFunction, 4};
    static auto ctrlThread = std::tuple{&controlThread};

    static size_t THREAD0 = 0;
    static size_t THREAD1 = 1;
    static size_t THREAD2 = 2;
    static size_t THREAD3 = 3;
    static size_t THREAD4 = 4;
    static size_t CTRLTHREAD = 5;
    
    static auto sch = make_UserControlledScheduler(
        thread_0, thread_1, thread_2, thread_3, thread_4, ctrlThread
    );
    // 0 4 1 2 3
    static constexpr auto executeSchedulingSequence = []{
        sch.switchContextTo(CTRLTHREAD);// acquire the lock and then the scheduler reaquire control
        sch.proceed(THREAD0, THREAD4);
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING_EXTERNAL>(THREAD0, THREAD4);// wait until 0 and 4 are stuck on the lock_shared
        sch.switchContextTo(CTRLTHREAD);// now you unlock the mutex so 0 since it has higher priority must go first
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING>(THREAD0, THREAD4);// so now both 0 and 4 can reach their next switch context
        sch.switchContextTo(THREAD0);//so now you unlock and finish while 4 is still waiting on the switch context
        sch.proceed(THREAD1, THREAD2, THREAD3);// now everyone else decide to start 1 and 2 can get in since they are also shared and 4 is still holding shared but 3 wait on the lock
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING>(THREAD1, THREAD2);
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING_EXTERNAL>(THREAD3);
        sch.switchContextTo(THREAD4, THREAD1, THREAD2);// after 4 1 and 2 are done finally 3 can acquire the unique lock
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING>(THREAD3);
        sch.switchContextTo(THREAD3);
        sch.joinAll();
    };
}