/**
 * @file SPM_scenario5Context.hpp
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @author F. Abrignani (federignoli@hotmail.it)
 * @brief Alias SPM_scenario5Context.
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

namespace SPM_scenario5{
    using namespace DeterministicConcurrency;

    std::vector<int> ret;

    std::vector<int> expected{
        0, 1
    };

    void threadFunction(thread_context* c , int i, boost::fairness::shared_priority_mutex<2>& m) {
        c->lock(&m);
        c->switchContext();
        ret.push_back(i);
        m.unlock();
    }

    void threadFunction2(thread_context* c , int i, boost::fairness::shared_priority_mutex<2>& m) {
        m.unlock();// this unlock should do nothing if m is a mutex and not a semaphore
        c->switchContext();
        c->lock(&m);
        ret.push_back(i);
        m.unlock();
    }

    static size_t THREAD0 = 0;
    static size_t THREAD1 = 1;
    
    static constexpr auto executeSchedulingSequence = []{
        boost::fairness::shared_priority_mutex<2> m;
    
        auto thread_0 = std::tuple{&threadFunction, 0, std::ref(m)};
        auto thread_1 = std::tuple{&threadFunction2, 1, std::ref(m)};

        auto sch = make_UserControlledScheduler(
            thread_0, thread_1
        );

        sch.switchContextTo(THREAD0);
        sch.switchContextTo(THREAD1);
        sch.proceed(THREAD1, THREAD0);// thread 1 here should be stuck on the lock assuming the mutex "m" is a mutex and not a semaphore
        sch.joinAll();
    };
}