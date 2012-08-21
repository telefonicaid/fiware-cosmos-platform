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

#include "xmlparser/xmlParser.h"


// Test std::string getStatus();
// Test std::string getDescription();
// Test bool isRunning();
TEST(processItemTest, getStatusTest) {
  init_engine_test();

  ProcessItemExample item;

  EXPECT_EQ(item.getStatus(), "Queued : 5 : ") << "Error getting status";
  EXPECT_EQ(item.getDescription(), "Process Item 'unknown' Status: Queued : 5 : ") << "Error in description";
  EXPECT_TRUE(item.running() == false) << "Process item is not supposed to be running at this point";

  close_engine_test();
}
