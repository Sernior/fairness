include("../cmake/GoogleBenchmark.cmake")
include("../cmake/DeterministicConcurrency.cmake")

add_executable(fairness_benchmark
        benchmark.cpp
        pipeline_benchmark.hpp
        utils/waiting_utils.hpp
        utils/thread_utils.hpp)
target_compile_features(fairness_benchmark PUBLIC cxx_std_20)
target_link_libraries(fairness_benchmark benchmark::benchmark fairness deterministic_concurrency)
if ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
        target_link_libraries(fairness_benchmark Shlwapi)
endif()
if(Boost_FOUND)
target_link_libraries(fairness_benchmark LINK_PUBLIC ${Boost_LIBRARIES} )
endif()

