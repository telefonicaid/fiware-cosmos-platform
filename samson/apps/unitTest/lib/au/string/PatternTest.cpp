/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#include "au/string/Pattern.h"
#include "gtest/gtest.h"


TEST(au_string_Pattern, pattern) {
  au::ErrorManager error;
  au::Pattern pattern("^A", error);

  EXPECT_FALSE(error.HasErrors());
  EXPECT_TRUE(pattern.match("Andreu"));
  EXPECT_FALSE(pattern.match("Pepe"));
}

TEST(au_string_Pattern, simple_pattern) {
  au::SimplePattern pattern("*.txt");

  EXPECT_TRUE(pattern.match("andreu.txt"));
  EXPECT_FALSE(pattern.match("andreu.gif"));
}
