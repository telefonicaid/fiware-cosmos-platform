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

#include "engine/NotificationElement.h"      // Own interface

#include "engine/Engine.h"               // engine::Engine
#include "engine/Logs.h"
#include "engine/Notification.h"       // engine::Notification
#include "engine/NotificationListener.h"  // engine::EngineNotification



namespace engine {
NotificationElement::NotificationElement(Notification *notification)
  : EngineElement(au::str("notification_%s", notification->name())) {
  notification_      = notification;
  set_description(notification_->GetDescription());
  set_short_description(notification_->GetShortDescription());
}

NotificationElement::~NotificationElement() {
  delete notification_;
}

NotificationElement::NotificationElement(Notification *notification, int seconds)
  : EngineElement(au::str("notification_%s_repeated_%d", notification->name(), seconds), seconds) {
  notification_ = notification;
  set_description(notification_->GetDescription());
  set_short_description(notification_->GetShortDescription());
}

void NotificationElement::run() {
  LOG_M(logs.notifications, ("Running notification %s", notification_->GetDescription().c_str()));
  Engine::shared()->Send(notification_);
}
}
