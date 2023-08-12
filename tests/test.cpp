#include <gtest/gtest.h>
#include "scenario1Context.h"
#include "scenario2Context.h"
#include <thread>
#include <algorithm>
#include <DeterministicConcurrency>



TEST(FSMSimple, Scenario1) {
    EXPECT_EQ(scenario1::ret, scenario1::expected);
}

using namespace DeterministicConcurrency;

int main(int argc, char* argv[]) {
    
    //TEST 1
    scenario1::executeSchedulingSequence();
    //////////////////////

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
