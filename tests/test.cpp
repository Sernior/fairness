#include <gtest/gtest.h>
#include "PMscenario1Context.h"
#include "PMscenario2Context.h"
#include "PMscenario3Context.h"
#include <thread>
#include <algorithm>
#include <DeterministicConcurrency>



TEST(PriorityMutex_ControlledScheduling, LockUnlockTest) {
    EXPECT_EQ(PMscenario1::ret, PMscenario1::expected);
}

TEST(PriorityMutex_ControlledScheduling, LockUnlockTest2) {
    EXPECT_EQ(PMscenario2::ret, PMscenario2::expected);
}

TEST(PriorityMutex_ControlledScheduling, TryLockTest) {
    EXPECT_EQ(PMscenario3::ret, PMscenario3::expected);
}

int main(int argc, char* argv[]) {
    
    //TEST 1
    PMscenario1::executeSchedulingSequence();
    //////////////////////
    //TEST 2
    PMscenario2::executeSchedulingSequence();
    //////////////////////
    //TEST 3
    PMscenario3::executeSchedulingSequence();
    //////////////////////


    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
