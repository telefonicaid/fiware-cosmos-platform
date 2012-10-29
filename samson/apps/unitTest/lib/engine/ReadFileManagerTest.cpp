#include "engine/ReadFile.h"
#include "engine/ReadFileManager.h"
#include "gtest/gtest.h"
#include "logMsg/logMsg.h"



//
// idTest
//
TEST(engine_ReadFileManager, engine_idTest) {
  engine::ReadFileManager  read_file_manager;
  std::string              file_name  = "/path/to/nowhere";
  engine::ReadFile*        rfP;

  rfP = read_file_manager.GetReadFile(file_name);
  EXPECT_EQ(NULL, rfP);

  rfP = read_file_manager.GetReadFile("does_not_exist");
  EXPECT_EQ(NULL, rfP);

  rfP = read_file_manager.GetReadFile("test_data/testdata.txt");
  if (rfP == NULL)
    EXPECT_STREQ("rfP == NULL", "Shouldn't be");

  //
  // Open more than a hundred files ...
  //
  char suffix[8];
  file_name = "test_data/testdata";
  for (int i = 0; i <= 101; i++)
  {
    sprintf(suffix, "%02d", i);
    LM_M(("reading file '%s'", (file_name + suffix).c_str()));
    rfP = read_file_manager.GetReadFile(file_name + suffix);

    if (rfP == NULL) {
      LM_E(("GetReadFile error for file %d", i));
      EXPECT_STREQ("rfP == NULL", "Shouldn't be");
    }
    else {
      if (i >= 100)
        rfP->Close();
    }
  }

  //
  // This call should exercise the while loop, removing old File descriptors in ReadFileManager::GetReadFile
  //
  rfP = read_file_manager.GetReadFile(file_name + "100");
  if (rfP == NULL)
    EXPECT_STREQ("rfP == NULL", "Shouldn't be");
}
