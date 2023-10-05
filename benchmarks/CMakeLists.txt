include("../cmake/GoogleBenchmark.cmake")
include("../cmake/DeterministicConcurrency.cmake")

add_executable(pm_benchmark
        benchmark.cpp
        pipeline_benchmark.hpp
        utils/waiting_utils.hpp
        utils/thread_utils.hpp)
target_compile_features(pm_benchmark PUBLIC cxx_std_20)
target_link_libraries(pm_benchmark benchmark::benchmark fsm tp deterministic_concurrency)
if ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
        target_link_libraries(pm_benchmark Shlwapi)
endif()
if(Boost_FOUND)
target_link_libraries(pm_benchmark LINK_PUBLIC ${Boost_LIBRARIES} )
endif()

