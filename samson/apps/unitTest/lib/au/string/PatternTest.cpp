
#include "au/string/Pattern.h"
#include "gtest/gtest.h"


TEST(au_string_Pattern, pattern) {
  au::ErrorManager error;
  au::Pattern pattern("^A", error);

  EXPECT_FALSE(error.IsActivated());
  EXPECT_TRUE(pattern.match("Andreu"));
  EXPECT_FALSE(pattern.match("Pepe"));
}

TEST(au_string_Pattern, simple_pattern) {
  au::SimplePattern pattern("*.txt");

  EXPECT_TRUE(pattern.match("andreu.txt"));
  EXPECT_FALSE(pattern.match("andreu.gif"));
}
