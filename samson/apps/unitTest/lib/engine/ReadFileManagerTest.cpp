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
/* ****************************************************************************
 *
 * FILE            ReadFileManagerTest.cpp
 *
 * AUTHOR          Javier Lois
 *
 * DATE            January 2012
 *
 * DESCRIPTION
 *
 * unit testing of the ReadFileManager class in the engine library
 *
 *
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefonica Investigacion y Desarrollo S.A.U.
 * All rights reserved.
 */
#include <fcntl.h>

#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "logMsg/logMsg.h"

#include "engine/ReadFile.h"
#include "engine/ReadFileManager.h"

// -----------------------------------------------------------------------------
// dataFilesCreate - create X data files in the directory indicated by 'base'
//
static bool dataFilesCreate(std::string base, std::string content, int numberOfFiles) {
  for (int fIx = 0; fIx < numberOfFiles; fIx++) {
    char fileName[256];

    snprintf(fileName, sizeof(fileName), "%s%03d", base.c_str(), fIx);

    int fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd == -1) {
      LM_E(("error opening '%s'", fileName));
      return false;
    }

    int nb = write(fd, content.c_str(), sizeof(content.c_str()));
    if (nb != sizeof(content.c_str())) {
      LM_E(("error writing to '%s'", fileName));
      close(fd);
      return false;
    }

    close(fd);
  }

  return true;
}

// -----------------------------------------------------------------------------
// dataFilesRemove
//
static void dataFilesRemove(std::string base, int numberOfFiles) {
  for (int fIx = 0; fIx < numberOfFiles; fIx++) {
    char fileName[256];

    snprintf(fileName, sizeof(fileName), "%s%03d", base.c_str(), fIx);
    unlink(fileName);
  }
}

// -----------------------------------------------------------------------------
// idTest
//
TEST(engine_ReadFileManager, engine_idTest) {
  engine::ReadFileManager  read_file_manager;
  std::string              file_name  = "/path/to/nowhere";
  engine::ReadFile*        rfP;
  LM_M(("Here"));

  rfP = read_file_manager.GetReadFile(file_name);
  EXPECT_EQ(NULL, rfP) << "'/path/to/nowhere' should not be OK ...";

  rfP = read_file_manager.GetReadFile("does_not_exist");
  EXPECT_EQ(NULL, rfP) << "'does_not_exist' should not be found";
  LM_M(("Here"));

  rfP = read_file_manager.GetReadFile("test_data/testdata.txt");
  EXPECT_NE((engine::ReadFile*) NULL, rfP) << "'test_data/testdata.txt' should be found!";

  LM_M(("Here"));

  // Create data files
  bool testFilesOk = dataFilesCreate("test_data/testdata", "012345678901234567890123456789\n", 102);
  EXPECT_EQ(true, testFilesOk);

  //
  // Open more than a hundred files ...
  //
  char suffix[8];

  file_name = "test_data/testdata";

  for (int i = 0; i <= 100; i++) {
    snprintf(suffix, sizeof(suffix), "%03d", i);
    rfP = read_file_manager.GetReadFile(file_name + suffix);

    EXPECT_NE((engine::ReadFile*) NULL, rfP);
  }

  //
  // This call should exercise the while loop, removing old File descriptors
  // in ReadFileManager::GetReadFile
  //
  rfP = read_file_manager.GetReadFile(file_name + "100");
  EXPECT_FALSE(rfP == NULL);

  dataFilesRemove("test_data/testdata", 102);
}
