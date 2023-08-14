#include <gtest/gtest.h>
#include <thread>
#include <algorithm>
#include <DeterministicConcurrency>

#include "PMscenario1Context.h"
#include "PMscenario2Context.h"
#include "PMscenario3Context.h"
#include "SPMscenario1Context.h"
#include "SPMscenario2Context.h"
#include "SPMscenario3Context.h"
#include "SPMscenario4Context.h"

TEST(PriorityMutex_ControlledScheduling, LockUnlockTest) {
    EXPECT_EQ(PMscenario1::ret, PMscenario1::expected);
}

TEST(PriorityMutex_ControlledScheduling, LockUnlockTest2) {
    EXPECT_EQ(PMscenario2::ret, PMscenario2::expected);
}

TEST(PriorityMutex_ControlledScheduling, TryLockTest) {
    EXPECT_EQ(PMscenario3::ret, PMscenario3::expected);
}

TEST(SharedPriorityMutex_ControlledScheduling, LockUnlockTest) {
    EXPECT_EQ(SPMscenario1::expected, SPMscenario1::ret);
}

TEST(SharedPriorityMutex_ControlledScheduling, LockUnlockTest2) {
    EXPECT_EQ(SPMscenario2::expected, SPMscenario2::ret);
}

TEST(SharedPriorityMutex_ControlledScheduling, TryLockTest) {
    EXPECT_EQ(SPMscenario3::ret, SPMscenario3::expected);
}

TEST(SharedPriorityMutex_ControlledScheduling, LockSharedTest) {
    EXPECT_EQ(SPMscenario4::ret, SPMscenario4::expected);
}

int main(int argc, char* argv[]) {

    /*****************priority_mutex******************/

    //TEST 1
    PMscenario1::executeSchedulingSequence();
    //////////////////////
    //TEST 2
    PMscenario2::executeSchedulingSequence();
    //////////////////////
    //TEST 3
    PMscenario3::executeSchedulingSequence();
    //////////////////////

    /**************priority_shared_mutex***************/

    //TEST 4
    SPMscenario1::executeSchedulingSequence();
    //////////////////////
    //TEST 5
    SPMscenario2::executeSchedulingSequence();
    //////////////////////
    //TEST 6
    SPMscenario3::executeSchedulingSequence();
    //////////////////////
    //TEST 7
    SPMscenario4::executeSchedulingSequence();
    //////////////////////


    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
