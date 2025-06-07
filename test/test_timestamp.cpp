#include <gtest/gtest.h>
#include "../Timestamp.hpp"

TEST(TimestampTest, EpochToString){
    //创建时间戳
    Timestamp t(0);
    EXPECT_EQ(t.toString().substr(0, 10), "1970/01/01");
}
TEST(TimestampTest, NowNotEmpty) {
    Timestamp t = Timestamp::now();
    EXPECT_FALSE(t.toString().empty());
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}