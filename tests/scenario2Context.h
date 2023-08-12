#include <priority_mutex.h>
#include <DeterministicConcurrency>
#include <vector>
namespace scenario2{
    using namespace DeterministicConcurrency;

    PrioSync::priority_mutex<5> m;

    std::mutex stdm;

    std::vector<int> ret;

    std::vector<int> expected{
        0, 1, 2, 3, 4
    };


    void threadFunction(thread_context* c ,int i) {
        c->uniqueLock(&stdm);
        c->switchContext();
        // std::cout<<i<<std::endl;
        stdm.unlock();

        // std::cout<<i<<std::endl;

        if (m.try_lock(i)) {
            ret.push_back(i);
            m.unlock();
        }

    }

    void controlThread(thread_context* c) {
        c->uniqueLock(&stdm);
        c->switchContext();
        stdm.unlock();
    }

    static auto thread_0 = std::tuple{&threadFunction, 0};
    static auto thread_1 = std::tuple{&threadFunction, 1};
    static auto thread_2 = std::tuple{&threadFunction, 2};
    static auto thread_3 = std::tuple{&threadFunction, 3};
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
    
    static constexpr auto executeSchedulingSequence = []{
        sch.switchContextTo(CTRLTHREAD);// give control to the control thread first so it acquires the lock
        sch.proceed(// allow all the other thread to proceed
            THREAD0,
            THREAD1,
            THREAD2,
            THREAD3,
            THREAD4
        );
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING_EXTERNAL>(// wait until all of them are waiting on the lock
            THREAD0,
            THREAD1,
            THREAD2,
            THREAD3,
            THREAD4
        );
        sch.switchContextTo(CTRLTHREAD);// at this point the the control thread can end and all the others should respect their priority

        sch.waitUntilAllThreadStatus<thread_status_t::WAITING>(THREAD0);
        sch.switchContextTo(THREAD0);
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING>(THREAD1);
        sch.switchContextTo(THREAD1);
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING>(THREAD2);
        sch.switchContextTo(THREAD2);
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING>(THREAD3);
        sch.switchContextTo(THREAD3);
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING>(THREAD4);
        sch.switchContextTo(THREAD4);
        sch.joinAll();
    };
}