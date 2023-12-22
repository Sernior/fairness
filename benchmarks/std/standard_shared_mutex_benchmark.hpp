/**
 * @file standard_shared_mutex_benchmark.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief Alias standard_shared_mutex_benchmark.
 * @version 0.1
 * @date 2023-10-06
 * @private
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */

#include <shared_mutex>
#include "../utils/thread_utils.hpp"

namespace standard_shared_mutex_benchmark{
    std::shared_mutex m;

    void lock_unlock_benchmark(benchmark::State& state) {
        for (auto _ : state){
            m.lock();
            m.unlock();
        }
    }

    void shared_lock_unlock_benchmark(benchmark::State& state) {
        for (auto _ : state){
            mf.lock_shared();
            mf.unlock_shared();
        }
    }

    void pipeline_benchmark_long(benchmark::State& state) {
        std::array<int, 8> preCT {20, 15, 20, 30, 10, 5, 5, 20};
        int CT = 10;
        std::array<int, 8> postCT {50, 30, 20, 25, 10, 15, 15, 45};

        for (auto _ : state) {
            utils::thread::thread_function<utils::thread::LockLevel::Unique, utils::waiting::WaitingLevel::Milli>(m, preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }

    void shared_pipeline_benchmark_long(benchmark::State& state) {
        std::array<int, 8> preCT {20, 15, 20, 30, 10, 5, 5, 20};
        int CT = 10;
        std::array<int, 8> postCT {50, 30, 20, 25, 10, 15, 15, 45};

        for (auto _ : state) {
            utils::thread::thread_function<utils::thread::LockLevel::Shared, utils::waiting::WaitingLevel::Milli>(m, preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }

    void pipeline_benchmark_gaming(benchmark::State& state) {
        std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
        int CT = 1000;
        std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

        for (auto _ : state) {
            utils::thread::thread_function<utils::thread::LockLevel::Unique, utils::waiting::WaitingLevel::Micro>(m, preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }

    void shared_pipeline_benchmark_gaming(benchmark::State& state) {
        std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
        int CT = 1000;
        std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

        for (auto _ : state) {
            utils::thread::thread_function<utils::thread::LockLevel::Shared, utils::waiting::WaitingLevel::Micro>(m, preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }

    void pipeline_benchmark_audio(benchmark::State& state) {
        std::array<int, 8> prios {0, 1, 2, 1, 3, 2, 2, 0};
        std::array<int, 8> preCT {200, 150, 200, 300, 100, 50, 50, 200};
        int CT = 100;
        std::array<int, 8> postCT {500, 300, 200, 250, 100, 150, 150, 450};

        for (auto _ : state) {
            utils::thread::thread_function<utils::thread::LockLevel::Unique, utils::waiting::WaitingLevel::Micro>(m, preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }

    void shared_pipeline_benchmark_audio(benchmark::State& state) {
        std::array<int, 8> prios {0, 1, 2, 1, 3, 2, 2, 0};
        std::array<int, 8> preCT {200, 150, 200, 300, 100, 50, 50, 200};
        int CT = 100;
        std::array<int, 8> postCT {500, 300, 200, 250, 100, 150, 150, 450};

        for (auto _ : state) {
            utils::thread::thread_function<utils::thread::LockLevel::Shared, utils::waiting::WaitingLevel::Micro>(m, preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }

    void pipeline_benchmark_fast(benchmark::State& state) { /*  */
        std::array<int, 8> prios {0, 1, 2, 1, 3, 2, 2, 0};
        std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
        int CT = 1000;
        std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

        for (auto _ : state) {
            utils::thread::thread_function<utils::thread::LockLevel::Unique, utils::waiting::WaitingLevel::Nano>(m, preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }

    void shared_pipeline_benchmark_fast(benchmark::State& state) { /*  */
        std::array<int, 8> prios {0, 1, 2, 1, 3, 2, 2, 0};
        std::array<int, 8> preCT {2000, 1500, 2000, 3000, 1000, 500, 500, 2000};
        int CT = 1000;
        std::array<int, 8> postCT {5000, 3000, 2000, 2500, 1000, 1500, 1500, 4500};

        for (auto _ : state) {
            utils::thread::thread_function<utils::thread::LockLevel::Shared, utils::waiting::WaitingLevel::Nano>(m, preCT[state.thread_index()], CT, postCT[state.thread_index()]);
        }
    }
}