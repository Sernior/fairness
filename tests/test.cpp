#include <gtest/gtest.h>
#include "scenario1Context.h"
#include "scenario2Context.h"
#include <thread>
#include <algorithm>
#include <DeterministicConcurrency>



TEST(FSMSimple, Scenario1) {
    EXPECT_EQ(scenario1::ret, scenario1::expected);
}

//TEST(FSMSimple, Scenario2) {
//    EXPECT_EQ(scenario2::ret, scenario2::expected);
//}

using namespace DeterministicConcurrency;

int main(int argc, char* argv[]) {

    //TEST 1
    auto sch = make_UserControlledScheduler(
        scenario1::thread_0, scenario1::thread_1, scenario1::thread_2, scenario1::thread_3, scenario1::thread_4, scenario1::ctrlThread
    );
    sch.switchContextTo(5);// give control to the control thread first so it acquires the lock
    sch.proceed(// allow all the other thread to proceed
        0,
        1,
        2,
        3,
        4
    );
    sch.waitUntilAllThreadStatus<thread_status_t::WAITING_EXTERNAL>(// wait until all of them are waiting on the lock
        0,
        1,
        2,
        3,
        4
    );
    sch.switchContextTo(5);// at this point the the control thread can end and all the others should respect their priority
    sch.joinAll();
    //////////////////////

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
