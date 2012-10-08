/* ****************************************************************************
 *
 * FILE            processItem_Test.cpp
 *
 * AUTHOR          Javier Lois
 *
 * DATE            January 2012
 *
 * DESCRIPTION
 *
 * unit testing of the ProcessItem class in the engine library
 *
 */

#include "gtest/gtest.h"

#include "engine/Engine.h"
#include "engine/ProcessItem.h"
#include "engine/ProcessManager.h"

#include "unitTest/common_engine_test.h"

// ProcessItem is pure virtual. In order to test it we need to create a derived class

class ProcessItemExample : public engine::ProcessItem {
public:

  ProcessItemExample() : engine::ProcessItem(5) {
  }

  void run() {
    // Do nothing ;)
  }
};

TEST(engine_ProcessItem, general) {
  init_engine_test();

  ProcessItemExample item;

  EXPECT_FALSE(item.running()) << "Process item is not supposed to be running at this point";
  EXPECT_EQ(0, item.progress());

  close_engine_test();
}
