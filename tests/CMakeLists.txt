include("../cmake/GoogleTest.cmake")
include("../cmake/DeterministicConcurrency.cmake")

add_executable(fsm_test
test.cpp
PMscenario1Context.h
PMscenario2Context.h
PMscenario3Context.h
SPMscenario1Context.h
SPMscenario2Context.h
SPMscenario3Context.h
SPMscenario4Context.h
)

target_compile_features(fsm_test PUBLIC cxx_std_20)

target_link_libraries(fsm_test gtest fsm deterministic_concurrency)