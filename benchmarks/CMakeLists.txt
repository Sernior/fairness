include("../cmake/GoogleBenchmark.cmake")
include("../cmake/DeterministicConcurrency.cmake")

add_executable(pm_benchmark benchmark.cpp pipeline_benchmark.cpp)
target_compile_features(pm_benchmark PUBLIC cxx_std_20)
target_link_libraries(pm_benchmark benchmark::benchmark fsm tp deterministic_concurrency)
if ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
        target_link_libraries(pm_benchmark Shlwapi)
endif()
