
#include "engine/Notification.h"
#include "gtest/gtest.h"

TEST(engine_Notification, engine_idTest) {
  engine::Notification notification("notification_name");

  EXPECT_EQ("notification_name", std::string(notification.name()));
  EXPECT_TRUE(notification.isName("notification_name"));
  EXPECT_FALSE(notification.isName("notification_name2"));

  notification.AddEngineListener(2);
  notification.AddEngineListener(2);
  notification.AddEngineListener(3);

  EXPECT_EQ(2ULL,  notification.targets().size());

  std::set<size_t> additional_targets;
  additional_targets.insert(2);
  additional_targets.insert(4);
  notification.AddEngineListeners(additional_targets);

  EXPECT_EQ(3ULL,  notification.targets().size());

  EXPECT_EQ("notification_name [ Notification notification_name Targets: (2 3 4 ) {} ]", notification.GetDescription());
  EXPECT_EQ("[ Not: notification_name ]", notification.GetShortDescription());
}