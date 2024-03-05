/**
 * @file standard_mutex_benchmark.cpp
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @author F. Abrignani (federignoli@hotmail.it)
 * @brief Alias standard_mutex_benchmark.
 * @version 0.1
 * @date 2024-03-05
 * @private
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
 
//#define BOOST_FAIRNESS_USE_TATAS
//#define BOOST_FAIRNESS_SPINWAIT_SPINS 100
//#define BOOST_FAIRNESS_SPINWAIT_SPINS_RELAXED 0
//#define BOOST_FAIRNESS_GETREQUEST_SPINS 1
//#define BOOST_FAIRNESS_GETREQUEST_SPINS_RELAXED 0
//#define BOOST_FAIRNESS_USE_EXPERIMENTAL_WAIT_NOTIFY
//#define BOOST_FAIRNESS_WAIT_SPINS 10000
//#define BOOST_FAIRNESS_WAIT_SPINS_RELAXED 10000
//#define BOOST_FAIRNESS_MAX_PQNODES 4

#include <benchmark/benchmark.h>
#include "pipeline_benchmark.hpp"

BENCHMARK(standard_mutex_benchmark::lock_unlock_benchmark)->Threads(8);

BENCHMARK_MAIN();
