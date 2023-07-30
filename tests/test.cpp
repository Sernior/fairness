#include <gtest/gtest.h>
#include "scenario1Context.h"
#include "scenario2Context.h"
#include <thread>

TEST(FairSharedMutexTest, Scenario_1) {
    bool conditionAcquire = scenario1::acquireMutexVector == scenario1::expectedAcquireMutexVector1 || 
                            scenario1::acquireMutexVector == scenario1::expectedAcquireMutexVector2;
    EXPECT_TRUE(conditionAcquire);

    bool conditionRelease = scenario1::releaseMutexVector == scenario1::expectedReleaseMutexVector1 || 
                            scenario1::releaseMutexVector == scenario1::expectedReleaseMutexVector2;
    EXPECT_TRUE(conditionRelease);
}

TEST(FairSharedMutexTest, Scenario_2) {
    EXPECT_EQ(scenario2::acquireMutexVector, scenario2::expectedAcquireMutexVector1);
    bool conditionAcquire = scenario2::acquireMutexVector == scenario2::expectedAcquireMutexVector1 || 
                            scenario2::acquireMutexVector == scenario2::expectedAcquireMutexVector2;
    EXPECT_TRUE(conditionAcquire);

    bool conditionRelease = scenario2::releaseMutexVector == scenario2::expectedReleaseMutexVector1 || 
                            scenario2::releaseMutexVector == scenario2::expectedReleaseMutexVector2;
    EXPECT_TRUE(conditionRelease);
}

int main(int argc, char* argv[]) {    
    // Create an array of threads
    std::vector<std::thread> threads(scenario1::THREAD_COUNT);

    // Start the threads
    for (int i = 0; i < scenario1::THREAD_COUNT; i++) {
        scenario1::threadStates[i].id = i;
        scenario1::threadStates[i].state = scenario1::thread_state_t::INACTIVE;

        threads[i] = std::thread([](int i){scenario1::threadFunction(i);},i);
    }

    // Join the threads (wait for them to finish)
    for (auto& thread : threads) {
        thread.join();
    }

    // Create an array of threads
    std::vector<std::thread> threads2(scenario2::THREAD_COUNT);

    // Start the threads
    for (int i = 0; i < scenario2::THREAD_COUNT; i++) {
        scenario2::threadStates[i].id = i;
        scenario2::threadStates[i].state = scenario2::thread_state_t::INACTIVE;

        threads2[i] = std::thread([](int i){scenario2::threadFunction(i);},i);
    }

    // Join the threads (wait for them to finish)
    for (auto& thread : threads2) {
        thread.join();
    }

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
