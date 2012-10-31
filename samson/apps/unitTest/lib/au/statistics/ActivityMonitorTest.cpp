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
#include <fcntl.h>

#include "au/statistics/ActivityMonitor.h"
#include "gtest/gtest.h"
#include "unitTest/TestClasses.h"

TEST(au_statistics_ActivityMonitor, basic) {
  au::statistics::ActivityMonitor activity_monitor;

  EXPECT_EQ("init_activity", activity_monitor.GetCurrentActivity());

  activity_monitor.StartActivity("do_something");
  EXPECT_EQ("do_something", activity_monitor.GetCurrentActivity());

  EXPECT_EQ(true, activity_monitor.GetElementsTable().length() > 0);
  EXPECT_EQ(true, activity_monitor.GetLastItemsTable().length() > 0);
}
