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

  std::string process_item_description2(void)              { return process_item_description(); }
  void        set_process_item_description2(std::string s) { set_process_item_description(s); }
  void        set_progress2(double p)                      { set_progress(p); }
  void        set_process_item_current_task_description2(std::string s) { set_process_item_current_task_description(s); }
};



//
// general
//
TEST(engine_ProcessItem, general) {
  init_engine_test();

  ProcessItemExample  item;
  std::string         s;
  double              d;

  EXPECT_STREQ("Queued waiting  00:00:00 : 5", item.process_item_status().c_str()) << "Error getting status";
  EXPECT_STREQ("Process Item 'unknown' Status: Queued waiting  00:00:00 : 5", item.str().c_str()) << "Error in description";
  EXPECT_FALSE(item.running()) << "Process item is not supposed to be running at this point";
  EXPECT_EQ(0, item.progress());

  item.StartActivity();
  EXPECT_TRUE(item.running()) << "Process item should be running at this point";
  EXPECT_FALSE(item.finished()) << "Process item should not be finished at this point";

  item.set_process_item_description2("Running");
  item.set_process_item_current_task_description2("Running");
  s = item.process_item_current_task_description();
  LM_M(("process_item_current_task_description: '%s'", s.c_str()));
  EXPECT_TRUE(s == "Running");

  s = item.process_item_status();

  item.StopActivity();
  EXPECT_FALSE(item.running()) << "Process item is not supposed to be running at this point";

  item.set_process_item_description2("Stopped");
  s = item.process_item_status();
  s = item.process_item_description2();
  
  item.AddListener(11);

  std::set<size_t> listeners = item.listeners();
  EXPECT_EQ(1, listeners.size());

  au::Environment  e  = item.environment();
  au::ErrorManager em = item.error();

  item.set_progress2(19.7);
  d = item.progress();
  EXPECT_EQ(19.7, d);
  s = item.process_item_status();

  const au::CronometerSystem rc  = item.running_cronometer();
  const au::CronometerSystem wc  = item.waiting_cronometer();
  size_t                     rts = item.running_time_seconds();
  size_t                     wts = item.waiting_time_seconds();
  EXPECT_EQ(0, rts);
  EXPECT_EQ(0, wts);
  // How do I cerify that the chronos are ok?

  close_engine_test();
}
