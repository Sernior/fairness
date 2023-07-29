include("../cmake/GoogleTest.cmake")

add_executable(fsm_test test.cpp)

target_compile_features(fsm_test PUBLIC cxx_std_17)

target_link_libraries(fsm_test gtest_main fsm)
