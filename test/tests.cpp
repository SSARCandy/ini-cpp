#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "ini/INIReader.h"

using namespace inih;

TEST(INIReader, get_sections) {
    INIReader r{"./fixtures/config.ini"};

    const std::set<std::string> ans = {"section1", "section2"};

    EXPECT_EQ(r.Sections(), ans);
}

TEST(INIReader, get_keys) {
    INIReader r{"./fixtures/config.ini"};

    const std::set<std::string> ans = {"any", "any2", "not_int", "not_int_arr"};

    EXPECT_EQ(r.Keys("section1"), ans);
}

TEST(INIReader, get_single_value) {
    INIReader r{"./fixtures/config.ini"};

    EXPECT_EQ(r.Get<>("section1", "any"), std::string("1"));
    EXPECT_EQ(r.Get<float>("section1", "any"), float(1.0));
    EXPECT_EQ(r.Get<double>("section1", "any"), double(1.0));
    EXPECT_EQ(r.Get<long>("section1", "any"), long(1));
    EXPECT_EQ(r.Get<unsigned long>("section1", "any"), (unsigned long)(1));

    EXPECT_EQ(r.Get("section1", "any2"), std::string("true"));
    EXPECT_EQ(r.Get<bool>("section1", "any2"), true);
}

TEST(INIReader, get_vector) {
    INIReader r{"./fixtures/config.ini"};

    const std::vector<int> ans1{1, 2, 3};
    const std::vector<std::string> ans2{"1", "2", "3"};

    ASSERT_EQ(r.GetVector<int>("section2", "any_vec"), ans1);
    ASSERT_EQ(r.GetVector<>("section2", "any_vec"), ans2);
}

TEST(INIReader, get_single_value_with_default) {
    INIReader r{"./fixtures/config.ini"};

    EXPECT_EQ(r.Get<std::string>("section1", "not_exist", "1"),
              std::string("1"));
    EXPECT_EQ(r.Get<int>("section1", "not_exist", 1), int(1));
    EXPECT_EQ(r.Get<float>("section1", "not_exist", 1), float(1.0));
    EXPECT_EQ(r.Get<double>("section1", "not_exist", 1), double(1.0));
    EXPECT_EQ(r.Get<long>("section1", "not_exist", 1), long(1));
    EXPECT_EQ(r.Get<unsigned long>("section1", "not_exist", 1),
              (unsigned long)(1));
    EXPECT_EQ(r.Get<bool>("section1", "not_exist", true), true);

    EXPECT_EQ(r.Get<bool>("section1", "any2", false), true);
}

TEST(INIReader, get_vector_with_default) {
    INIReader r{"./fixtures/config.ini"};

    const std::vector<int> ans1{1, 2, 3};
    const std::vector<std::string> ans2{"1", "2", "3"};
    const std::vector<double> ans3{1.23, 4.56};

    const auto& vec1 = r.GetVector<int>("section2", "not_exist", ans1);
    const auto& vec2 = r.GetVector<std::string>(
        "section2", "not_exist", std::vector<std::string>{"1", "2", "3"});
    const auto& vec3 =
        r.GetVector<double>("section2", "doubles", std::vector<double>{0});

    ASSERT_EQ(vec1, ans1);
    ASSERT_EQ(vec2, ans2);
    ASSERT_EQ(vec3, ans3);
}

TEST(INIReader, exception) {
    EXPECT_THROW(INIReader{"QQ"}, std::runtime_error);  // file not found
    EXPECT_THROW(INIReader{"./fixtures/bad_file.ini"},
                 std::runtime_error);  // parse error

    INIReader r{"./fixtures/config.ini"};

    // section not found error
    EXPECT_THROW(r.Get("section3"), std::runtime_error);

    // key not found error
    EXPECT_THROW(r.Get<int>("section1", "not_exist"), std::runtime_error);
    EXPECT_THROW(r.GetVector<int>("section1", "not_exist"), std::runtime_error);

    // parse error
    EXPECT_THROW(r.Get<int>("section1", "not_int"), std::runtime_error);
    EXPECT_THROW(r.Get<bool>("section1", "not_int"), std::runtime_error);
    EXPECT_THROW(r.GetVector<int>("section1", "not_int_arr"),
                 std::runtime_error);
}

TEST(INIReader, read_big_file) {
    INIReader r{"./fixtures/bigfile.ini"};

    for (int i = 1; i <= 1000; ++i) {
        const auto& v = r.Get<int>("section", "key" + std::to_string(i));
        EXPECT_EQ(v, i);
    }
}

TEST(INIReader, dulicate_keys) {
    EXPECT_THROW(INIReader r{"./fixtures/duplicate_keys.ini"},
                 std::runtime_error);
}
