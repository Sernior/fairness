//          Copyright © Federico Abrignani 2023 - 2024.
//          Copyright © Salvatore Martorana 2023 - 2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt) 

/**
 * @file SPM_scenario3Context.hpp
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @author F. Abrignani (federignoli@hotmail.it)
 * @brief Alias SPM_scenario3Context.
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

namespace SPM_scenario3{
    using namespace DeterministicConcurrency;

    std::vector<bool> ret;

    std::vector<bool> expected{
        true, false, false, true, false
    };

    void threadFunction(thread_context* c , int i, boost::fairness::shared_priority_mutex<5>& m) {
        if (m.try_lock(i)){
            ret.push_back(true);
            m.unlock();
            return;
        }
        ret.push_back(false);
    }

    void threadFunction2(thread_context* c , int i, boost::fairness::shared_priority_mutex<5>& m) {
        if (m.try_lock(i)){
            ret.push_back(true);
            c->switchContext();
            m.unlock();
            return;
        }
        ret.push_back(false);
    }

    void controlThread(thread_context* c, boost::fairness::shared_priority_mutex<5>& m) {
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
        boost::fairness::shared_priority_mutex<5> m;
    
        auto thread_0 = std::tuple{&threadFunction, 0, std::ref(m)};
        auto thread_1 = std::tuple{&threadFunction, 1, std::ref(m)};
        auto thread_2 = std::tuple{&threadFunction, 2, std::ref(m)};
        auto thread_3 = std::tuple{&threadFunction2, 3, std::ref(m)};
        auto thread_4 = std::tuple{&threadFunction, 4, std::ref(m)};
        auto ctrlThread = std::tuple{&controlThread, std::ref(m)};

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