////#include <gooleTests/googletest/src/gtest-internal-inl.h> 
////#include "unittest-cpp/UnitTest++/UnitTest++.h"
#include <gtest/gtest.h>
////
//
TEST(ExampleTest, BasicAssertions) {
   EXPECT_EQ(1 + 1, 2);
}
TEST(ExampleTest2, BasicAssertions) {
   EXPECT_EQ(1 , 2);
}
TEST(ExampleTest3, BasicAssertions) {
	EXPECT_EQ(1 - 3, 2);
}

int main(int argc, char** argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}

// Example test

