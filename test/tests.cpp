#include <vector>
#include "ini/INIReader.h"
#include "gtest/gtest.h"


using namespace inih;

TEST(INIReader, get_single_value) {
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

TEST(INIReader, get_vector) {
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

TEST(INIReader, get_single_value_with_default) {
    INIReader r{"./fixtures/config.ini"};

    EXPECT_EQ(r.Get<std::string>("section1", "not_exist", "1"), std::string("1"));
    EXPECT_EQ(r.Get<int>("section1", "not_exist", 1), int(1));
    EXPECT_EQ(r.Get<float>("section1", "not_exist", 1), float(1.0));
    EXPECT_EQ(r.Get<double>("section1", "not_exist", 1), double(1.0));
    EXPECT_EQ(r.Get<long>("section1", "not_exist", 1), long(1));
    EXPECT_EQ(r.Get<unsigned long>("section1", "not_exist", 1), (unsigned long)(1));
    EXPECT_EQ(r.Get<bool>("section1", "not_exist", true), true);

    EXPECT_EQ(r.Get<bool>("section1", "any2", false), true);
}

TEST(INIReader, get_vector_with_default) {
    INIReader r{"./fixtures/config.ini"};

    const std::vector<int> ans1{1, 2, 3};
    const std::vector<std::string> ans2{"1", "2", "3"};
    const std::vector<double> ans3{1.23, 4.56};

    const auto& vec1 = r.GetVector<int>("section2", "not_exist", ans1);
    const auto& vec2 = r.GetVector<std::string>("section2", "not_exist", std::vector<std::string>{"1", "2", "3"});
    const auto& vec3 = r.GetVector<double>("section2", "doubles", std::vector<double>{0});
    
    for (int i = 0; i < ans1.size(); ++i) {
        EXPECT_EQ(vec1[i], ans1[i]);
        EXPECT_EQ(vec2[i], ans2[i]);
        EXPECT_EQ(vec3[i], ans3[i]);
    }
}


TEST(INIReader, exception) {

    // file not found
    EXPECT_THROW(INIReader{"QQ"}, std::runtime_error);

    INIReader r{"./fixtures/config.ini"};

    EXPECT_THROW(r.Get<int>("section1", "not_int"), std::runtime_error);
    EXPECT_THROW(r.GetVector<int>("section1", "not_int_arr"), std::runtime_error);
    
}