/**
 * @file RPM_scenario5Context.hpp
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @author F. Abrignani (federignoli@hotmail.it)
 * @brief Alias RPM_scenario5Context.
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

namespace RPM_scenario5{
    using namespace DeterministicConcurrency;

    std::vector<int> ret;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, 6);

    void threadFunction(thread_context* c, int i, boost::fairness::recursive_priority_mutex<7>& m) {
        c->lock(&m,i);
        ret.push_back(i);
        m.unlock();
    }

    void controlThread(thread_context* c, boost::fairness::recursive_priority_mutex<7>& m) {
        c->lock(&m);
        c->switchContext();
        m.unlock();
    }

    static size_t THREAD0 = 0;
    static size_t THREAD1 = 1;
    static size_t THREAD2 = 2;
    static size_t THREAD3 = 3;
    static size_t THREAD4 = 4;
    static size_t THREAD5 = 5;
    static size_t THREAD6 = 6;
    static size_t THREAD7 = 7;
    static size_t CTRLTHREAD = 8;
        
    static constexpr auto executeSchedulingSequence = []{
        boost::fairness::recursive_priority_mutex<7> m;
    
        auto thread_0 = std::tuple{&threadFunction, distribution(gen), std::ref(m)};
        auto thread_1 = std::tuple{&threadFunction, distribution(gen), std::ref(m)};
        auto thread_2 = std::tuple{&threadFunction, distribution(gen), std::ref(m)};
        auto thread_3 = std::tuple{&threadFunction, distribution(gen), std::ref(m)};
        auto thread_4 = std::tuple{&threadFunction, distribution(gen), std::ref(m)};
        auto thread_5 = std::tuple{&threadFunction, distribution(gen), std::ref(m)};
        auto thread_6 = std::tuple{&threadFunction, distribution(gen), std::ref(m)};
        auto thread_7 = std::tuple{&threadFunction, distribution(gen), std::ref(m)};
        auto ctrlThread = std::tuple{&controlThread, std::ref(m)};
        
        auto sch = make_UserControlledScheduler(
            thread_0, thread_1, thread_2, thread_3, thread_4, thread_5, thread_6, thread_7, ctrlThread
        );
        
        sch.switchContextTo(CTRLTHREAD);// give control to the control thread first so it acquires the lock
        sch.proceed(// allow all the other thread to proceed
            THREAD0,
            THREAD1,
            THREAD2,
            THREAD3,
            THREAD4,
            THREAD5,
            THREAD6,
            THREAD7
        );
        sch.waitUntilAllThreadStatus<thread_status_t::WAITING_EXTERNAL>(// wait until all of them are waiting on the lock
            THREAD0,
            THREAD1,
            THREAD2,
            THREAD3,
            THREAD4,
            THREAD5,
            THREAD6,
            THREAD7
        );
        sch.switchContextTo(CTRLTHREAD);// at this point the the control thread can end and all the others should respect their priority
        sch.joinAll();
    };
}