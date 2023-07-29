#include <gtest/gtest.h>
#include <fair_shared_mutex.h>

TEST(FairSharedMutexTest, LockTest) {
    EXPECT_EQ(2, 5);
    EXPECT_EQ(5, 0);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
