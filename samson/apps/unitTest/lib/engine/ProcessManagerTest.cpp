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



TEST(engine_ProcessManager, instantiationTest) {

  init_engine_test();

  engine::ProcessManager* process_manager = engine::Engine::process_manager();
  
  EXPECT_TRUE( process_manager != NULL ) << "ProcessManager instance should not be null after instantiation";
  EXPECT_EQ(4, process_manager->max_num_procesors());
  EXPECT_EQ(0, process_manager->num_used_procesors());
  
  close_engine_test();
}


