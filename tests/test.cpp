#include <gtest/gtest.h>
#include "scenario1Context.h"
#include <thread>

TEST(FairSharedMutexTest, Scenario_1) {
    bool conditionAcquire = acquireMutexVector == expectedAcquireMutexVector1 || 
                     acquireMutexVector == expectedAcquireMutexVector2;
    EXPECT_TRUE(conditionAcquire);

    bool conditionRelease = releaseMutexVector == expectedReleaseMutexVector1 || 
                     releaseMutexVector == expectedReleaseMutexVector2;
    EXPECT_TRUE(conditionRelease);
}

int main(int argc, char* argv[]) {    
    // Create an array of threads
    std::vector<std::thread> threads(THREAD_COUNT);

    // Start the threads
    for (int i = 0; i < THREAD_COUNT; i++) {
        threadStates[i].id = i;
        threadStates[i].state = thread_state_t::INACTIVE;

        threads[i] = std::thread([](int i){threadFunction(i);},i);
    }

    // Join the threads (wait for them to finish)
    for (auto& thread : threads) {
        thread.join();
    }

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
