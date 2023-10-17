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

namespace SPM_scenario2{
    using namespace DeterministicConcurrency;

    boost::fairness::shared_priority_mutex<5> m;

    std::vector<int> ret;

    std::vector<int> expected{
        4, 3, 2, 1, 0
    };

    void threadFunction(thread_context* c ,int i) {
        c->lock(&m);
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
        sch.proceed(// allow thread4 to proceed alone
            THREAD4
        );
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING_EXTERNAL>(// wait until thread4 is waiting for lock
            THREAD4
        );
        sch.switchContextTo(CTRLTHREAD);// at this point the the control thread can end and thread 4 who is the only one in the lock can proceed
        sch.waitUntilAllThreadStatus<thread_status_t::FINISHED>(// wait until thread4 has finished so the lock now is free
            THREAD4
        );
        sch.switchContextTo(THREAD3);
        sch.switchContextTo(THREAD2);
        sch.switchContextTo(THREAD2);
        sch.switchContextTo(THREAD1);
        sch.switchContextTo(THREAD0);
        sch.joinAll();
    };
}