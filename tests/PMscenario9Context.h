#include <DeterministicConcurrency>
#include <vector>
#include <slim_priority_mutex.h>

namespace PMscenario9{
    using namespace DeterministicConcurrency;

    PrioSync::slim_priority_mutex<5> m;

    std::vector<bool> ret;

    std::vector<bool> expected{
        true, false, false, true, false
    };

    void threadFunction(thread_context* c ,int i) {
        if (m.try_lock(i)){
            ret.push_back(true);
            m.unlock();
            return;
        }
        ret.push_back(false);
    }

    void threadFunction2(thread_context* c ,int i) {
        if (m.try_lock(i)){
            ret.push_back(true);
            c->switchContext();
            m.unlock();
            return;
        }
        ret.push_back(false);
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
        auto thread_3 = std::tuple{&threadFunction2, 3};
        auto thread_4 = std::tuple{&threadFunction, 4};
        auto ctrlThread = std::tuple{&controlThread};
        
        auto sch = make_UserControlledScheduler(
            thread_0, thread_1, thread_2, thread_3, thread_4, ctrlThread
        );

        sch.switchContextTo(THREAD4);// thread4 starts and since he is the first he must be able to aquire the lock and push true
        sch.switchContextTo(CTRLTHREAD);// acquire the lock and then the scheduler reaquire control
        sch.switchContextTo(THREAD2, THREAD0);// thread2 and thread0 aquire control but they cannot aquire the lock so he just push false and finish
        sch.switchContextTo(CTRLTHREAD);// now CTRLTHREAD reaquire the lock and will unlock the mutex and finish
        sch.switchContextTo(THREAD3);// thread3 will now be able to aquire the lock, push true and then switch context
        sch.switchContextTo(THREAD1);// while the lock is aquired by thread3, thread1 wake up and try lock, it will fail, push false and finish
        sch.switchContextTo(THREAD3);// thread3 now wake up again unlock and finish
        sch.joinAll();
    };
}