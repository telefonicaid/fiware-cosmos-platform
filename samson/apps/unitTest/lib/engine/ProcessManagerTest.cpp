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


class ProcessItemTest : public engine::ProcessItem
{
public:
  ProcessItemTest(int prio) : ProcessItem(prio) {}
  void run()                                    {}
};

TEST(DISABLED_engine_ProcessManager, instantiationTest) {
  init_engine_test();

  {
    au::SharedPointer<ProcessItemTest>    item(new ProcessItemTest(9));
    engine::ProcessManager*               process_manager = engine::Engine::process_manager();

    EXPECT_TRUE(process_manager != NULL) << "ProcessManager instance should not be null after instantiation";
    EXPECT_EQ(4, process_manager->max_num_procesors());
    EXPECT_EQ(0, process_manager->num_used_procesors());
  
    size_t x = 71;
    process_manager->Add(item.static_pointer_cast<engine::ProcessItem>(), x);
    // EXPECT_EQ(1, process_manager->num_used_procesors());  Timing issue?
    process_manager->Cancel(item.static_pointer_cast<engine::ProcessItem>());
    // EXPECT_EQ(0, process_manager->num_used_procesors());  Timing issue?
     
    engine::Notification notification("notification_name");
    process_manager->notify(&notification);
  }

  close_engine_test();
}
