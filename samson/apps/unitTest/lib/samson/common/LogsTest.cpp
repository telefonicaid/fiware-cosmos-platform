/* ****************************************************************************
 *
 * FILE            LogsTest.cpp
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
 *
 * DESCRIPTION
 *
 * unit test of the file 'Logs.cpp' in the samson/common library
 *
 */
#include "gtest/gtest.h"

#include "au/Log.h"
#include "engine/Logs.h"
#include "samson/common/Logs.h"

// -----------------------------------------------------------------------------
// registerLogChannels1 - test 'manual' registration of log channels
//
TEST(DISABLED_samson_common_Logs, registerLogChannels1) {
  samson::LogChannels logChannels;

  logChannels.RegisterChannels();
  
  logChannels.RegisterChannels(); // call it again to exercise 'error handling'
}

// -----------------------------------------------------------------------------
// registerLogChannels - test the utility function that registrates log channels
//
TEST(DISABLED_samson_common_Logs, registerLogChannels2) {
   samson::RegisterLogChannels();
}
