/* ****************************************************************************
 *
 * FILE            SamsonFile_test.cpp
 *
 * AUTHOR          Gregorio Escalada
 *
 * DATE            May 2012
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
 *
 * unit testing of the SamsonFile class in the samson/common library
 *
 */
#include "gtest/gtest.h"

#include "logMsg/logMsg.h"

#include "engine/MemoryManager.h"
#include "samson/common/SamsonFile.h" // samson::Delilah
#include "samson/common/ports.h"      // for SAMSON_WORKER_PORT
#include "unitTest/common_engine_test.h"

// Test void samson_file();
TEST(samson_common_SamsonFile, basic) {
  // Init Engine
  init_engine_test();

  // Check error is detected in non-exiting files
  // ------------------------------------------------------------

  {
    au::ErrorManager error;
    au::SharedPointer<samson::SamsonFile> samon_file = samson::SamsonFile::create("/tmp/non_existing_file", error);
    EXPECT_EQ(error.HasErrors(), true) << "Non detected missing file in SamsonFile";
    EXPECT_EQ(error.GetLastError(),
              "Error reading file /tmp/non_existing_file (No such file or directory)") <<
    "Wrong error message for /tmp/non_existing_file";
  }

  // Check error is detected in wrong files
  // ------------------------------------------------------------

  {
    au::ErrorManager error;
    au::SharedPointer<samson::SamsonFile> samon_file = samson::SamsonFile::create("/bin/bash", error);

    EXPECT_EQ(error.HasErrors(), true) << "Non detected missing file in SamsonFile";
    EXPECT_EQ(error.GetLastError(), "KVHeader error: wrong magic number") << "Wrong error message";
  }

  engine::Engine::StopEngine();
}
