/**
 * @file benchmark.cpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief Alias benchmark.
 * @version 0.1
 * @date 2023-10-06
 * @private
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
//#define BOOST_FAIRNESS_USE_TATAS
#define BOOST_FAIRNESS_SPINWAIT_SPINS 16
#define BOOST_FAIRNESS_SPINWAIT_SPINS_RELAXED 12
#define BOOST_FAIRNESS_GETREQUEST_SPINS 1
#define BOOST_FAIRNESS_GETREQUEST_SPINS_RELAXED 0
//#define BOOST_FAIRNESS_USE_EXPERIMENTAL_WAIT_NOTIFY
#define BOOST_FAIRNESS_MAX_PQNODES 4
#include <benchmark/benchmark.h>
#include "pipeline_benchmark.hpp"

//BENCHMARK(utils::waiting::busy_wait_50milli_benchmark);
//BENCHMARK(utils::waiting::busy_wait_50micro_benchmark);
//BENCHMARK(utils::waiting::busy_wait_50nano_benchmark);
//BENCHMARK(____________________________________________________________________________________________);
//BENCHMARK(priority_mutex_benchmark::pq_lock_unlock_benchmark)->Threads(8);
BENCHMARK(priority_mutex_benchmark::lock_unlock_benchmark)->Threads(8);
//BENCHMARK(standard_mutex_benchmark::lock_unlock_benchmark)->Threads(8);
//BENCHMARK(spinlock_priority_mutex_benchmark::lock_unlock_benchmark)->Threads(8);
/*
BENCHMARK(slim_priority_mutex_benchmark::lock_unlock_benchmark)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
#endif
BENCHMARK(recursive_priority_mutex_benchmark::lock_unlock_benchmark)->Threads(8);
BENCHMARK(standard_recursive_mutex_benchmark::lock_unlock_benchmark)->Threads(8);
BENCHMARK(shared_priority_mutex_benchmark::lock_unlock_benchmark)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::lock_unlock_benchmark)->Threads(8);
BENCHMARK(shared_priority_mutex_benchmark::shared_lock_unlock_benchmark)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::shared_lock_unlock_benchmark)->Threads(8);
BENCHMARK(____________________________________________________________________________________________);
BENCHMARK(priority_mutex_benchmark::pipeline_benchmark_long)->Threads(8);
BENCHMARK(standard_mutex_benchmark::pipeline_benchmark_long)->Threads(8);
BENCHMARK(slim_priority_mutex_benchmark::pipeline_benchmark_long)->Threads(8);
//BENCHMARK(spinlock_priority_mutex_benchmark::pipeline_benchmark_long)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
#endif
BENCHMARK(recursive_priority_mutex_benchmark::pipeline_benchmark_long)->Threads(8);
BENCHMARK(standard_recursive_mutex_benchmark::pipeline_benchmark_long)->Threads(8);
BENCHMARK(shared_priority_mutex_benchmark::pipeline_benchmark_long)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::pipeline_benchmark_long)->Threads(8);
BENCHMARK(shared_priority_mutex_benchmark::shared_pipeline_benchmark_long)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::shared_pipeline_benchmark_long)->Threads(8);
BENCHMARK(____________________________________________________________________________________________);
BENCHMARK(priority_mutex_benchmark::pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(standard_mutex_benchmark::pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(slim_priority_mutex_benchmark::pipeline_benchmark_gaming)->Threads(8);
//BENCHMARK(spinlock_priority_mutex_benchmark::pipeline_benchmark_gaming)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
#endif
BENCHMARK(recursive_priority_mutex_benchmark::pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(standard_recursive_mutex_benchmark::pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(shared_priority_mutex_benchmark::pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(shared_priority_mutex_benchmark::shared_pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::shared_pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(____________________________________________________________________________________________);
BENCHMARK(priority_mutex_benchmark::pipeline_benchmark_audio)->Threads(8);
BENCHMARK(standard_mutex_benchmark::pipeline_benchmark_audio)->Threads(8);
BENCHMARK(slim_priority_mutex_benchmark::pipeline_benchmark_audio)->Threads(8);
//BENCHMARK(spinlock_priority_mutex_benchmark::pipeline_benchmark_audio)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
#endif
BENCHMARK(recursive_priority_mutex_benchmark::pipeline_benchmark_audio)->Threads(8);
BENCHMARK(standard_recursive_mutex_benchmark::pipeline_benchmark_audio)->Threads(8);
BENCHMARK(shared_priority_mutex_benchmark::pipeline_benchmark_audio)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::pipeline_benchmark_audio)->Threads(8);
BENCHMARK(shared_priority_mutex_benchmark::shared_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::shared_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(____________________________________________________________________________________________);
BENCHMARK(priority_mutex_benchmark::pipeline_benchmark_fast)->Threads(8);
BENCHMARK(standard_mutex_benchmark::pipeline_benchmark_fast)->Threads(8);
BENCHMARK(slim_priority_mutex_benchmark::pipeline_benchmark_fast)->Threads(8);
//BENCHMARK(spinlock_priority_mutex_benchmark::pipeline_benchmark_fast)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
#endif
BENCHMARK(recursive_priority_mutex_benchmark::pipeline_benchmark_fast)->Threads(8);
BENCHMARK(standard_recursive_mutex_benchmark::pipeline_benchmark_fast)->Threads(8);
BENCHMARK(shared_priority_mutex_benchmark::pipeline_benchmark_fast)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::pipeline_benchmark_fast)->Threads(8);
BENCHMARK(shared_priority_mutex_benchmark::shared_pipeline_benchmark_fast)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::shared_pipeline_benchmark_fast)->Threads(8);*/
BENCHMARK_MAIN();
