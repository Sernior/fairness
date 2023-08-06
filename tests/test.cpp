#include <gtest/gtest.h>
#include "scenario1Context.h"
#include "scenario2Context.h"
#include <thread>
#include <algorithm>
#include <DeterministicConcurrency>



//TEST(DeterministicThread, Scenario1) {
//    EXPECT_EQ(scenario1DT::out, scenario1DT::exp);
//}


int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
