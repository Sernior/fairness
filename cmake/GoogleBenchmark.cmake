# //          Copyright © Federico Abrignani 2023 - 2024.
# //          Copyright © Salvatore Martorana 2023 - 2024.
# // Distributed under the Boost Software License, Version 1.0.
# //    (See accompanying file LICENSE_1_0.txt or copy at
# //          https://www.boost.org/LICENSE_1_0.txt) 

if (MSVC)
    set(BENCHMARK_STATIC_DEFINE ON)
    set(gtest_force_shared_crt on)
endif()

set(BENCHMARK_ENABLE_TESTING OFF)
set(CMAKE_BUILD_TYPE Release)
include(FetchContent)

FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.0
)

FetchContent_MakeAvailable(googlebenchmark)