/**
 * @file #TODO.hpp
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief Alias #TODO.
 * @version 0.1
 * @date 2023-10-06
 * @private
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */

#include <boost/fairness.hpp>
#include "../utils/thread_utils.hpp"

#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
namespace slim_spinlock_priority_mutex_benchmark{

    boost::fairness::experimental::slim_spinlock_priority_mutex<5> m;

    void SPNLC_SLM_PM_LockUnlock(benchmark::State& state) { /* the non slim version is better for spinlocking */
        boost::fairness::experimental::slim_spinlock_priority_mutex mf;

        for (auto _ : state){
            mf.lock();
            mf.unlock();
        }
    }

    void SPNLC_SLM_PM_pipeline_benchmark_long(benchmark::State& state) {// order of 1/10th of a second (PM faster)
        std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
        std::array<int, 8> preCT {20, 15, 20, 30, 10, 5, 5, 20};
        int CT = 10;
        std::array<int, 8> postCT {50, 30, 20, 25, 10, 15, 15, 45};

        for (auto _ : state) {
            utils::thread::thread_function<utils::thread::LockLevel::Unique, utils::waiting::WaitingLevel::Milli>(m, preCT[state.thread_index()], CT, postCT[state.thread_index()], prios[state.thread_index()]);
        }
    }

    void SPNLC_SLM_PM_pipeline_benchmark_gaming(benchmark::State& state) {// order of 10 to 15 milliseconds (PM faster)
        std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
        std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
        int CT = 1000;
        std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

        for (auto _ : state) {
            utils::thread::thread_function<utils::thread::LockLevel::Unique, utils::waiting::WaitingLevel::Micro>(m, preCT[state.thread_index()], CT, postCT[state.thread_index()], prios[state.thread_index()]);
        }
    }

    void SPNLC_SLM_PM_pipeline_benchmark_audio(benchmark::State& state) {
        std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
        std::array<int, 8> preCT {200, 150, 200, 300, 100, 50, 50, 200};
        int CT = 100;
        std::array<int, 8> postCT {500, 300, 200, 250, 100, 150, 150, 450};

        for (auto _ : state) {
            utils::thread::thread_function<utils::thread::LockLevel::Unique, utils::waiting::WaitingLevel::Micro>(m, preCT[state.thread_index()], CT, postCT[state.thread_index()], prios[state.thread_index()]);
        }
    }

    void SPNLC_SLM_pipeline_benchmark_fast(benchmark::State& state) {
        std::array<int, 8> prios {0, 2, 2, 1, 1, 3, 3, 0};
        std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
        int CT = 1000;
        std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

        for (auto _ : state) {
            utils::thread::thread_function<utils::thread::LockLevel::Unique, utils::waiting::WaitingLevel::Nano>(m, preCT[state.thread_index()], CT, postCT[state.thread_index()], prios[state.thread_index()]);
        }
    }
}
#endif