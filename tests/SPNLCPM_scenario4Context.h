#include <spinlock_priority_mutex.h>
#include <DeterministicConcurrency>
#include <vector>
#include <random>

namespace SPNLCPM_scenario4{
    using namespace DeterministicConcurrency;

    PrioSync::spinlock_priority_mutex<7> m;

    std::vector<int> ret;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, 6);

    void threadFunction(thread_context* c ,int i) {
        c->lock(&m,i);
        ret.push_back(i);
        m.unlock();
    }

    void controlThread(thread_context* c) {
        c->lock(&m);
        c->switchContext();
        m.unlock();
    }

    static size_t THREAD0 = 0;
    static size_t THREAD1 = 1;
    static size_t THREAD2 = 2;
    static size_t THREAD3 = 3;
    static size_t THREAD4 = 4;
    static size_t THREAD5 = 5;
    static size_t THREAD6 = 6;
    static size_t THREAD7 = 7;
    static size_t CTRLTHREAD = 8;
        
    static constexpr auto executeSchedulingSequence = []{
        auto thread_0 = std::tuple{&threadFunction, distribution(gen)};
        auto thread_1 = std::tuple{&threadFunction, distribution(gen)};
        auto thread_2 = std::tuple{&threadFunction, distribution(gen)};
        auto thread_3 = std::tuple{&threadFunction, distribution(gen)};
        auto thread_4 = std::tuple{&threadFunction, distribution(gen)};
        auto thread_5 = std::tuple{&threadFunction, distribution(gen)};
        auto thread_6 = std::tuple{&threadFunction, distribution(gen)};
        auto thread_7 = std::tuple{&threadFunction, distribution(gen)};
        auto ctrlThread = std::tuple{&controlThread};
        
        auto sch = make_UserControlledScheduler(
            thread_0, thread_1, thread_2, thread_3, thread_4, thread_5, thread_6, thread_7, ctrlThread
        );
        sch.switchContextTo(CTRLTHREAD);// give control to the control thread first so it acquires the lock
        sch.proceed(// allow all the other thread to proceed
            THREAD0,
            THREAD1,
            THREAD2,
            THREAD3,
            THREAD4,
            THREAD5,
            THREAD6,
            THREAD7
        );
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING_EXTERNAL>(// wait until all of them are waiting on the lock
            THREAD0,
            THREAD1,
            THREAD2,
            THREAD3,
            THREAD4,
            THREAD5,
            THREAD6,
            THREAD7
        );
        sch.switchContextTo(CTRLTHREAD);// at this point the the control thread can end and all the others should respect their priority
        sch.joinAll();
    };
}