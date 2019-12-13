// Copyright 2018 Your Name <your_email>

#include <gtest/gtest.h>
#include "header.hpp"
TEST(SharedPtr, value )
{
    int*a = new int;
    a[0] = 10;
    int*b = new int;
    b[0] = 9;
    SharedPtr<int> test(a);
    EXPECT_EQ(static_cast<int>(test._adress[0]), 10);
    SharedPtr<int> test2(test);
    EXPECT_EQ(static_cast<int>(test2._adress[0]), 10);
    test.reset(b);
    EXPECT_EQ(static_cast<int>(test._adress[0]), 9);
    test.swap(test2);
    EXPECT_EQ(static_cast<int>(test._adress[0]), 10);
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
