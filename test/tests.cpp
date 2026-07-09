#include <cstdio>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "ini/ini.h"

using namespace inih;

// Write a runtime-generated fixture (binary mode: exact bytes, no newline
// translation). Generated fixtures are named gen_* and git-ignored.
static void write_file(const std::string& path, const std::string& content) {
    std::ofstream out{path, std::ios::binary};
    out << content;
}

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
    const std::vector<std::string> ans2{"1", "2", "3", "4"};
    const std::vector<double> ans3{1.23, 4.56};

    const auto& vec1 = r.GetVector<int>("section2", "not_exist", ans1);
    const auto& vec2 = r.GetVector<std::string>(
        "section2", "not_exist", std::vector<std::string>{"1", "2", "3", "4"});
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
    EXPECT_THROW(INIReader{"./fixtures/duplicate_keys.ini"},
                 std::runtime_error);
}

TEST(INIReader, InsertEntry) {
    INIReader r{"./fixtures/config.ini"};

    // section exist, key not exist
    r.InsertEntry("section1", "my_custom_key", "hello world");

    // section&key not exist
    r.InsertEntry("new_section", "key1", 5);

    EXPECT_EQ("hello world", r.Get("section1", "my_custom_key"));
    EXPECT_EQ(5, r.Get<int>("new_section", "key1"));
}

TEST(INIReader, UpdateEntry) {
    INIReader r{"./fixtures/config.ini"};
    r.InsertEntry("section1", "my_custom_key", "hello world");

    r.UpdateEntry("section1", "my_custom_key", 123);
    EXPECT_EQ(123, r.Get<int>("section1", "my_custom_key"));

    std::vector<double> ans1{0.1, 0.2, 0.3};
    r.UpdateEntry("section1", "my_custom_key", ans1);
    for (size_t i = 0; i < ans1.size(); ++i) {
        EXPECT_EQ(ans1[i], r.GetVector<double>("section1", "my_custom_key")[i]);
    }
}

TEST(INIWriter, write) {
    INIReader r{"./fixtures/config.ini"};
    r.InsertEntry("new_section", "key1", "123");
    r.InsertEntry("new_section", "key2", 5.5);
    r.InsertEntry("new_section", "key3", std::vector<double>{0.1, 0.2, 0.3});
    r.InsertEntry("new_section", "key4", std::vector<std::string>{"a", "b"});

    system("rm -rf ./fixtures/output.ini");
    INIWriter::write("./fixtures/output.ini", r);

    INIReader r2{"./fixtures/output.ini"};
    for (const auto& section : r.Sections()) {
        for (const auto& key : r.Keys(section)) {
            EXPECT_EQ(r.Get(section, key), r2.Get(section, key));
        }
    }
}

TEST(INIReader, from_file_pointer) {
    FILE* fp = fopen("./fixtures/config.ini", "r");
    ASSERT_NE(fp, nullptr);
    INIReader r{fp};
    fclose(fp);

    EXPECT_EQ(r.Get<int>("section1", "any"), 1);
    EXPECT_EQ(r.ParseError(), 0);
}

TEST(INIReader, utf8_bom) {
    write_file("./fixtures/gen_bom.ini", "\xEF\xBB\xBF[s]\nk = 1\n");
    INIReader r{"./fixtures/gen_bom.ini"};

    EXPECT_EQ(r.Get<int>("s", "k"), 1);
}

TEST(INIReader, crlf_line_endings) {
    write_file("./fixtures/gen_crlf.ini", "[s]\r\na = 1\r\nb = two\r\n");
    INIReader r{"./fixtures/gen_crlf.ini"};

    EXPECT_EQ(r.Get<int>("s", "a"), 1);
    EXPECT_EQ(r.Get("s", "b"), "two");
}

TEST(INIReader, comments) {
    write_file("./fixtures/gen_comments.ini",
               "; full-line comment\n"
               "# also a full-line comment\n"
               "[c]\n"
               "a = 1 ; inline comment starts after whitespace\n"
               "b =;no leading whitespace, not a comment\n"
               "c = hash # is not an inline comment prefix\n"
               "d = semi;colon inside value stays\n");
    INIReader r{"./fixtures/gen_comments.ini"};

    EXPECT_EQ(r.Get("c", "a"), "1");
    EXPECT_EQ(r.Get("c", "b"), ";no leading whitespace, not a comment");
    EXPECT_EQ(r.Get("c", "c"), "hash # is not an inline comment prefix");
    EXPECT_EQ(r.Get("c", "d"), "semi;colon inside value stays");
}

TEST(INIReader, whitespace_and_empty_values) {
    write_file("./fixtures/gen_ws.ini",
               "[w]\n"
               "empty =\n"
               "spaced    =      42\t\t\n"
               "name with spaces = ok\n"
               "eq = a=b\n"
               "no_trailing_newline = 1");
    INIReader r{"./fixtures/gen_ws.ini"};

    EXPECT_EQ(r.Get("w", "empty"), "");
    EXPECT_EQ(r.GetVector<int>("w", "empty"), std::vector<int>{});
    EXPECT_EQ(r.Get<int>("w", "spaced"), 42);
    EXPECT_EQ(r.Get("w", "name with spaces"), "ok");
    EXPECT_EQ(r.Get("w", "eq"), "a=b");
    EXPECT_EQ(r.Get<int>("w", "no_trailing_newline"), 1);
}

TEST(INIReader, section_quirks) {
    write_file("./fixtures/gen_sections.ini",
               "[empty_section]\n"
               "[a]\n"
               "x = 1\n"
               "[b] trailing junk is ignored\n"
               "y = 2\n"
               "[a]\n"
               "z = 3\n");
    INIReader r{"./fixtures/gen_sections.ini"};

    // sections with no keys are not listed; reopened sections are merged
    const std::set<std::string> ans = {"a", "b"};
    EXPECT_EQ(r.Sections(), ans);
    EXPECT_EQ(r.Get<int>("a", "x"), 1);
    EXPECT_EQ(r.Get<int>("a", "z"), 3);
    EXPECT_EQ(r.Get<int>("b", "y"), 2);

    // duplicate key in a reopened section still throws
    write_file("./fixtures/gen_sections_dup.ini", "[a]\nx = 1\n[a]\nx = 2\n");
    EXPECT_THROW(INIReader{"./fixtures/gen_sections_dup.ini"},
                 std::runtime_error);
}

TEST(INIReader, empty_file) {
    write_file("./fixtures/gen_empty.ini", "");
    INIReader r{"./fixtures/gen_empty.ini"};

    EXPECT_EQ(r.Sections(), std::set<std::string>{});
    EXPECT_EQ(r.ParseError(), 0);
}

TEST(INIReader, long_lines_and_section_names) {
    // no line-length limit and no section-name truncation (the legacy
    // parser capped these at 2000 and 49 chars)
    const std::string long_value(5000, 'x');
    const std::string long_section(120, 's');
    write_file("./fixtures/gen_long.ini",
               "[" + long_section + "]\nk = " + long_value + "\n");
    INIReader r{"./fixtures/gen_long.ini"};

    EXPECT_EQ(r.Get(long_section, "k"), long_value);
}

TEST(INIReader, numeric_conversions) {
    write_file("./fixtures/gen_num.ini",
               "[n]\n"
               "neg = -42\n"
               "pos = +5\n"
               "sci = 1e3\n"
               "big = 99999999999999999999999999\n");
    INIReader r{"./fixtures/gen_num.ini"};

    EXPECT_EQ(r.Get<int>("n", "neg"), -42);
    EXPECT_EQ(r.Get<int>("n", "pos"), 5);
    EXPECT_EQ(r.Get<double>("n", "sci"), 1000.0);
    EXPECT_THROW(r.Get<int>("n", "big"), std::runtime_error);  // overflow
}

TEST(INIReader, bool_conversions) {
    INIReader r{"./fixtures/config.ini"};
    const std::vector<std::pair<std::string, bool>> cases{
        {"1", true},    {"0", false},   {"true", true}, {"false", false},
        {"yes", true},  {"no", false},  {"on", true},   {"off", false},
        {"TRUE", true}, {"Off", false},
    };
    for (const auto& c : cases) {
        r.InsertEntry("bools", c.first, c.first);
        EXPECT_EQ(r.Get<bool>("bools", c.first), c.second) << c.first;
    }

    r.InsertEntry("bools", "bad", "maybe");
    EXPECT_THROW(r.Get<bool>("bools", "bad"), std::runtime_error);
}

TEST(INIReader, default_on_parse_failure) {
    INIReader r{"./fixtures/config.ini"};

    // unparseable values fall back to the default
    EXPECT_EQ(r.Get<int>("section1", "not_int", 7), 7);
    const std::vector<int> dv{9};
    EXPECT_EQ(r.GetVector<int>("section1", "not_int_arr", dv), dv);
}

TEST(INIReader, error_messages) {
    try {
        INIReader r{"./fixtures/bad_file.ini"};
        FAIL() << "expected std::runtime_error";
    } catch (const std::runtime_error& e) {
        EXPECT_STREQ("parse error on line no: 3", e.what());
    }

    try {
        INIReader r{"QQ"};
        FAIL() << "expected std::runtime_error";
    } catch (const std::runtime_error& e) {
        EXPECT_STREQ("ini file not found.", e.what());
    }

    write_file("./fixtures/gen_badsec.ini", "[never_closed\n");
    try {
        INIReader r{"./fixtures/gen_badsec.ini"};
        FAIL() << "expected std::runtime_error";
    } catch (const std::runtime_error& e) {
        EXPECT_STREQ("parse error on line no: 1", e.what());
    }
}

TEST(INIReader, insert_and_update_errors) {
    INIReader r{"./fixtures/config.ini"};

    // inserting an existing key throws
    EXPECT_THROW(r.InsertEntry("section1", "any", 2), std::runtime_error);
    // updating a missing key/section throws
    EXPECT_THROW(r.UpdateEntry("section1", "not_exist", 1), std::runtime_error);
    EXPECT_THROW(r.UpdateEntry("section3", "any", 1), std::runtime_error);
    // keys/section lookups on missing section throw
    EXPECT_THROW(r.Keys("section3"), std::runtime_error);
}

TEST(INIWriter, exception) {
    INIReader r{"./fixtures/config.ini"};
    EXPECT_THROW(INIWriter::write("./fixtures/config.ini", r),
                 std::runtime_error);
}

TEST(INIWriter, overwrite) {
    INIReader r{"./fixtures/config.ini"};

    std::remove("./fixtures/gen_overwrite.ini");
    INIWriter::write("./fixtures/gen_overwrite.ini", r);
    // overwrite = false (default) refuses to clobber an existing file
    EXPECT_THROW(INIWriter::write("./fixtures/gen_overwrite.ini", r),
                 std::runtime_error);
    // overwrite = true replaces it
    r.InsertEntry("new_section", "k", 1);
    INIWriter::write("./fixtures/gen_overwrite.ini", r, true);

    INIReader r2{"./fixtures/gen_overwrite.ini"};
    EXPECT_EQ(r2.Get<int>("new_section", "k"), 1);
}