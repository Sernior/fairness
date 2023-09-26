include("../cmake/GoogleTest.cmake")
include("../cmake/DeterministicConcurrency.cmake")

add_executable(fsm_test
test.cpp
scenarios.hpp
# pm/PM_scenario1Context.h
# pm/PM_scenario2Context.h
# pm/PM_scenario3Context.h
# pm/PM_scenario4Context.h
# pm/PM_scenario5Context.h
# slm/SLM_scenario1Context.h
# slm/SLM_scenario2Context.h
# slm/SLM_scenario3Context.h
# slm/SLM_scenario4Context.h
# spnlcpm/SPNLCPM_scenario1Context.h
# spnlcpm/SPNLCPM_scenario2Context.h
# spnlcpm/SPNLCPM_scenario3Context.h
# spnlcpm/SPNLCPM_scenario4Context.h
# spm/SPM_scenario1Context.h
# spm/SPM_scenario2Context.h
# spm/SPM_scenario3Context.h
# spm/SPM_scenario4Context.h
# spm/SPM_scenario5Context.h
# spm/SPM_scenario6Context.h
# spm/SPM_scenario7Context.h
# spm/SPM_scenario8Context.h
)

target_compile_features(fsm_test PUBLIC cxx_std_20)

target_link_libraries(fsm_test gtest fsm deterministic_concurrency)