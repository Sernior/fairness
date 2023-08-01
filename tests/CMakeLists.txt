include("../cmake/GoogleTest.cmake")

add_executable(fsm_test test.cpp scenario1Context.h scenario2Context.h scenario1DThread.h det_Thread.h)

target_compile_features(fsm_test PUBLIC cxx_std_20)

target_link_libraries(fsm_test gtest_main fsm)
