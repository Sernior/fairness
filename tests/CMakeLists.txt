include("../cmake/GoogleTest.cmake")
include("../cmake/DeterministicConcurrency.cmake")

add_executable(fairness_test
    test.cpp
    scenarios.hpp
)

target_compile_features(fairness_test PUBLIC cxx_std_20)

target_link_libraries(fairness_test gtest fairness deterministic_concurrency)

if(Boost_FOUND)
target_link_libraries(fairness_test LINK_PUBLIC ${Boost_LIBRARIES} )
endif()