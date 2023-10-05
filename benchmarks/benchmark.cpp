#include <boost/fairness.hpp>
#include <benchmark/benchmark.h>
#include <mutex>
#include <shared_mutex>
#include "pipeline_benchmark.hpp"
#include <DeterministicConcurrency>

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
