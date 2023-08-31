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
