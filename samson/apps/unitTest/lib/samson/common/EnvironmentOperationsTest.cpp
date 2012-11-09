/* ****************************************************************************
 *
 * FILE            EnvironmentOperationsTest.cpp
 *
 * AUTHOR          Ken Zangelin
 *
 * DATE            November 2012
 *
 * DESCRIPTION
 *
 * Unit testing of the functions in samson/common/EnvironmentOperations
 *
 *
 * Telefonica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefonica Investigacion y Desarrollo S.A.U.
 * All rights reserved.
 */
#include <string>

#include "gtest/gtest.h"

#include "samson/module/Environment.h"
#include "samson/common/samson.pb.h"
#include "samson/common/EnvironmentOperations.h"

namespace samson {

// -----------------------------------------------------------------------------
// moduleToGbp - copy samson module env to GPB env
//
TEST(samson_common_EnvironmentOperations, moduleToGbp) {
  Environment        moduleEnv;
  gpb::Environment   gpbEnv;

  moduleEnv.set("zero", "0");
  moduleEnv.set("one",  "1");

  copyEnviroment(&moduleEnv, &gpbEnv);

  EXPECT_EQ(2, gpbEnv.variable_size());

  gpb::EnvironmentVariable var0 = gpbEnv.variable(1);
  gpb::EnvironmentVariable var1 = gpbEnv.variable(0);

  EXPECT_STREQ("zero", var0.name().c_str());
  EXPECT_STREQ("one",  var1.name().c_str());

  EXPECT_STREQ("0", var0.value().c_str());
  EXPECT_STREQ("1",  var1.value().c_str());
}

}
