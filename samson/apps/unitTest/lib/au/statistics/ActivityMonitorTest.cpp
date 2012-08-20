#include <fcntl.h>

#include "au/statistics/ActivityMonitor.h"
#include "gtest/gtest.h"
#include "unitTest/test_common.h"

TEST(au_statistics_ActivityMonitor, basic) {
  au::statistics::ActivityMonitor activity_monitor;

  EXPECT_EQ("init_activity", activity_monitor.GetCurrentActivity());

  activity_monitor.StartActivity("do_something");
  EXPECT_EQ("do_something", activity_monitor.GetCurrentActivity());

  EXPECT_EQ(true, activity_monitor.str_last_items().length() > 0);
  EXPECT_EQ(true, activity_monitor.str_elements().length() > 0);
}
