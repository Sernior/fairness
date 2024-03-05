cmake_minimum_required(VERSION 3.14.0)

set(PROJ_NAME "Fairness")
project(${PROJ_NAME} VERSION 1.0.0 LANGUAGES CXX)

option(LIB_FAIRNESS_EXPERIMENTAL "Add experimental mutexes" OFF)
option(Boost_FOUND "Boost found" OFF)

add_library(fairness INTERFACE)
target_include_directories(fairness INTERFACE include)

# add_compile_options(-mcx16)

find_package(Boost 1.74.0)

if(LIB_FAIRNESS_EXPERIMENTAL)#experimental stuff
    target_include_directories(fairness INTERFACE include/experimental)
    add_compile_definitions(BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES)
endif()

if(Boost_FOUND)#if boost is found for boost atomic
    add_compile_definitions(BOOST_FOUND)
endif()

target_compile_features(fairness INTERFACE cxx_std_20)

set(CMAKE_CXX_STANDARD 20)
if (MSVC)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif ()

option(LIB_FAIRNESS_COMPILE_TESTS "Build the lib-fairnesstests" OFF)

option(LIB_FAIRNESS_COMPILE_BENCHMARKS "Build the lib-fairness tests" OFF)

option(LIB_FAIRNESS_COMPILE_BENCHMARKS_AS_SINGLE_FILE "" OFF)

option(LIB_FAIRNESS_EXPERIMENTAL "add experimental mutexes" OFF)
 
if(LIB_FAIRNESS_COMPILE_TESTS)
    add_subdirectory(tests)
endif()

if(LIB_FAIRNESS_COMPILE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
