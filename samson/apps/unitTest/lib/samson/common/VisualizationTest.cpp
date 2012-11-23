/* ****************************************************************************
 *
 * FILE            VisualizationTest.cpp
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
 * unit testing of the common class in the samson  library
 *
 */
#include "gtest/gtest.h"

#include "samson/common/Visualitzation.h"
// ----------------------------------------------------------------------------
// constructor - 
//
TEST(samson_common_Visualization, constructor) {
  samson::Visualization  visualization;
  samson::Visualization* visualizationP = new samson::Visualization();

  EXPECT_TRUE(visualization.match("a"));
  EXPECT_TRUE(visualizationP != NULL);

  delete visualizationP;
}

// ----------------------------------------------------------------------------
// flags - test the set and get flag methods
//
TEST(samson_common_Visualization, flags) {
  samson::Visualization visualization;

  visualization.set_flag("-a", true);
  visualization.set_flag("-b", false);

  EXPECT_TRUE(visualization.get_flag("-a"));
  EXPECT_TRUE(visualization.get_flag("a"));
  EXPECT_FALSE(visualization.get_flag("-b"));
  EXPECT_FALSE(visualization.get_flag("b"));
  EXPECT_FALSE(visualization.get_flag("-c"));
}

// ----------------------------------------------------------------------------
// patterns - test the set and get pattern methods
//
TEST(samson_common_Visualization, patterns) {
  samson::Visualization visualization;

  visualization.set_pattern("[1-9]*");
  EXPECT_STREQ(visualization.pattern().c_str(), "[1-9]*");
}

// ----------------------------------------------------------------------------
// match - 
//
TEST(samson_common_Visualization, match) {
  samson::Visualization visualization;

  visualization.set_pattern("[1-9]*");
  EXPECT_TRUE(visualization.match("123456789"));
  EXPECT_FALSE(visualization.match("0123456789"));
}

// ----------------------------------------------------------------------------
// environment - 
//
TEST(samson_common_Visualization, environment) {
  samson::Visualization visualization;
  const au::Environment env = visualization.environment();
  
  EXPECT_STREQ(env.str().c_str(), "{}");
}
