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

#include "au/CommandLine.h"
#include "au/statistics/CounterCollection.h"
#include "au/string/StringComponents.h"
#include "gtest/gtest.h"


TEST(au_StringComponents, simple) {
  const char *line = "This is a line|to test|somehing|with StringComponents";
  au::StringComponents string_components;

  size_t ans = string_components.ProcessLine(line, strlen(line), '|');

  EXPECT_TRUE(ans == strlen(line));
  EXPECT_TRUE(string_components.components.size() == 4);
  EXPECT_TRUE(strcmp("This is a line", string_components.components[0]) == 0);
  EXPECT_TRUE(strcmp("with StringComponents", string_components.components[3]) == 0);
}
