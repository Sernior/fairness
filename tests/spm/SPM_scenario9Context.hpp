//          Copyright © Federico Abrignani 2023 - 2024.
//          Copyright © Salvatore Martorana 2023 - 2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt) 

/**
 * @file SPM_scenario9Context.hpp
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @author F. Abrignani (federignoli@hotmail.it)
 * @brief Alias SPM_scenario9Context.
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
#include <random>
#include <mutex>

namespace SPM_scenario9{
    using namespace DeterministicConcurrency;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, 6);

    void threadFunction(thread_context* c, int i, boost::fairness::shared_priority_mutex<7>& m) {
        c->lock_shared(&m, i);
        c->switchContext();
        m.unlock_shared();
    }

    void controlThread(thread_context* c, boost::fairness::shared_priority_mutex<7>& m) {
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
        boost::fairness::shared_priority_mutex<7> m;
        
        auto thread_0 = std::tuple{&threadFunction, distribution(gen), std::ref(m)};
        auto thread_1 = std::tuple{&threadFunction, distribution(gen), std::ref(m)};
        auto thread_2 = std::tuple{&threadFunction, distribution(gen), std::ref(m)};
        auto thread_3 = std::tuple{&threadFunction, distribution(gen), std::ref(m)};
        auto ctrlThread = std::tuple{&controlThread, std::ref(m)};
        
        auto sch = make_UserControlledScheduler(
            thread_0, thread_1, thread_2, thread_3, ctrlThread
        );

        sch.switchContextTo(CTRLTHREAD);
        sch.proceed(THREAD0, THREAD1, THREAD2, THREAD3);
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING_EXTERNAL>(THREAD0, THREAD1, THREAD2, THREAD3);
        sch.switchContextTo(CTRLTHREAD);
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING>(THREAD0, THREAD1, THREAD2, THREAD3); // if the lock_shared wouldn`t be "shared" this instruction would cause deadlock.
        sch.proceed(THREAD0, THREAD1, THREAD2, THREAD3);
        sch.joinAll();
    };
}