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
#include <fcntl.h>

#include "au/network/RESTService.h"
#include "au/network/RESTServiceCommand.h"
#include "gtest/gtest.h"
#include "unitTest/TestClasses.h"

TEST(au_mutex_Token, basic) {
  au::Token token("Test au_mutex_Token");

  EXPECT_FALSE(token.IsRetainedByMe());

  {
    au::TokenTaker tt(&token);
    EXPECT_TRUE(token.IsRetainedByMe());
  }

  EXPECT_FALSE(token.IsRetainedByMe());
}
