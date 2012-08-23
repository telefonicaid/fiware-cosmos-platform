/* ****************************************************************************
 *
 * FILE            SamsonFile_test.cpp
 *
 * AUTHOR         Gregorio Escalada
 *
 * DATE            May 2012
 *
 * DESCRIPTION
 *
 * unit testing of the delilah class in the samson  library
 *
 */

#include "engine/MemoryManager.h"
#include "gtest/gtest.h"
#include "samson/common/SamsonFile.h"                                     // samson::Delailh
#include "samson/common/ports.h"  // for SAMSON_WORKER_PORT

#include "logMsg/logMsg.h"


// Test void samson_file();
TEST(samson_common_SamsonFile, basic) {
  // Init Engine
  engine::Engine::InitEngine(1, 1024 * 1024 * 1024, 1);

  // Check error is detected in non-exiting files
  // ------------------------------------------------------------

  {
    au::ErrorManager error;
    au::SharedPointer<samson::SamsonFile> samon_file = samson::SamsonFile::create("/tmp/non_existing_file", error);
    EXPECT_EQ(error.IsActivated(), true) << "Non detected missing file in SamsonFile";
    EXPECT_EQ(error.GetMessage(),
              "Error reading file /tmp/non_existing_file (No such file or directory)") <<
    "Wrong error message for /tmp/non_existing_file";
  }

  // Check error is detected in wrong files
  // ------------------------------------------------------------

  {
    au::ErrorManager error;
    au::SharedPointer<samson::SamsonFile> samon_file = samson::SamsonFile::create("/bin/bash", error);

    EXPECT_EQ(error.IsActivated(), true) << "Non detected missing file in SamsonFile";
    EXPECT_EQ(error.GetMessage(), "KVHeader error: wrong magic number") << "Wrong error message";
  }

  engine::Engine::DestroyEngine();
}
