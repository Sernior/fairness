include("../cmake/GoogleBenchmark.cmake")
include("../cmake/DeterministicConcurrency.cmake")

add_executable(fairness_benchmark
        benchmark.cpp
        pipeline_benchmark.hpp
        utils/waiting_utils.hpp
        utils/thread_utils.hpp)

if(LIB_FAIRNESS_COMPILE_BENCHMARKS_AS_SINGLE_FILE)
add_executable(priority_mutex_benchmark
        priority_mutex_benchmark.cpp
        pipeline_benchmark.hpp
        utils/waiting_utils.hpp
        utils/thread_utils.hpp)
target_compile_features(priority_mutex_benchmark PUBLIC cxx_std_20)
target_link_libraries(priority_mutex_benchmark benchmark::benchmark fairness deterministic_concurrency)
        
add_executable(spinlock_priority_mutex_benchmark
        spinlock_priority_mutex_benchmark.cpp
        pipeline_benchmark.hpp
        utils/waiting_utils.hpp
        utils/thread_utils.hpp)
target_compile_features(spinlock_priority_mutex_benchmark PUBLIC cxx_std_20)
target_link_libraries(spinlock_priority_mutex_benchmark benchmark::benchmark fairness deterministic_concurrency)
        
add_executable(standard_mutex_benchmark
        standard_mutex_benchmark.cpp
        pipeline_benchmark.hpp
        utils/waiting_utils.hpp
        utils/thread_utils.hpp)
target_compile_features(standard_mutex_benchmark PUBLIC cxx_std_20)
target_link_libraries(standard_mutex_benchmark benchmark::benchmark fairness deterministic_concurrency)
endif()  

target_compile_features(fairness_benchmark PUBLIC cxx_std_20)
target_link_libraries(fairness_benchmark benchmark::benchmark fairness deterministic_concurrency)
if ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
        target_link_libraries(fairness_benchmark Shlwapi)
endif()
if(Boost_FOUND)
target_link_libraries(fairness_benchmark LINK_PUBLIC ${Boost_LIBRARIES} )
endif()

