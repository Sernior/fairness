#include <boost/fairness.hpp>
#include <DeterministicConcurrency>
#include <vector>

namespace RPM_scenario9{
    using namespace DeterministicConcurrency;

    boost::fairness::recursive_priority_mutex<10> m;

    std::vector<int> ret;

    std::vector<int> expected{
        0, 1, 2, 3, 4
    };

    void threadFunction(thread_context* c ,int i) {
        c->lock(&m,i);
        if (m.try_lock(i+2)){
            ret.push_back(i);
            m.unlock();
        }
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
    static size_t CTRLTHREAD = 5;
    
    static constexpr auto executeSchedulingSequence = []{
        auto thread_0 = std::tuple{&threadFunction, 0};
        auto thread_1 = std::tuple{&threadFunction, 1};
        auto thread_2 = std::tuple{&threadFunction, 2};
        auto thread_3 = std::tuple{&threadFunction, 3};
        auto thread_4 = std::tuple{&threadFunction, 4};
        auto ctrlThread = std::tuple{&controlThread};

        auto sch = make_UserControlledScheduler(
            thread_0, thread_1, thread_2, thread_3, thread_4, ctrlThread
        );

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
        sch.joinAll();
    };
}