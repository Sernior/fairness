# //          Copyright © Federico Abrignani 2023 - 2024.
# //          Copyright © Salvatore Martorana 2023 - 2024.
# // Distributed under the Boost Software License, Version 1.0.
# //    (See accompanying file LICENSE_1_0.txt or copy at
# //          https://www.boost.org/LICENSE_1_0.txt) 

set(CMAKE_BUILD_TYPE Release)

if (MSVC)
  set(gtest_force_shared_crt on)
endif()

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)

FetchContent_MakeAvailable(googletest)
