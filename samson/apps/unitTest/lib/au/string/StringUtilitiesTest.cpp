#include "au/string/StringUtilities.h"
#include "gtest/gtest.h"


TEST(au_string_StringUtilities, simple) {
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

  EXPECT_EQ(au::IsOneOf('d', "abcd"), true) << "Error in isOneOf() true";
  EXPECT_EQ(au::IsOneOf('*', "abcd"), false) << "Error in isOneOf() false";

  std::string str_1 = "abc";
  std::string str_2 = "abcd";
  std::string str_3 = "abcde";
  std::string str_4 = "123";

  EXPECT_EQ(au::GetCommonChars(str_1, str_2), 3) << "Error in getCommonChars() ";
  EXPECT_EQ(au::IsCharInRange('b', 'a', 'c'), true) << "Error in IsCharInRange() true ";
  EXPECT_EQ(au::IsCharInRange('d', 'a', 'c'), false) << "Error in IsCharInRange() false ";

  EXPECT_TRUE(au::CheckIfStringsBeginWith("abcd","abc" )) << "Error in CheckIfStringsBeginWith() true";
  EXPECT_FALSE(au::CheckIfStringsBeginWith("abcd","z")) << "Error in CheckIfStringsBeginWith() false";
  EXPECT_FALSE(au::CheckIfStringsBeginWith("abcd","abcz")) << "Error in CheckIfStringsBeginWith() false";

  EXPECT_EQ(au::CheckIfStringsBeginWith(str_3, "abc"), true) << "Error in string_begins() true";
  EXPECT_EQ(au::CheckIfStringsBeginWith(str_4, "abc"), false) << "Error in string_begins() false";

  EXPECT_EQ(au::CheckIfStringsEndsWith(str_3, "cde"), true) << "Error in CheckIfStringsEndsWith() true";
  EXPECT_EQ(au::CheckIfStringsEndsWith(str_4, "cde"), false) << "Error in CheckIfStringsEndsWith() false";
  EXPECT_EQ(au::CheckIfStringsEndsWith(str_4, "1234"), false) << "Error in CheckIfStringsEndsWith() false";

  EXPECT_EQ(au::CheckIfStringBeginsAndEndsWith(str_3, "ab", "de"), true) << "Error in CheckIfStringBeginsAndEndsWith() true";
  EXPECT_EQ(au::CheckIfStringBeginsAndEndsWith(str_3, "abcd", "cde"), true) << "Error in CheckIfStringBeginsAndEndsWith() true";
  EXPECT_EQ(au::CheckIfStringBeginsAndEndsWith(str_3, "abcdefg", "cde"), false) << "Error in CheckIfStringBeginsAndEndsWith() false";
  EXPECT_EQ(au::CheckIfStringBeginsAndEndsWith(str_3, "ab", "bcdef"), false) << "Error in CheckIfStringBeginsAndEndsWith() false";
  EXPECT_EQ(au::CheckIfStringBeginsAndEndsWith(str_3, "123", "cde"), false) << "Error in CheckIfStringBeginsAndEndsWith() false";
  EXPECT_EQ(au::CheckIfStringBeginsAndEndsWith(str_3, "ab", "1234"), false) << "Error in CheckIfStringBeginsAndEndsWith() false";

  std::string lines = "123\nabc\n";
  EXPECT_EQ(au::ReverseLinesOrder(lines), "abc\n123\n") << "Error in ReverseLinesOrder() false";
}


TEST ( au_string_StringUtilities  , SplitInWords )
{
  char line[1024];
  sprintf(line, "One,Two,Three,Four");

  std::vector<char *> fields = au::SplitInWords( line , ',');
  
  EXPECT_EQ( fields.size() , 4);
  EXPECT_STREQ( fields[0] , "One");
  EXPECT_STREQ( fields[1] , "Two");
  EXPECT_STREQ( fields[2] , "Three");
  EXPECT_STREQ( fields[3] , "Four");

}
TEST ( au_string_StringUtilities , str_vector )
{
   std::vector<int> numbers;
   numbers.push_back(1);
   numbers.push_back(4);
   numbers.push_back(5);

   std::string s = au::str( numbers );
   EXPECT_EQ( "[ 1 4 5 ]" , s );

}
