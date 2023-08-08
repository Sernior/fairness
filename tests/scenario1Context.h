#include <fair_shared_mutex.h>
#include <DeterministicConcurrency>
#include <vector>
namespace scenario1{
    using namespace DeterministicConcurrency;

    static fsm::fair_shared_mutex m;

    static std::vector<int> ret;

    static std::vector<int> expected{
        0, 1, 2, 3, 4
    };


    void threadFunction(thread_context* c ,int i) {
        c->tryLock([i]{m.lock(i);});
        ret.push_back(i);
        m.unlock();
    }

    void controlThread(thread_context* c) {
        c->tryLock([]{m.lock();});
        c->switchContext();
        m.unlock();
    }

    static auto thread_0 = std::tuple{&threadFunction, 0};
    static auto thread_1 = std::tuple{&threadFunction, 1};
    static auto thread_2 = std::tuple{&threadFunction, 2};
    static auto thread_3 = std::tuple{&threadFunction, 3};
    static auto thread_4 = std::tuple{&threadFunction, 4};
    static auto ctrlThread = std::tuple{&controlThread};

    static size_t THREAD1 = 0;
    static size_t THREAD2 = 1;
    static size_t THREAD3 = 2;
    static size_t THREAD4 = 3;
    static size_t THREAD5 = 4;
    static size_t CTRLTHREAD = 5;

    static auto sch = DeterministicConcurrency::make_UserControlledScheduler(
        thread_0, thread_1, thread_2, thread_3, thread_4, ctrlThread
    );

}