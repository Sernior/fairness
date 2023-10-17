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

#include <DeterministicConcurrency>
#include <vector>
#include <boost/fairness.hpp>

namespace PM_scenario4{
    using namespace DeterministicConcurrency;

    boost::fairness::priority_mutex<2> m;

    std::vector<int> ret;

    std::vector<int> expected{
        0, 1
    };

    void threadFunction(thread_context* c ,int i) {
        c->lock(&m);
        c->switchContext();
        ret.push_back(i);
        m.unlock();
    }

    void threadFunction2(thread_context* c ,int i) {
        m.unlock();// this unlock should do nothing if m is a mutex and not a semaphore
        c->switchContext();
        c->lock(&m);
        ret.push_back(i);
        m.unlock();
    }

    static size_t THREAD0 = 0;
    static size_t THREAD1 = 1;
    
    static constexpr auto executeSchedulingSequence = []{
        static auto thread_0 = std::tuple{&threadFunction, 0};
        static auto thread_1 = std::tuple{&threadFunction2, 1};

        auto sch = make_UserControlledScheduler(
            thread_0, thread_1
        );

        sch.switchContextTo(THREAD0);
        sch.switchContextTo(THREAD1);
        sch.proceed(THREAD1, THREAD0);// thread 1 here should be stuck on the lock assuming my mutex is a mutex and not a semaphore
        sch.joinAll();
    };
}