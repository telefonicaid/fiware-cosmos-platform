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

#include "engine/Notification.h"
#include "engine/NotificationElement.h"
#include "gtest/gtest.h"

TEST(engine_NotificationElement, engine_idTest) {
  engine::Notification *notification = new engine::Notification("notification_name");
  engine::NotificationElement notification_element(notification);
}