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
  EXPECT_EQ(rate.get_total_kvs(), 0);
}

// -----------------------------------------------------------------------------
// Test the 'push' methods
//
TEST(samson_common_Rate, push) {
  samson::Rate rate;

  rate.push(5, 10);
  EXPECT_EQ(10, rate.get_total_size()) << "should have a total size of 10";
  EXPECT_EQ(5, rate.get_total_kvs()) << "should have a total of 5 kvs";

  samson::FullKVInfo fInfo(10, 5);
  rate.push(fInfo);
  EXPECT_EQ(20, rate.get_total_size()) << "should have a total size of 20";
  EXPECT_EQ(10, rate.get_total_kvs()) << "should have a total of 10 kvs";
}

// -----------------------------------------------------------------------------
// Test the 'get' methods
//
TEST(samson_common_Rate, get) {
  samson::Rate rate;

  rate.push(5, 10);
  EXPECT_EQ(5, rate.get_total_kvs());
  EXPECT_EQ(10, rate.get_total_size());
  EXPECT_TRUE(rate.get_rate_kvs() >= 0);
  EXPECT_TRUE(rate.get_rate_size() >= 0);
}
