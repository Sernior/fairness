/**
 * @file test.cpp
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @author F. Abrignani (federignoli@hotmail.it)
 * @brief Alias test.
 * @version 0.1
 * @date 2023-10-06
 * @private
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */

#include <boost/fairness.hpp>
#include <gtest/gtest.h>
#include <thread>
#include <algorithm>
#include <DeterministicConcurrency>

#include "scenarios.hpp"

TEST(PriorityMutex_ControlledScheduling, lock_unlock_benchmarkTest) {
    PM_scenario1::executeSchedulingSequence();
    EXPECT_EQ(PM_scenario1::ret, PM_scenario1::expected);
    PM_scenario1::ret.clear();
    PM_scenario1::expected.clear();
}

TEST(PriorityMutex_ControlledScheduling, lock_unlock_benchmarkTest2) {
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

TEST(SlimPriorityMutex_ControlledScheduling, SLM_RandomizedPriorityTest) {
    bool condition = true;
    for (int i = 0; i < RANDOM_TESTS; ++i) {
        SLM_scenario4::executeSchedulingSequence();
        condition &= std::is_sorted(SLM_scenario4::ret.cbegin(), SLM_scenario4::ret.cend());

        if (condition && i % 100 == 0){
            std::cout << i << std::endl; 
        }

        SLM_scenario4::ret.clear();
    }
    EXPECT_TRUE(condition);
}

TEST(SpinlockPriorityMutex_ControlledScheduling, SPNLCPM_RandomizedPriorityTest) {
    bool condition = true;
    for (int i = 0; i < RANDOM_TESTS; ++i) {
        SPNLCPM_scenario4::executeSchedulingSequence();
        condition &= std::is_sorted(SPNLCPM_scenario4::ret.cbegin(), SPNLCPM_scenario4::ret.cend());

        if (condition && i % 100 == 0){
            std::cout << i << std::endl; 
        }

        SPNLCPM_scenario4::ret.clear();
    }
    EXPECT_TRUE(condition);
}

TEST(SharedPriorityMutex_ControlledScheduling, SPM_RandomizedPriorityTest) {
    bool condition = true;
    for (int i = 0; i < RANDOM_TESTS; ++i) {
        SPM_scenario8::executeSchedulingSequence();
        condition &= std::is_sorted(SPM_scenario8::ret.cbegin(), SPM_scenario8::ret.cend());

        if (i % 100 == 0){
            std::cout << i << std::endl; 
        }

        if (!std::is_sorted(SPM_scenario8::ret.cbegin(), SPM_scenario8::ret.cend())){
            for (int j = 0; j < 8; ++j) {
                std::cout << SPM_scenario8::ret[j] << " ";
            }
            std::cout << std::endl;
        }

        SPM_scenario8::ret.clear();
    }
    EXPECT_TRUE(condition);
}

TEST(RecursivePriorityMutex_ControlledScheduling, RPM_RandomizedPriorityTest) {
    bool condition = true;
    for (int i = 0; i < RANDOM_TESTS; i++) {
        RPM_scenario5::executeSchedulingSequence();
        condition &= std::is_sorted(RPM_scenario5::ret.cbegin(), RPM_scenario5::ret.cend());

        if (condition && i % 100 == 0){
            std::cout << i << std::endl; 
        }

        RPM_scenario5::ret.clear();
    }
    EXPECT_TRUE(condition);
}

#undef RANDOM_TESTS

TEST(SlimPriorityMutex_ControlledScheduling, SLM_lock_unlock_benchmarkTest) {
    SLM_scenario1::executeSchedulingSequence();
    EXPECT_EQ(SLM_scenario1::ret, SLM_scenario1::expected);
    SLM_scenario1::ret.clear();
    SLM_scenario1::expected.clear();
}

TEST(SlimPriorityMutex_ControlledScheduling, SLM_lock_unlock_benchmarkTest2) {
    SLM_scenario2::executeSchedulingSequence();
    EXPECT_EQ(SLM_scenario2::ret, SLM_scenario2::expected);
    SLM_scenario2::ret.clear();
    SLM_scenario2::expected.clear();
}

TEST(SlimPriorityMutex_ControlledScheduling, SLM_TryLockTest) {
    SLM_scenario3::executeSchedulingSequence();
    EXPECT_EQ(SLM_scenario3::ret, SLM_scenario3::expected);
    SLM_scenario3::ret.clear();
    SLM_scenario3::expected.clear();
}

TEST(SpinlockPriorityMutex_ControlledScheduling, SPNLCPM_lock_unlock_benchmarkTest) {
    SPNLCPM_scenario1::executeSchedulingSequence();
    EXPECT_EQ(SPNLCPM_scenario1::ret, SPNLCPM_scenario1::expected);
    SPNLCPM_scenario1::ret.clear();
    SPNLCPM_scenario1::expected.clear();
}

TEST(SpinlockPriorityMutex_ControlledScheduling, SPNLCPM_lock_unlock_benchmarkTest2) {
    SPNLCPM_scenario2::executeSchedulingSequence();
    EXPECT_EQ(SPNLCPM_scenario2::ret, SPNLCPM_scenario2::expected);
    SPNLCPM_scenario2::ret.clear();
    SPNLCPM_scenario2::expected.clear();
}

TEST(SpinlockPriorityMutex_ControlledScheduling, SPNLCPM_TryLockTest) {
    SPNLCPM_scenario3::executeSchedulingSequence();
    EXPECT_EQ(SPNLCPM_scenario3::ret, SPNLCPM_scenario3::expected);
    SPNLCPM_scenario3::ret.clear();
    SPNLCPM_scenario3::expected.clear();
}

TEST(SharedPriorityMutex_ControlledScheduling, lock_unlock_benchmarkTest) {
    SPM_scenario1::executeSchedulingSequence();
    EXPECT_EQ(SPM_scenario1::expected, SPM_scenario1::ret);
    SPM_scenario1::ret.clear();
    SPM_scenario1::expected.clear();
}

TEST(SharedPriorityMutex_ControlledScheduling, lock_unlock_benchmarkTest2) {
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

TEST(SharedPriorityMutex_ControlledScheduling, TryLockSharedTest) {
    SPM_scenario7::executeSchedulingSequence();
    EXPECT_EQ(SPM_scenario7::ret, SPM_scenario7::expected);
    SPM_scenario7::ret.clear();
    SPM_scenario7::expected.clear();
}

TEST(RecursivePriorityMutex_ControlledScheduling, RPM_lock_unlock_benchmarkTest) {
    RPM_scenario1::executeSchedulingSequence();
    EXPECT_EQ(RPM_scenario1::ret, RPM_scenario1::expected);
    RPM_scenario1::ret.clear();
    RPM_scenario1::expected.clear();
}

TEST(RecursivePriorityMutex_ControlledScheduling, RPM_lock_unlock_benchmarkTest2) {
    RPM_scenario2::executeSchedulingSequence();
    EXPECT_EQ(RPM_scenario2::ret, RPM_scenario2::expected);
    RPM_scenario2::ret.clear();
    RPM_scenario2::expected.clear();
}

TEST(RecursivePriorityMutex_ControlledScheduling, RPM_TryLockTest) {
    RPM_scenario3::executeSchedulingSequence();
    EXPECT_EQ(RPM_scenario3::ret, RPM_scenario3::expected);
    RPM_scenario3::ret.clear();
    RPM_scenario3::expected.clear();
}

TEST(RecursivePriorityMutex_ControlledScheduling, RPM_RecursiveTest) {
   RPM_scenario4::executeSchedulingSequence();//not semaphore
   EXPECT_EQ(RPM_scenario4::ret, RPM_scenario4::expected);
   RPM_scenario4::ret.clear();
   RPM_scenario4::expected.clear();
}

TEST(RecursivePriorityMutex_ControlledScheduling, RPM_RecursiveTryLockTest) {
   RPM_scenario6::executeSchedulingSequence();
   EXPECT_EQ(RPM_scenario6::ret, RPM_scenario6::expected);
   RPM_scenario6::ret.clear();
   RPM_scenario6::expected.clear();
}

TEST(RecursivePriorityMutex_ControlledScheduling, RPM_RecursiveTryLockDifferentPriorityTest) {
   RPM_scenario7::executeSchedulingSequence();
   EXPECT_EQ(RPM_scenario7::ret, RPM_scenario7::expected);
   RPM_scenario7::ret.clear();
   RPM_scenario7::expected.clear();
}

TEST(RecursivePriorityMutex_ControlledScheduling, RPM_RecursiveDifferentPriorityTest) {
   RPM_scenario8::executeSchedulingSequence();//not semaphore
   EXPECT_EQ(RPM_scenario8::ret, RPM_scenario8::expected);
   RPM_scenario8::ret.clear();
   RPM_scenario8::expected.clear();
}

TEST(RecursivePriorityMutex_ControlledScheduling, RPM_RecursiveCustomTest) {
   RPM_scenario9::executeSchedulingSequence();//not semaphore
   EXPECT_EQ(RPM_scenario9::ret, RPM_scenario9::expected);
   RPM_scenario9::ret.clear();
   RPM_scenario9::expected.clear();
}
int main(int argc, char* argv[]) {

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
