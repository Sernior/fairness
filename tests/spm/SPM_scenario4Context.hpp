/**
 * @file SPM_scenario4Context.hpp
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @author F. Abrignani (federignoli@hotmail.it)
 * @brief Alias SPM_scenario4Context.
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

namespace SPM_scenario4{
    using namespace DeterministicConcurrency;

    boost::fairness::shared_priority_mutex<5> m;
    std::mutex sm;

    std::vector<int> ret;

    std::vector<int> expected{
        0, 4, 1, 2, 3
    };

    void threadFunction(thread_context* c ,int i) {
        c->lock_shared(&m, i);
        c->switchContext();
        {
            std::unique_lock<std::mutex> lock(sm);
            ret.push_back(i);               
        }
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

    static size_t THREAD0 = 0;
    static size_t THREAD1 = 1;
    static size_t THREAD2 = 2;
    static size_t THREAD3 = 3;
    static size_t THREAD4 = 4;
    static size_t CTRLTHREAD = 5;
    
    // 0 4 1 2 3
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