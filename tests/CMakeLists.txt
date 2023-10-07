include("../cmake/GoogleTest.cmake")
include("../cmake/DeterministicConcurrency.cmake")

add_executable(fsm_test
test.cpp
scenarios.hpp
)

target_compile_features(fsm_test PUBLIC cxx_std_20)

target_link_libraries(fsm_test gtest fsm deterministic_concurrency)

if(Boost_FOUND)
target_link_libraries(fsm_test LINK_PUBLIC ${Boost_LIBRARIES} )
endif()