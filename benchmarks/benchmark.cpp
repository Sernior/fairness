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

#include <benchmark/benchmark.h>
#include "pipeline_benchmark.hpp"

BENCHMARK(priority_mutex_benchmark::PM_LockUnlock)->Threads(8);
BENCHMARK(standard_mutex_benchmark::STD_LockUnlock)->Threads(8);
BENCHMARK(slim_priority_mutex_benchmark::SLM_PM_LockUnlock)->Threads(8);
BENCHMARK(spinlock_priority_mutex_benchmark::SPNLC_PM_LockUnlock)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
BENCHMARK(slim_spinlock_priority_mutex_benchmark::SPNLC_SLM_PM_LockUnlock)->Threads(8);
#endif
BENCHMARK(recursive_priority_mutex_benchmark::R_PM_LockUnlock)->Threads(8);
BENCHMARK(standard_recursive_mutex_benchmark::R_STD_LockUnlock)->Threads(8);
BENCHMARK(shared_priority_mutex_benchmark::PM_S_LockUnlock)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::STD_S_LockUnlock)->Threads(8);
BENCHMARK(shared_priority_mutex_benchmark::PM_S_SLockSUnlock)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::STD_S_SLockSUnlock)->Threads(8);
BENCHMARK(____________________________________________________________________________________________);
BENCHMARK(priority_mutex_benchmark::PM_pipeline_benchmark_long)->Threads(8);
BENCHMARK(standard_mutex_benchmark::STD_pipeline_benchmark_long)->Threads(8);
BENCHMARK(slim_priority_mutex_benchmark::SLM_PM_pipeline_benchmark_long)->Threads(8);
BENCHMARK(spinlock_priority_mutex_benchmark::SPNLC_PM_pipeline_benchmark_long)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
BENCHMARK(slim_spinlock_priority_mutex_benchmark::SPNLC_SLM_PM_pipeline_benchmark_long)->Threads(8);
#endif
BENCHMARK(recursive_priority_mutex_benchmark::R_PM_pipeline_benchmark_long)->Threads(8);
BENCHMARK(standard_recursive_mutex_benchmark::R_STD_pipeline_benchmark_long)->Threads(8);

BENCHMARK(shared_priority_mutex_benchmark::PM_S_pipeline_benchmark_long)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::STD_S_pipeline_benchmark_long)->Threads(8);
BENCHMARK(shared_priority_mutex_benchmark::PM_S_Spipeline_benchmark_long)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::STD_S_Spipeline_benchmark_long)->Threads(8);
BENCHMARK(____________________________________________________________________________________________);
BENCHMARK(priority_mutex_benchmark::PM_pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(standard_mutex_benchmark::STD_pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(slim_priority_mutex_benchmark::SLM_PM_pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(spinlock_priority_mutex_benchmark::SPNLC_PM_pipeline_benchmark_gaming)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
BENCHMARK(slim_spinlock_priority_mutex_benchmark::SPNLC_SLM_PM_pipeline_benchmark_gaming)->Threads(8);
#endif
BENCHMARK(recursive_priority_mutex_benchmark::R_PM_pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(standard_recursive_mutex_benchmark::R_STD_pipeline_benchmark_gaming)->Threads(8);

BENCHMARK(shared_priority_mutex_benchmark::PM_S_pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::STD_S_pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(shared_priority_mutex_benchmark::PM_S_Spipeline_benchmark_gaming)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::STD_S_Spipeline_benchmark_gaming)->Threads(8);
BENCHMARK(____________________________________________________________________________________________);
BENCHMARK(priority_mutex_benchmark::PM_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(standard_mutex_benchmark::STD_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(slim_priority_mutex_benchmark::SLM_PM_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(spinlock_priority_mutex_benchmark::SPNLC_PM_pipeline_benchmark_audio)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
BENCHMARK(slim_spinlock_priority_mutex_benchmark::SPNLC_SLM_PM_pipeline_benchmark_audio)->Threads(8);
#endif
BENCHMARK(recursive_priority_mutex_benchmark::R_PM_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(standard_recursive_mutex_benchmark::R_STD_pipeline_benchmark_audio)->Threads(8);

BENCHMARK(shared_priority_mutex_benchmark::PM_S_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::STD_S_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(shared_priority_mutex_benchmark::PM_S_Spipeline_benchmark_audio)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::STD_S_Spipeline_benchmark_audio)->Threads(8);
BENCHMARK(____________________________________________________________________________________________);
BENCHMARK(priority_mutex_benchmark::PM_pipeline_benchmark_fast)->Threads(8);
BENCHMARK(standard_mutex_benchmark::STD_pipeline_benchmark_fast)->Threads(8);
BENCHMARK(slim_priority_mutex_benchmark::SLM_PM_pipeline_benchmark_fast)->Threads(8);
BENCHMARK(spinlock_priority_mutex_benchmark::SPNLC_PM_pipeline_benchmark_fast)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
BENCHMARK(slim_spinlock_priority_mutex_benchmark::SPNLC_SLM_pipeline_benchmark_fast)->Threads(8);
#endif
BENCHMARK(recursive_priority_mutex_benchmark::R_PM_pipeline_benchmark_fast)->Threads(8);
BENCHMARK(standard_recursive_mutex_benchmark::R_STD_pipeline_benchmark_fast)->Threads(8);

BENCHMARK(shared_priority_mutex_benchmark::PM_S_pipeline_benchmark_fast)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::STD_S_pipeline_benchmark_fast)->Threads(8);
BENCHMARK(shared_priority_mutex_benchmark::PM_S_Spipeline_benchmark_fast)->Threads(8);
BENCHMARK(standard_shared_mutex_benchmark::STD_S_Spipeline_benchmark_fast)->Threads(8);

BENCHMARK_MAIN();
