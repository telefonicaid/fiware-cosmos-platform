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


#include "au/statistics/Rate.h"
#include "gtest/gtest.h"

TEST(au_Rate, basic) {
  au::Rate rate;

  rate.Push(1);
  rate.Push(2);
  rate.Push(3);

  EXPECT_EQ((size_t)6, rate.size());
  EXPECT_EQ((size_t)3, rate.hits());

  EXPECT_TRUE(rate.rate() >= 0);
  EXPECT_TRUE(rate.hit_rate() >= 0);
}
