#include <gtest/gtest.h>
#include "scenario1Context.h"
#include "scenario2Context.h"
#include <thread>
#include <algorithm>
#include "det_Thread.h"
#include "scenario1DThread.h"

//TEST(FairSharedMutexTest, Scenario_1) {
//    EXPECT_EQ(scenario1::acquireMutexVector, scenario1::expectedAcquireMutexVector1);
//    EXPECT_EQ(scenario1::acquireMutexVector, scenario1::expectedAcquireMutexVector2);
//    bool conditionAcquire = scenario1::acquireMutexVector == scenario1::expectedAcquireMutexVector1 || 
//                            scenario1::acquireMutexVector == scenario1::expectedAcquireMutexVector2;
//    EXPECT_TRUE(conditionAcquire);

//    EXPECT_EQ(scenario1::releaseMutexVector, scenario1::expectedReleaseMutexVector1);
//    EXPECT_EQ(scenario1::releaseMutexVector, scenario1::expectedReleaseMutexVector2);
//    bool conditionRelease = scenario1::releaseMutexVector == scenario1::expectedReleaseMutexVector1 || 
//                            scenario1::releaseMutexVector == scenario1::expectedReleaseMutexVector2;
//    EXPECT_TRUE(conditionRelease);
//}
TEST(DeterministicThread, Scenario1) {
    EXPECT_EQ(scenario1DT::out, scenario1DT::exp);
}
//TEST(FairSharedMutexTest, Scenario_2) {
//    bool is_sorted = std::is_sorted(scenario2::acquireMutexVector.begin()+1,scenario2::acquireMutexVector.end());
//    EXPECT_TRUE(is_sorted);
    //EXPECT_EQ(scenario2::acquireMutexVector, scenario2::expectedAcquireMutexVector1);
    //EXPECT_EQ(scenario2::acquireMutexVector, scenario2::expectedAcquireMutexVector2);
    //bool conditionAcquire = scenario2::acquireMutexVector == scenario2::expectedAcquireMutexVector1 || 
    //                        scenario2::acquireMutexVector == scenario2::expectedAcquireMutexVector2;
    //EXPECT_TRUE(conditionAcquire);

    //EXPECT_EQ(scenario2::releaseMutexVector, scenario2::expectedReleaseMutexVector1);
    //EXPECT_EQ(scenario2::releaseMutexVector, scenario2::expectedReleaseMutexVector2);
    //bool conditionRelease = scenario2::releaseMutexVector == scenario2::expectedReleaseMutexVector1 || 
    //                        scenario2::releaseMutexVector == scenario2::expectedReleaseMutexVector2;
    //EXPECT_TRUE(conditionRelease);
//}

int main(int argc, char* argv[]) {
    // Create an array of threads
    //std::vector<std::thread> threads(scenario1::THREAD_COUNT);
    auto* threadUtil = new det_thread_utils::this_thread();
    DeterministicThread my_thread([](det_thread_utils::this_thread* threadUtil) {
    // Perform some work in the thread
    scenario1DT::threadFunction(threadUtil);
    } , threadUtil);

    auto* threadUtil2 = new det_thread_utils::this_thread();
    DeterministicThread my_thread2([](det_thread_utils::this_thread* threadUtil) {
    // Perform some work in the thread
    scenario1DT::threadFunction(threadUtil);
    } , threadUtil2);

    my_thread.tick();
    my_thread.wait_for_tock();
    my_thread2.tick();
    my_thread2.wait_for_tock();
    my_thread.tick();
    my_thread.wait_for_tock();
    my_thread2.tick();
    my_thread2.wait_for_tock();
    my_thread.tick();
    my_thread.wait_for_tock();
    my_thread2.tick();
    my_thread2.wait_for_tock();
    my_thread.tick();
    my_thread.wait_for_tock();
    my_thread2.tick();
    my_thread2.wait_for_tock();
    my_thread.tick();
    my_thread.wait_for_tock();
    my_thread2.tick();
    my_thread2.wait_for_tock();
    my_thread.tick();
    my_thread.wait_for_tock();
    my_thread2.tick();
    my_thread2.wait_for_tock();

    // Start the threads
    //for (int i = 0; i < scenario1::THREAD_COUNT; i++) {
    //    scenario1::threadStates[i].id = i;
    //    scenario1::threadStates[i].state = scenario1::thread_state_t::INACTIVE;

    //    threads[i] = std::thread([](int i){scenario1::threadFunction(i);},i);
    //}

    // Join the threads (wait for them to finish)
    //for (auto& thread : threads) {
    //    thread.join();
    //}

    // Create an array of threads
    //std::vector<std::thread> threads2(scenario2::THREAD_COUNT);

    // Start the threads
    //for (int i = 0; i < scenario2::THREAD_COUNT; i++) {
    //    scenario2::threadStates[i].state = scenario2::thread_state_t::INACTIVE;

    //    threads2[i] = std::thread([](int i){scenario2::threadFunction(i);},i);
    //}

    // Join the threads (wait for them to finish)
    //for (auto& thread : threads2) {
    //    thread.join();
    //}

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
