/* ****************************************************************************
 *
 * FILE            processManager_test.cpp
 *
 * AUTHOR          Javier Lois
 *
 * DATE            January 2012
 *
 * DESCRIPTION
 *
 * unit testing of the ProcessManager class in the engine library
 *
 */

// #include <direct.h> // for getcwd
#include <stdlib.h>  // for MAX_PATH

#include "gtest/gtest.h"

#include "engine/Notification.h"
#include "engine/ProcessItem.h"
#include "engine/ProcessManager.h"

#include "xmlmarkup/xmlmarkup.h"

#include "unitTest/common_engine_test.h"



TEST(processManagerTest, instantiationTest) {
  init_engine_test();

  EXPECT_TRUE(engine::Engine::process_manager() !=
              static_cast<engine::ProcessManager *>(NULL)) <<
  "ProcessManager instance should not be null after instantiation";

  close_engine_test();
}


