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
    scenario1::sch.switchContextTo(scenario1::CTRLTHREAD);// give control to the control thread first so it acquires the lock
    scenario1::sch.proceed(// allow all the other thread to proceed
        scenario1::THREAD1,
        scenario1::THREAD2,
        scenario1::THREAD3,
        scenario1::THREAD4,
        scenario1::THREAD5
    );
    scenario1::sch.waitUntilAllThreadStatus<thread_status_t::WAITING_EXTERNAL>(// wait until all of them are waiting on the lock
        scenario1::THREAD1,
        scenario1::THREAD2,
        scenario1::THREAD3,
        scenario1::THREAD4,
        scenario1::THREAD5
    );
    scenario1::sch.switchContextTo(scenario1::CTRLTHREAD);// at this point the the control thread can end and all the others should respect their priority
    scenario1::sch.joinAll();
    //////////////////////

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
