#include <priority_mutex.h>
#include <fair_shared_mutex.h>
#include <DeterministicConcurrency>
#include <vector>
namespace scenario1{
    using namespace DeterministicConcurrency;

    PrioSync::priority_mutex<5> m;

    std::vector<int> ret;

    std::vector<int> expected{
        0, 1, 2, 3, 4
    };


    void threadFunction(thread_context* c ,int i) {
        c->uniqueLock(&m,i);
        ret.push_back(i);
        m.unlock();
    }

    void controlThread(thread_context* c) {
        c->uniqueLock(&m);
        c->switchContext();
        m.unlock();
    }

    auto thread_0 = std::tuple{&threadFunction, 0};
    auto thread_1 = std::tuple{&threadFunction, 1};
    auto thread_2 = std::tuple{&threadFunction, 2};
    auto thread_3 = std::tuple{&threadFunction, 3};
    auto thread_4 = std::tuple{&threadFunction, 4};
    auto ctrlThread = std::tuple{&controlThread};

    size_t THREAD1 = 0;
    size_t THREAD2 = 1;
    size_t THREAD3 = 2;
    size_t THREAD4 = 3;
    size_t THREAD5 = 4;
    size_t CTRLTHREAD = 5;

}