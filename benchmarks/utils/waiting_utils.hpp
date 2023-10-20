/**
 * @file waiting_utils.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief Alias waiting_utils.
 * @version 0.1
 * @date 2023-10-06
 * @private
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */


#ifndef BOOST_FAIRNESS_WAITING_UTILS_HPP
#define BOOST_FAIRNESS_WAITING_UTILS_HPP
#include <chrono>

#define NOW std::chrono::high_resolution_clock::now()

namespace utils::waiting{
    enum class WaitingLevel { Milli, Micro, Nano };

    /*
    std::this_thread::sleep_for is too imprecise, and also I could make a bunch of variables during busy waiting to simulate heated cpu caches.
    */

    template <WaitingLevel level>
    void busy_wait(uint32_t utime) {
        int i = 0;
        auto begin = NOW;
        if constexpr (level == WaitingLevel::Milli)
            for(;std::chrono::duration_cast<std::chrono::milliseconds>(NOW - begin).count() < utime;){
                benchmark::DoNotOptimize(i += 1);
            }
        if constexpr (level == WaitingLevel::Micro)
            for(;std::chrono::duration_cast<std::chrono::microseconds>(NOW - begin).count() < utime;){
                benchmark::DoNotOptimize(i += 1);
            }
        if constexpr (level == WaitingLevel::Nano)
            for(;std::chrono::duration_cast<std::chrono::nanoseconds>(NOW - begin).count() < utime;){
                benchmark::DoNotOptimize(i += 1);
            }
    }

    void busy_wait_50milli_benchmark(benchmark::State& state) {
        for (auto _ : state){
            busy_wait<WaitingLevel::Milli>(50);
        }
    }

    void busy_wait_50micro_benchmark(benchmark::State& state) {
        for (auto _ : state){
            busy_wait<WaitingLevel::Micro>(50);
        }
    }

    void busy_wait_50nano_benchmark(benchmark::State& state) {
        for (auto _ : state){
            busy_wait<WaitingLevel::Nano>(50);
        }
    }

}

#undef NOW
#endif // BOOST_FAIRNESS_WAITING_UTILS_HPP