/* ****************************************************************************
 *
 * FILE            RateTest.cpp
 *
 * AUTHOR          Ken Zangelin
 *
 * DATE            Nov 2012
 *
 *
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 *
 * DESCRIPTION
 * unit testing of the class Rate in the samson common library
 *
 */
#include "gtest/gtest.h"

#include "samson/common/Rate.h"

// -----------------------------------------------------------------------------
// Test the Rate constructor
//
TEST(samson_common_Rate, constructor) {
  samson::Rate rate;

  EXPECT_EQ(rate.get_total_size(), 0);
}

// -----------------------------------------------------------------------------
// Test the 'push' methods
//
TEST(samson_common_Rate, push) {
  samson::Rate rate;

  rate.push(5, 10);
  EXPECT_EQ(rate.get_total_size(), 10);

  samson::FullKVInfo fInfo(10, 5);
  rate.push(fInfo);
  EXPECT_EQ(rate.get_total_size(), 20);
}

// -----------------------------------------------------------------------------
// Test the 'get' methods
//
TEST(samson_common_Rate, get) {
  samson::Rate rate;

  rate.push(5, 10);
  EXPECT_EQ(rate.get_total_kvs(), 5);
  EXPECT_EQ(rate.get_rate_size(), 10);
  EXPECT_EQ(rate.get_rate_kvs(), 5);
}  
