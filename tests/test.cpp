#include <gtest/gtest.h>
#include <thread>
#include <algorithm>
#include <DeterministicConcurrency>

#include "PM_scenario1Context.h"
#include "PM_scenario2Context.h"
#include "PM_scenario3Context.h"
#include "PM_scenario4Context.h"
#include "PM_scenario5Context.h"
#include "SLM_scenario1Context.h"
#include "SLM_scenario2Context.h"
#include "SLM_scenario3Context.h"
#include "SLM_scenario4Context.h"
#include "SPM_scenario1Context.h"
#include "SPM_scenario2Context.h"
#include "SPM_scenario3Context.h"
#include "SPM_scenario4Context.h"
#include "SPM_scenario5Context.h"
#include "SPM_scenario6Context.h"

TEST(PriorityMutex_ControlledScheduling, LockUnlockTest) {
    PM_scenario1::executeSchedulingSequence();
    EXPECT_EQ(PM_scenario1::ret, PM_scenario1::expected);
    PM_scenario1::ret.clear();
    PM_scenario1::expected.clear();
}

TEST(PriorityMutex_ControlledScheduling, LockUnlockTest2) {
    PM_scenario2::executeSchedulingSequence();
    EXPECT_EQ(PM_scenario2::ret, PM_scenario2::expected);
    PM_scenario2::ret.clear();
    PM_scenario2::expected.clear();
}

TEST(PriorityMutex_ControlledScheduling, TryLockTest) {
    PM_scenario3::executeSchedulingSequence();
    EXPECT_EQ(PM_scenario3::ret, PM_scenario3::expected);
    PM_scenario3::ret.clear();
    PM_scenario3::expected.clear();
}

//TEST(PriorityMutex_ControlledScheduling, MutexOrSemaphore) {
//    PM_scenario4::executeSchedulingSequence();//not semaphore
//    EXPECT_EQ(PM_scenario4::ret, PM_scenario4::expected);
//    PM_scenario4::ret.clear();
//    PM_scenario4::expected.clear();
//}

#define RANDOM_TESTS 250

TEST(PriorityMutex_ControlledScheduling, RandomizedPriorityTest) {
    bool condition = true;
    for (int i = 0; i < RANDOM_TESTS; i++) {
        PM_scenario5::executeSchedulingSequence();
        condition &= std::is_sorted(PM_scenario5::ret.cbegin(), PM_scenario5::ret.cend());

        if (condition && i % 100 == 0){
            std::cout << i << std::endl; 
        }

        PM_scenario5::ret.clear();
    }
    EXPECT_TRUE(condition);
}

TEST(PriorityMutex_ControlledScheduling, SLM_RandomizedPriorityTest) {
    bool condition = true;
    for (int i = 0; i < RANDOM_TESTS; i++) {
        SPM_scenario1::executeSchedulingSequence();
        condition &= std::is_sorted(SPM_scenario1::ret.cbegin(), SPM_scenario1::ret.cend());

        if (condition && i % 100 == 0){
            std::cout << i << std::endl; 
        }

        SPM_scenario1::ret.clear();
    }
    EXPECT_TRUE(condition);
}

#undef RANDOM_TESTS

TEST(PriorityMutex_ControlledScheduling, SLM_LockUnlockTest) {
    SPM_scenario2::executeSchedulingSequence();
    EXPECT_EQ(SPM_scenario2::ret, SPM_scenario2::expected);
    SPM_scenario2::ret.clear();
    SPM_scenario2::expected.clear();
}

TEST(PriorityMutex_ControlledScheduling, SLM_LockUnlockTest2) {
    SPM_scenario3::executeSchedulingSequence();
    EXPECT_EQ(SPM_scenario3::ret, SPM_scenario3::expected);
    SPM_scenario3::ret.clear();
    SPM_scenario3::expected.clear();
}

TEST(PriorityMutex_ControlledScheduling, SLM_TryLockTest) {
    SPM_scenario4::executeSchedulingSequence();
    EXPECT_EQ(SPM_scenario4::ret, SPM_scenario4::expected);
    SPM_scenario4::ret.clear();
    SPM_scenario4::expected.clear();
}

TEST(SharedPriorityMutex_ControlledScheduling, LockUnlockTest) {
    SPM_scenario1::executeSchedulingSequence();
    EXPECT_EQ(SPM_scenario1::expected, SPM_scenario1::ret);
    SPM_scenario1::ret.clear();
    SPM_scenario1::expected.clear();
}

TEST(SharedPriorityMutex_ControlledScheduling, LockUnlockTest2) {
    SPM_scenario2::executeSchedulingSequence();
    EXPECT_EQ(SPM_scenario2::expected, SPM_scenario2::ret);
    SPM_scenario2::ret.clear();
    SPM_scenario2::expected.clear();
}

TEST(SharedPriorityMutex_ControlledScheduling, TryLockTest) {
    SPM_scenario3::executeSchedulingSequence();
    EXPECT_EQ(SPM_scenario3::ret, SPM_scenario3::expected);
    SPM_scenario3::ret.clear();
    SPM_scenario3::expected.clear();
}

TEST(SharedPriorityMutex_ControlledScheduling, LockSharedTest) {
    SPM_scenario4::executeSchedulingSequence();
    EXPECT_EQ(SPM_scenario4::ret, SPM_scenario4::expected);
    SPM_scenario4::ret.clear();
    SPM_scenario4::expected.clear();
}

//TEST(SharedPriorityMutex_ControlledScheduling, MutexOrSemaphore) {
//    SPM_scenario5::executeSchedulingSequence();//not semaphore
//    EXPECT_EQ(SPM_scenario5::ret, SPM_scenario5::expected);
//    SPM_scenario5::ret.clear();
//    SPM_scenario5::expected.clear();
//}

TEST(SharedPriorityMutex_ControlledScheduling, PriorityBehavior) {
    SPM_scenario6::executeSchedulingSequence();
    EXPECT_EQ(SPM_scenario6::ret, SPM_scenario6::expected);
    SPM_scenario6::ret.clear();
    SPM_scenario6::expected.clear();
}

int main(int argc, char* argv[]) {

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
