#include <gtest/gtest.h>
#include <thread>
#include <algorithm>
#include <DeterministicConcurrency>

#include "PMscenario1Context.h"
#include "PMscenario2Context.h"
#include "PMscenario3Context.h"
#include "PMscenario4Context.h"
#include "SPMscenario1Context.h"
#include "SPMscenario2Context.h"
#include "SPMscenario3Context.h"
#include "SPMscenario4Context.h"
#include "SPMscenario5Context.h"
#include "SPMscenario6Context.h"

TEST(PriorityMutex_ControlledScheduling, LockUnlockTest) {
    EXPECT_EQ(PMscenario1::ret, PMscenario1::expected);
}

TEST(PriorityMutex_ControlledScheduling, LockUnlockTest2) {
    EXPECT_EQ(PMscenario2::ret, PMscenario2::expected);
}

TEST(PriorityMutex_ControlledScheduling, TryLockTest) {
    EXPECT_EQ(PMscenario3::ret, PMscenario3::expected);
}

//TEST(PriorityMutex_ControlledScheduling, MutexOrSemaphore) {
//    EXPECT_EQ(PMscenario4::ret, PMscenario4::expected);
//}

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

//TEST(SharedPriorityMutex_ControlledScheduling, MutexOrSemaphore) {
//    EXPECT_EQ(SPMscenario5::ret, SPMscenario5::expected);
//}

TEST(SharedPriorityMutex_ControlledScheduling, PriorityBehavior) {
    EXPECT_EQ(SPMscenario6::ret, SPMscenario6::expected);
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
    //TEST 4
    PMscenario4::executeSchedulingSequence();
    //////////////////////

    /**************shared_priority_mutex***************/

    //TEST 5
    SPMscenario1::executeSchedulingSequence();
    //////////////////////
    //TEST 6
    SPMscenario2::executeSchedulingSequence();
    //////////////////////
    //TEST 7
    SPMscenario3::executeSchedulingSequence();
    //////////////////////
    //TEST 8
    SPMscenario4::executeSchedulingSequence();
    //////////////////////
    //TEST 9
    SPMscenario5::executeSchedulingSequence();
    //////////////////////
    //TEST 10 (This test does have an assert above but not deadlocking is the real assert, read comments in SPMscenario6Conext.h)
    SPMscenario6::executeSchedulingSequence();
    //////////////////////


    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
