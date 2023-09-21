include("../cmake/GoogleTest.cmake")
include("../cmake/DeterministicConcurrency.cmake")

add_executable(fsm_test
test.cpp
PM_scenario1Context.h
PM_scenario2Context.h
PM_scenario3Context.h
PM_scenario4Context.h
PM_scenario5Context.h
SLM_scenario1Context.h
SLM_scenario2Context.h
SLM_scenario3Context.h
SLM_scenario4Context.h
SPNLCPM_scenario1Context.h
SPNLCPM_scenario2Context.h
SPNLCPM_scenario3Context.h
SPNLCPM_scenario4Context.h
SPM_scenario1Context.h
SPM_scenario2Context.h
SPM_scenario3Context.h
SPM_scenario4Context.h
SPM_scenario5Context.h
SPM_scenario6Context.h
)

target_compile_features(fsm_test PUBLIC cxx_std_20)

target_link_libraries(fsm_test gtest fsm deterministic_concurrency)