#include <gtest/gtest.h>
#include <fair_shared_mutex.h>

TEST(FairSharedMutexTest, LockTest) {
    EXPECT_EQ(Add(2, 3), 5);
    EXPECT_EQ(Add(0, 0), 0);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
