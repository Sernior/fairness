#include <gtest/gtest.h>
#include <thread>
#include <algorithm>
#include <DeterministicConcurrency>

#include "PMscenario1Context.h"
#include "PMscenario2Context.h"
#include "PMscenario3Context.h"
#include "PMscenario4Context.h"
#include "PMscenario5Context.h"
#include "SPMscenario1Context.h"
#include "SPMscenario2Context.h"
#include "SPMscenario3Context.h"
#include "SPMscenario4Context.h"
#include "SPMscenario5Context.h"
#include "SPMscenario6Context.h"

TEST(PriorityMutex_ControlledScheduling, LockUnlockTest) {
    PMscenario1::executeSchedulingSequence();
    EXPECT_EQ(PMscenario1::ret, PMscenario1::expected);
    PMscenario1::ret.clear();
    PMscenario1::expected.clear();
}

TEST(PriorityMutex_ControlledScheduling, LockUnlockTest2) {
    PMscenario2::executeSchedulingSequence();
    EXPECT_EQ(PMscenario2::ret, PMscenario2::expected);
    PMscenario2::ret.clear();
    PMscenario2::expected.clear();
}

TEST(PriorityMutex_ControlledScheduling, TryLockTest) {
    PMscenario3::executeSchedulingSequence();
    EXPECT_EQ(PMscenario3::ret, PMscenario3::expected);
    PMscenario3::ret.clear();
    PMscenario3::expected.clear();
}

//TEST(PriorityMutex_ControlledScheduling, MutexOrSemaphore) {
//    PMscenario4::executeSchedulingSequence();//not semaphore
//    EXPECT_EQ(PMscenario4::ret, PMscenario4::expected);
//    PMscenario4::ret.clear();
//    PMscenario4::expected.clear();
//}

#define RANDOM_TESTS 20000000

TEST(PriorityMutex_ControlledScheduling, RandomizedPriorityTest) {
    bool condition = true;
    for (int i = 0; i < RANDOM_TESTS; i++) {
        PMscenario5::executeSchedulingSequence();
        condition &= std::is_sorted(PMscenario5::ret.cbegin(), PMscenario5::ret.cend());

        if (condition && i % 100 == 0){
            std::cout << i << std::endl; 
        }

        PMscenario5::ret.clear();
    }
    EXPECT_TRUE(condition);
}

#undef RANDOM_TESTS

TEST(SharedPriorityMutex_ControlledScheduling, LockUnlockTest) {
    SPMscenario1::executeSchedulingSequence();
    EXPECT_EQ(SPMscenario1::expected, SPMscenario1::ret);
    SPMscenario1::ret.clear();
    SPMscenario1::expected.clear();
}

TEST(SharedPriorityMutex_ControlledScheduling, LockUnlockTest2) {
    SPMscenario2::executeSchedulingSequence();
    EXPECT_EQ(SPMscenario2::expected, SPMscenario2::ret);
    SPMscenario2::ret.clear();
    SPMscenario2::expected.clear();
}

TEST(SharedPriorityMutex_ControlledScheduling, TryLockTest) {
    SPMscenario3::executeSchedulingSequence();
    EXPECT_EQ(SPMscenario3::ret, SPMscenario3::expected);
    SPMscenario3::ret.clear();
    SPMscenario3::expected.clear();
}

TEST(SharedPriorityMutex_ControlledScheduling, LockSharedTest) {
    SPMscenario4::executeSchedulingSequence();
    EXPECT_EQ(SPMscenario4::ret, SPMscenario4::expected);
    SPMscenario4::ret.clear();
    SPMscenario4::expected.clear();
}

//TEST(SharedPriorityMutex_ControlledScheduling, MutexOrSemaphore) {
//    SPMscenario5::executeSchedulingSequence();//not semaphore
//    EXPECT_EQ(SPMscenario5::ret, SPMscenario5::expected);
//    SPMscenario5::ret.clear();
//    SPMscenario5::expected.clear();
//}

TEST(SharedPriorityMutex_ControlledScheduling, PriorityBehavior) {
    SPMscenario6::executeSchedulingSequence();
    EXPECT_EQ(SPMscenario6::ret, SPMscenario6::expected);
    SPMscenario6::ret.clear();
    SPMscenario6::expected.clear();
}

int main(int argc, char* argv[]) {

    /*****************priority_mutex******************/

    //TEST 1
    // PMscenario1::executeSchedulingSequence();
    //////////////////////
    //TEST 2
    // PMscenario2::executeSchedulingSequence();
    //////////////////////
    //TEST 3
    // PMscenario3::executeSchedulingSequence();
    //////////////////////
    //TEST 4
    //PMscenario4::executeSchedulingSequence();//not semaphore
    //////////////////////
    //TEST 5
    // PMscenario5::executeSchedulingSequence();
    //////////////////////
    /**************shared_priority_mutex***************/

    //TEST 6
    // SPMscenario1::executeSchedulingSequence();
    //////////////////////
    //TEST 7
    // SPMscenario2::executeSchedulingSequence();
    //////////////////////
    //TEST 8
    // SPMscenario3::executeSchedulingSequence();
    //////////////////////
    //TEST 9
    // SPMscenario4::executeSchedulingSequence();
    //////////////////////
    //TEST 10
    //SPMscenario5::executeSchedulingSequence();//not semaphore
    //////////////////////
    //TEST 11 (This test does have an assert above but not deadlocking is the real assert, read comments in SPMscenario6Conext.h)
    // SPMscenario6::executeSchedulingSequence();
    //////////////////////

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
