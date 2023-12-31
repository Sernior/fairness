cmake_minimum_required(VERSION 3.14.0)

set(PROJ_NAME "Fair_Shared_Mutex")
project(${PROJ_NAME} VERSION 0.0.0 LANGUAGES CXX)

option(LIBFSM_EXPERIMENTAL "Add experimental mutexes" OFF)
option(Boost_FOUND "Boost found" OFF)

add_library(fsm INTERFACE)
target_include_directories(fsm INTERFACE include)

add_compile_options(-mcx16)

find_package(Boost 1.74.0)

if(LIBFSM_EXPERIMENTAL)#experimental stuff
target_include_directories(fsm INTERFACE include/experimental)
add_compile_definitions(BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES)
endif()

if(Boost_FOUND)#if boost is found for boost atomic
add_compile_definitions(BOOST_FOUND)
endif()

target_compile_features(fsm INTERFACE cxx_std_20)

set(CMAKE_CXX_STANDARD 20)
if (MSVC)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif ()
option(LIBFSM_COMPILE_TESTS "Build the libfsm tests" OFF)

option(LIBFSM_COMPILE_BENCHMARKS "Build the libfsm tests" OFF)

option(LIBFSM_EXPERIMENTAL "add experimental mutexes" OFF)

add_library(tp INTERFACE)# testing to remove in the end
target_include_directories(tp INTERFACE thread-pool)
target_compile_features(tp INTERFACE cxx_std_20)

if(LIBFSM_COMPILE_TESTS)
    add_subdirectory(tests)
endif()

if(LIBFSM_COMPILE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

set(SOURCE_LIST
    main.cpp)

add_executable(test ${SOURCE_LIST})
target_link_libraries(test PRIVATE fsm tp)