
#include "gtest/gtest.h"
#include "engine/Notification.h"
#include "engine/NotificationElement.h"

TEST(engine_NotificationElement, engine_idTest) {
  
  engine::Notification* notification = new engine::Notification("notification_name");
  engine::NotificationElement notification_element(notification);
  
}