#include "engine/ReadFileManager.h"
#include "gtest/gtest.h"
#include "logMsg/logMsg.h"

TEST(engine_ReadFileManager, engine_idTest)
{
  engine::ReadFileManager  read_file_manager;
  std::string              file_name  = "/dev/kk/ingenting";
  engine::ReadFile*        rfP;

  LM_M(("GetReadFile(%s)", file_name.c_str()));
  rfP = read_file_manager.GetReadFile(file_name);  // Should this not fail?
  LM_M(("GetReadFile(%s): %p", file_name.c_str(), rfP));

  EXPECT_EQ(NULL, rfP);

  file_name = "does_not_exist";
  rfP = read_file_manager.GetReadFile(file_name);
  EXPECT_EQ(NULL, rfP);

  file_name = "test_data/testdata.txt";
  rfP = read_file_manager.GetReadFile(file_name);
  if (rfP == NULL)
	 EXPECT_STREQ("rfP == NULL", "Shouldn't be");

  //
  // Open a hundred files and then another one ...
  //
  for (int i = 0; i < 99; i++)
  {
	 rfP = read_file_manager.GetReadFile(file_name);
	 if (rfP == NULL)
		EXPECT_STREQ("rfP == NULL", "Shouldn't be");
  }

  //
  // This call should exercise the while loop, removing old File descriptors in ReadFileManager::GetReadFile
  //
  rfP = read_file_manager.GetReadFile(file_name);
  if (rfP == NULL)
	 EXPECT_STREQ("rfP == NULL", "Shouldn't be");
}
