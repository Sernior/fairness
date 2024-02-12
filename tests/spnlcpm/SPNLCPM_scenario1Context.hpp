//          Copyright © Federico Abrignani 2023 - 2024.
//          Copyright © Salvatore Martorana 2023 - 2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt) 

/**
 * @file SPNLCPM_scenario1Context.hpp
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @author F. Abrignani (federignoli@hotmail.it)
 * @brief Alias SPNLCPM_scenario1Context.
 * @version 0.1
 * @date 2023-10-06
 * @private
 * @copyright Copyright © 2023 - 2024 Salvatore Martorana (salvatoremartorana@hotmail.com).
 * @copyright Copyright © 2023 - 2024 Federico Abrignani (federignoli@hotmail.it).
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */

#include <boost/fairness.hpp>
#include <DeterministicConcurrency>
#include <vector>

namespace SPNLCPM_scenario1{
    using namespace DeterministicConcurrency;

    std::vector<int> ret;

    std::vector<int> expected{
        0, 1, 2, 3, 4
    };

    void threadFunction(thread_context* c ,int i, boost::fairness::spinlock_priority_mutex<5>& m) {
        c->lock(&m,i);
        ret.push_back(i);
        m.unlock();
    }

    void controlThread(thread_context* c, boost::fairness::spinlock_priority_mutex<5>& m) {
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
        boost::fairness::spinlock_priority_mutex<5> m;
    
        auto thread_0 = std::tuple{&threadFunction, 0, std::ref(m)};
        auto thread_1 = std::tuple{&threadFunction, 1, std::ref(m)};
        auto thread_2 = std::tuple{&threadFunction, 2, std::ref(m)};
        auto thread_3 = std::tuple{&threadFunction, 3, std::ref(m)};
        auto thread_4 = std::tuple{&threadFunction, 4, std::ref(m)};
        auto ctrlThread = std::tuple{&controlThread, std::ref(m)};

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