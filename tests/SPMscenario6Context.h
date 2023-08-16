#include <shared_priority_mutex.h>
#include <DeterministicConcurrency>
#include <vector>
namespace SPMscenario6{
    using namespace DeterministicConcurrency;

    PrioSync::shared_priority_mutex<4> m;

    std::vector<int> ret;

    std::vector<int> expected{
        0, 1, 2, 2, 3
    };


    void threadFunction0(thread_context* c ,int i) {
        c->lock(&m, i);
        ret.push_back(i);
        c->switchContext();
        m.unlock();
    }

    void threadFunction1(thread_context* c ,int i) {
        c->lock_shared(&m, i);
        ret.push_back(i);
        c->switchContext();
        m.unlock_shared();
    }

    void threadFunction2(thread_context* c ,int i) {
        c->lock_shared(&m, i);
        ret.push_back(i);
        c->switchContext();
        m.unlock_shared();
    }

    void threadFunction3(thread_context* c ,int i) {
        c->lock(&m, i);
        ret.push_back(i);
        c->switchContext();
        m.unlock();
    }

    void threadFunction4(thread_context* c ,int i) {
        c->lock(&m, i);
        ret.push_back(i);
        c->switchContext();
        m.unlock();
    }


    static auto thread_0 = std::tuple{&threadFunction0, 0};
    static auto thread_1 = std::tuple{&threadFunction1, 1};
    static auto thread_2 = std::tuple{&threadFunction2, 2};
    static auto thread_3 = std::tuple{&threadFunction3, 2};
    static auto thread_4 = std::tuple{&threadFunction4, 3};

    static size_t THREAD0 = 0;
    static size_t THREAD1 = 1;
    static size_t THREAD2 = 2;
    static size_t THREAD3 = 3;
    static size_t THREAD4 = 4;
    
    static auto sch = make_UserControlledScheduler(
        thread_0, thread_1, thread_2, thread_3, thread_4
    );
    
    static constexpr auto executeSchedulingSequence = []{
        sch.switchContextTo(THREAD0);// lock
        sch.proceed(THREAD1);
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING_EXTERNAL>(THREAD1);
        sch.switchContextTo(THREAD0);// unlock
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING>(THREAD1); // lock shared
        sch.proceed(THREAD3, THREAD2);

        // 1) since thread 1 hold a shared lock and thread2 and 3 have the same priority thread 2 will be allowed to go past
        // while thread 3 who is requesting a unique lock will have to stop
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING_EXTERNAL>(THREAD3);
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING>(THREAD2);
        // the fact that the past 2 instructions did not deadlock is a meaningful assert
        // it means that the mutex is working as intended in point 1.

        // now thread 4 also wants to do a lock_shared but he, unlike thread 2, has lower priority than thread 3
        // so he must wait until thread 3 takes the unique lock and unlock
        sch.proceed(THREAD4);
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING_EXTERNAL>(THREAD4);
        // getting past this point without a deadlock means everything worked as expected

        sch.switchContextTo(THREAD1);// unlock shared
        sch.switchContextTo(THREAD2);// unlock shared
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING>(THREAD3);
        sch.switchContextTo(THREAD3);// unlock
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING>(THREAD4);
        sch.switchContextTo(THREAD4);
        sch.joinAll();
    };
}