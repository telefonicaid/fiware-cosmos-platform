/* ****************************************************************************
 *
 * FILE            ProcessItemTest.cpp
 *
 * AUTHOR          Javier Lois
 *
 * DATE            January 2012
 *
 * DESCRIPTION
 *
 * unit testing of the ProcessItem class in the engine library
 *
 *
 * Telefonica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefonica Investigacion y Desarrollo S.A.U.
 * All rights reserved.
 */
#include <set>
#include <string>

#include "gtest/gtest.h"

#include "engine/Engine.h"
#include "engine/ProcessItem.h"
#include "engine/ProcessManager.h"

#include "unitTest/common_engine_test.h"

// ProcessItem is pure virtual. In order to test it we need to create a derived class
class ProcessItemExample : public engine::ProcessItem {
 public:
  ProcessItemExample() : engine::ProcessItem(5) {}

  void run() { }

  std::string process_item_description2(void) {
     return process_item_description();
  }

  void set_process_item_description2(const std::string& s) {
     set_process_item_description(s);
  }

  void set_progress2(double p) {
     set_progress(p);
  }

  void set_process_item_current_task_description2(const std::string& s) {
     set_process_item_current_task_description(s);
  }
};

// -----------------------------------------------------------------------------
// general
//
TEST(engine_ProcessItem, general) {
  init_engine_test();

  ProcessItemExample  item;
  std::string         s;
  double              d;

  EXPECT_STREQ("Queued waiting  00:00:00 : 5",
               item.process_item_status().c_str()) <<
     "Error getting status";

  EXPECT_STREQ("Process Item 'unknown' Status: Queued waiting  00:00:00 : 5",
               item.str().c_str()) <<
     "Error in description";

  EXPECT_FALSE(item.running()) << "Process item is not supposed to be running at this point";
  EXPECT_EQ(0, item.progress());

  item.StartActivity();
  EXPECT_TRUE(item.running()) << "Process item should be running at this point";
  EXPECT_FALSE(item.finished()) << "Process item should not be finished at this point";

  std::string running = "Running";
  item.set_process_item_description2(running);
  item.set_process_item_current_task_description2(running);
  std::string task_description = item.process_item_current_task_description();
  EXPECT_STREQ(task_description.c_str(), "Running");

  s = item.process_item_status();

  item.StopActivity();
  EXPECT_FALSE(item.running()) << "Process item is not supposed to be running at this point";

  std::string stopped = "Stopped";
  item.set_process_item_description2(stopped);
  std::string status           = item.process_item_status();
  std::string item_description = item.process_item_description2();

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
