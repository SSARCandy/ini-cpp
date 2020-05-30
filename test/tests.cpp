#include <vector>
#include "ini/INIReader.h"
#include "gtest/gtest.h"


using namespace inih;

TEST(INIReader, get_any_single_value) {
    INIReader r{"./fixtures/config.ini"};

    EXPECT_EQ(r.Get<std::string>("section1", "any"), std::string("1"));
    EXPECT_EQ(r.Get<int>("section1", "any"), int(1));
    EXPECT_EQ(r.Get<float>("section1", "any"), float(1.0));
    EXPECT_EQ(r.Get<double>("section1", "any"), double(1.0));
    EXPECT_EQ(r.Get<long>("section1", "any"), long(1));
    EXPECT_EQ(r.Get<unsigned long>("section1", "any"), (unsigned long)(1));

    EXPECT_EQ(r.Get<std::string>("section1", "any2"), std::string("true"));
    EXPECT_EQ(r.Get<bool>("section1", "any2"), true);
}

TEST(INIReader, get_any_vector) {
    INIReader r{"./fixtures/config.ini"};

    const std::vector<int> ans1{1, 2, 3};
    const std::vector<std::string> ans2{"1", "2", "3"};

    const auto& vec1 = r.GetVector<int>("section2", "any_vec");
    const auto& vec2 = r.GetVector<std::string>("section2", "any_vec");
    
    for (int i = 0; i < ans1.size(); ++i) {
        EXPECT_EQ(vec1[i], ans1[i]);
        EXPECT_EQ(vec2[i], ans2[i]);
    }
}

TEST(INIReader, exception) {
    
}