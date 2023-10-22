/**
 * @file #TODO.hpp
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @author F. Abrignani (federignoli@hotmail.it)
 * @brief Alias #TODO.
 * @version 0.1
 * @date 2023-10-06
 * @private
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */

#include <boost/fairness.hpp>
#include <DeterministicConcurrency>
#include <vector>
#include <random>
#include <mutex>

namespace SPM_scenario9{
    using namespace DeterministicConcurrency;

    boost::fairness::shared_priority_mutex<7> m;
    std::mutex sm;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, 6);

    void threadFunction(thread_context* c ,int i) {
        c->lock_shared(&m, i);
        c->switchContext();
        m.unlock_shared();
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
    static size_t CTRLTHREAD = 4;
        
    static constexpr auto executeSchedulingSequence = []{
        auto thread_0 = std::tuple{&threadFunction, distribution(gen)};
        auto thread_1 = std::tuple{&threadFunction, distribution(gen)};
        auto thread_2 = std::tuple{&threadFunction, distribution(gen)};
        auto thread_3 = std::tuple{&threadFunction, distribution(gen)};
        auto ctrlThread = std::tuple{&controlThread};
        
        auto sch = make_UserControlledScheduler(
            thread_0, thread_1, thread_2, thread_3, ctrlThread
        );

        sch.switchContextTo(CTRLTHREAD);// acquire the lock and then the scheduler reaquire control
        sch.proceed(THREAD0, THREAD1, THREAD2, THREAD3);
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING_EXTERNAL>(THREAD0, THREAD1, THREAD2, THREAD3);// wait until 0 and 4 are stuck on the lock_shared
        sch.switchContextTo(CTRLTHREAD);// now you unlock the mutex so 0 since it has higher priority must go first
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING>(THREAD0, THREAD1, THREAD2, THREAD3);// so now both 0 and 4 can reach their next switch context
        sch.proceed(THREAD0, THREAD1, THREAD2, THREAD3);// now everyone else decide to start 1 and 2 can get in since they are also shared and 4 is still holding shared but 3 wait on the lock
        sch.joinAll();
    };
}