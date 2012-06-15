#include "gtest/gtest.h"
#include "au/string.h"


TEST(auStringTest, simple )
{
    EXPECT_EQ( au::str_tabs(2), "\t\t") << "Error in str_tabs()";

    EXPECT_EQ( au::str_time(3661), " 01:01:01") << "Error in str_time less than 1 day()";
    EXPECT_EQ( au::str_time(90061), " 01d 01:01:01") << "Error in str_time more than 1 day()";

    time_t old_time = 1;
    EXPECT_EQ( au::str_timestamp(old_time), "01/01/1970 (00:00:01)") << "Error in str_timestamp()";

    EXPECT_EQ( au::str_percentage(0.7125), "71.25%") << "Error in str_percentage()";
    EXPECT_EQ( au::str_percentage(5.0, 20.0), "25.00%") << "Error in str_percentage()";

    EXPECT_EQ( au::str_progress_bar(0.7125, 10), " [ ********.. ] ") << "Error in str_progress_bar()";
    EXPECT_EQ( au::str_progress_bar(0.7125, 10, '=', '?'), " [ ========?? ] ") << "Error in str_progress_bar()";
    EXPECT_EQ( au::str_double_progress_bar(0.7125, 0.9, 10, '*', '=', '?', 10), " [ ========?? ] ") << "Error in str_progress_bar()";

    EXPECT_EQ( au::getRoot("/tmp/path_txt"), "/tmp/path_txt") << "Error in getRoot()";
    EXPECT_EQ( au::getRoot("/tmp/path.txt"), "/tmp/path") << "Error in getRoot()";

    EXPECT_EQ( au::getRest("/tmp/path_txt"), "") << "Error in getRest()";
    EXPECT_EQ( au::getRest("/tmp/path.txt"), "txt") << "Error in getRest()";

    EXPECT_EQ( au::isOneOf('d', "abcd"), true) << "Error in isOneOf() true";
    EXPECT_EQ( au::isOneOf('*', "abcd"), false) << "Error in isOneOf() false";

    EXPECT_EQ( au::getCommonChars("abc", "abcd"), 3) << "Error in getCommonChars() ";
    EXPECT_EQ( au::isCharInRange('b', 'a', 'c'), true) << "Error in isCharInRange() true ";
    EXPECT_EQ( au::isCharInRange('d', 'a', 'c'), false) << "Error in isCharInRange() false ";

    EXPECT_EQ( au::strings_begin_equal("abc", "abcd"), true) << "Error in strings_begin_equal() true";
    EXPECT_EQ( au::strings_begin_equal("abcz", "abcd"), false) << "Error in strings_begin_equal() false";

    EXPECT_EQ( au::path_from_directory("", "file"), "file") << "Error in path_from_directory() ";
    EXPECT_EQ( au::path_from_directory("/dir", "file"), "/dir/file") << "Error in path_from_directory() ";
    EXPECT_EQ( au::path_from_directory("/dir/", "file"), "/dir/file") << "Error in path_from_directory() ";
    
    EXPECT_EQ( au::string_begins("abcde", "abc"), true) << "Error in string_begins() true";
    EXPECT_EQ( au::string_begins("123", "abc"), false) << "Error in string_begins() false";
    
    EXPECT_EQ( au::string_ends("abcde", "cde"), true) << "Error in string_ends() true";
    EXPECT_EQ( au::string_ends("123", "cde"), false) << "Error in string_ends() false";
    EXPECT_EQ( au::string_ends("123", "1234"), false) << "Error in string_ends() false";
    
    EXPECT_EQ( au::string_begins_and_ends("abcde", "ab", "de"), true) << "Error in string_begins_and_ends() true";
    EXPECT_EQ( au::string_begins_and_ends("abcde", "abcd", "cde"), true) << "Error in string_begins_and_ends() true";
    EXPECT_EQ( au::string_begins_and_ends("abcde", "abcdefg", "cde"), false) << "Error in string_begins_and_ends() false";
    EXPECT_EQ( au::string_begins_and_ends("abcde", "ab", "bcdef"), false) << "Error in string_begins_and_ends() false";
    EXPECT_EQ( au::string_begins_and_ends("abcde", "123", "cde"), false) << "Error in string_begins_and_ends() false";
    EXPECT_EQ( au::string_begins_and_ends("abcde", "ab", "1234"), false) << "Error in string_begins_and_ends() false";
    
    EXPECT_EQ( au::reverse_lines("123\nabc\n", "abc\n123\n"), false) << "Error in reverse_lines() false";
}
