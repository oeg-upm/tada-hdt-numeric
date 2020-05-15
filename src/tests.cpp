#include <gtest/gtest.h>


namespace {

    TEST(ProfilerTest, hello){
        ASSERT_TRUE(true);
    }


//    TEST(ProfilerTest, profiler){

//    }

}// namespace



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
