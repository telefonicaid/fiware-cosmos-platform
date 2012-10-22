#include "au/string/string.h"
#include "gtest/gtest.h"


TEST(au_string, simple) {
  EXPECT_EQ(au::str_tabs(2), "\t\t") << "Error in str_tabs()";

  EXPECT_EQ(au::str_time(3661), " 01:01:01") << "Error in str_time less than 1 day()";
  EXPECT_EQ(au::str_time(90061), "01d 01:01") << "Error in str_time more than 1 day()";

  time_t old_time = 1;
  EXPECT_EQ(au::str_timestamp(old_time), "01/01/1970 (01:00:01)") << "Error in str_timestamp()";

  EXPECT_EQ(au::str_percentage(0.7125), "071.2%") << "Error in str_percentage()";
  EXPECT_EQ(au::str_percentage(5.0, 20.0), "025.0%") << "Error in str_percentage()";

  EXPECT_EQ(au::str_progress_bar(0.7125, 10), " [ *******... ] ") << "Error in str_progress_bar()";
  EXPECT_EQ(au::str_progress_bar(0.7125, 10, '=', '?'), " [ =======??? ] ") << "Error in str_progress_bar()";
  EXPECT_EQ(au::str_double_progress_bar(0.7125, 0.9, '*', '=', '?',
                                        10), " [ *******==? ] ") << "Error in str_progress_bar()";

  std::string path = "/tmp/path.txt";
  std::string wrong_path = "/tmp/path_txt";

  EXPECT_EQ(au::getRoot(wrong_path), "/tmp/path_txt") << "Error in getRoot()";
  EXPECT_EQ(au::getRoot(path), "/tmp/path") << "Error in getRoot()";

  EXPECT_EQ(au::getRest(wrong_path), "") << "Error in getRest()";
  EXPECT_EQ(au::getRest(path), "txt") << "Error in getRest()";

  EXPECT_EQ(au::isOneOf('d', "abcd"), true) << "Error in isOneOf() true";
  EXPECT_EQ(au::isOneOf('*', "abcd"), false) << "Error in isOneOf() false";

  std::string str_1 = "abc";
  std::string str_2 = "abcd";
  std::string str_3 = "abcde";
  std::string str_4 = "123";

  EXPECT_EQ(au::getCommonChars(str_1, str_2), 3) << "Error in getCommonChars() ";
  EXPECT_EQ(au::isCharInRange('b', 'a', 'c'), true) << "Error in isCharInRange() true ";
  EXPECT_EQ(au::isCharInRange('d', 'a', 'c'), false) << "Error in isCharInRange() false ";

  EXPECT_EQ(au::strings_begin_equal("abc", "abcd"), true) << "Error in strings_begin_equal() true";
  EXPECT_EQ(au::strings_begin_equal("abcz", "abcd"), false) << "Error in strings_begin_equal() false";

  EXPECT_EQ(au::path_from_directory("", "file"), "file") << "Error in path_from_directory() ";
  EXPECT_EQ(au::path_from_directory("/dir", "file"), "/dir/file") << "Error in path_from_directory() ";
  EXPECT_EQ(au::path_from_directory("/dir/", "file"), "/dir/file") << "Error in path_from_directory() ";

  EXPECT_EQ(au::string_begins(str_3, "abc"), true) << "Error in string_begins() true";
  EXPECT_EQ(au::string_begins(str_4, "abc"), false) << "Error in string_begins() false";

  EXPECT_EQ(au::string_ends(str_3, "cde"), true) << "Error in string_ends() true";
  EXPECT_EQ(au::string_ends(str_4, "cde"), false) << "Error in string_ends() false";
  EXPECT_EQ(au::string_ends(str_4, "1234"), false) << "Error in string_ends() false";

  EXPECT_EQ(au::string_begins_and_ends(str_3, "ab", "de"), true) << "Error in string_begins_and_ends() true";
  EXPECT_EQ(au::string_begins_and_ends(str_3, "abcd", "cde"), true) << "Error in string_begins_and_ends() true";
  EXPECT_EQ(au::string_begins_and_ends(str_3, "abcdefg", "cde"), false) << "Error in string_begins_and_ends() false";
  EXPECT_EQ(au::string_begins_and_ends(str_3, "ab", "bcdef"), false) << "Error in string_begins_and_ends() false";
  EXPECT_EQ(au::string_begins_and_ends(str_3, "123", "cde"), false) << "Error in string_begins_and_ends() false";
  EXPECT_EQ(au::string_begins_and_ends(str_3, "ab", "1234"), false) << "Error in string_begins_and_ends() false";

  std::string lines = "123\nabc\n";
  EXPECT_EQ(au::reverse_lines(lines), "abc\n123\n") << "Error in reverse_lines() false";
}

TEST ( au_string_string , str_vector )
{
   std::vector<int> numbers;
   numbers.push_back(1);
   numbers.push_back(4);
   numbers.push_back(5);

   std::string s = au::str( numbers );
   EXPECT_EQ( "[ 1 4 5]" , s );

}
