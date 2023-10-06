/**
 * @file #TODO.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana
 * @brief Alias #TODO.
 * @version 0.1
 * @date 2023-10-06
 * @private
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */

#include <benchmark/benchmark.h>
#include "pipeline_benchmark.hpp"

BENCHMARK(_PM_pipeline_benchmark::PM_LockUnlock)->Threads(8);
BENCHMARK(_STD_pipeline_benchmark::STD_LockUnlock)->Threads(8);
BENCHMARK(_SLM_PM_pipeline_benchmark::SLM_PM_LockUnlock)->Threads(8);
BENCHMARK(_SPNLC_PM_pipeline_benchmark::SPNLC_PM_LockUnlock)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
BENCHMARK(_SPNLC_SLM_PM_pipeline_benchmark::SPNLC_SLM_PM_LockUnlock)->Threads(8);
#endif
BENCHMARK(_PM_R_pipeline_benchmark::R_PM_LockUnlock)->Threads(8);
BENCHMARK(_PM_S_pipeline_benchmark::PM_S_LockUnlock)->Threads(8);
BENCHMARK(_STD_S_pipeline_benchmark::STD_S_LockUnlock)->Threads(8);
BENCHMARK(_PM_S_pipeline_benchmark::PM_S_SLockSUnlock)->Threads(8);
BENCHMARK(_STD_S_pipeline_benchmark::STD_S_SLockSUnlock)->Threads(8);

BENCHMARK(_PM_pipeline_benchmark::PM_pipeline_benchmark_long)->Threads(8);
BENCHMARK(_STD_pipeline_benchmark::STD_pipeline_benchmark_long)->Threads(8);
BENCHMARK(_SLM_PM_pipeline_benchmark::SLM_PM_pipeline_benchmark_long)->Threads(8);
BENCHMARK(_SPNLC_PM_pipeline_benchmark::SPNLC_PM_pipeline_benchmark_long)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
BENCHMARK(_SPNLC_SLM_PM_pipeline_benchmark::SPNLC_SLM_PM_pipeline_benchmark_long)->Threads(8);
#endif
BENCHMARK(_PM_R_pipeline_benchmark::R_PM_pipeline_benchmark_long)->Threads(8);
BENCHMARK(_PM_S_pipeline_benchmark::PM_S_pipeline_benchmark_long)->Threads(8);
BENCHMARK(_STD_S_pipeline_benchmark::STD_S_pipeline_benchmark_long)->Threads(8);
BENCHMARK(_PM_S_pipeline_benchmark::PM_S_Spipeline_benchmark_long)->Threads(8);
BENCHMARK(_STD_S_pipeline_benchmark::STD_S_Spipeline_benchmark_long)->Threads(8);

BENCHMARK(_PM_pipeline_benchmark::PM_pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(_STD_pipeline_benchmark::STD_pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(_SLM_PM_pipeline_benchmark::SLM_PM_pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(_SPNLC_PM_pipeline_benchmark::SPNLC_PM_pipeline_benchmark_gaming)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
BENCHMARK(_SPNLC_SLM_PM_pipeline_benchmark::SPNLC_SLM_PM_pipeline_benchmark_gaming)->Threads(8);
#endif
BENCHMARK(_PM_R_pipeline_benchmark::R_PM_pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(_PM_S_pipeline_benchmark::PM_S_pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(_STD_S_pipeline_benchmark::STD_S_pipeline_benchmark_gaming)->Threads(8);
BENCHMARK(_PM_S_pipeline_benchmark::PM_S_Spipeline_benchmark_gaming)->Threads(8);
BENCHMARK(_STD_S_pipeline_benchmark::STD_S_Spipeline_benchmark_gaming)->Threads(8);

BENCHMARK(_PM_pipeline_benchmark::PM_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(_STD_pipeline_benchmark::STD_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(_SLM_PM_pipeline_benchmark::SLM_PM_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(_SPNLC_PM_pipeline_benchmark::SPNLC_PM_pipeline_benchmark_audio)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
BENCHMARK(_SPNLC_SLM_PM_pipeline_benchmark::SPNLC_SLM_PM_pipeline_benchmark_audio)->Threads(8);
#endif
BENCHMARK(_PM_R_pipeline_benchmark::R_PM_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(_PM_S_pipeline_benchmark::PM_S_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(_STD_S_pipeline_benchmark::STD_S_pipeline_benchmark_audio)->Threads(8);
BENCHMARK(_PM_S_pipeline_benchmark::PM_S_Spipeline_benchmark_audio)->Threads(8);
BENCHMARK(_STD_S_pipeline_benchmark::STD_S_Spipeline_benchmark_audio)->Threads(8);

BENCHMARK(_PM_pipeline_benchmark::PM_pipeline_benchmark_fast)->Threads(8);
BENCHMARK(_STD_pipeline_benchmark::STD_pipeline_benchmark_fast)->Threads(8);
BENCHMARK(_SLM_PM_pipeline_benchmark::SLM_PM_pipeline_benchmark_fast)->Threads(8);
BENCHMARK(_SPNLC_PM_pipeline_benchmark::SPNLC_PM_pipeline_benchmark_fast)->Threads(8);
#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
BENCHMARK(_SPNLC_SLM_PM_pipeline_benchmark::SPNLC_SLM_pipeline_benchmark_fast)->Threads(8);
#endif
BENCHMARK(_PM_R_pipeline_benchmark::R_PM_pipeline_benchmark_fast)->Threads(8);
BENCHMARK(_PM_S_pipeline_benchmark::PM_S_pipeline_benchmark_fast)->Threads(8);
BENCHMARK(_STD_S_pipeline_benchmark::STD_S_pipeline_benchmark_fast)->Threads(8);
BENCHMARK(_PM_S_pipeline_benchmark::PM_S_Spipeline_benchmark_fast)->Threads(8);
BENCHMARK(_STD_S_pipeline_benchmark::STD_S_Spipeline_benchmark_fast)->Threads(8);

BENCHMARK_MAIN();
