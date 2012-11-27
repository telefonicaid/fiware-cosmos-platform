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


#include "NotificationListener.h"   // Own interface

#include "au/log/LogMain.h"
#include "engine/Engine.h"                 // register_object
#include "engine/Logs.h"
#include "engine/Notification.h"    // engine::Notification

namespace engine {
size_t NotificationListener::engine_id() {
  return engine_id_;
}

NotificationListener::NotificationListener() {
  // Add myself as an engine_objects receiving my id
  if (engine::Engine::IsEngineWorking()) {
    Engine::shared()->AddListener(this);
  } else {
    LOG_W(logs.engine, ("Created a notification listener while engine is not working"));
    engine_id_ = 0;
  }
}

NotificationListener::~NotificationListener() {
  // Unregister this object if engine is still working
  if (engine::Engine::IsEngineWorking()) {
    Engine::shared()->RemoveListener(this);
  }
}

void NotificationListener::notify(Notification *notification) {
  LOG_SW(("Notification %s not handled since it has not been overwritted method notify",
          notification->GetDescription().c_str()));
}

void NotificationListener::listen(const char *notification_name) {
  Engine::shared()->AddListenerToChannel(this,  notification_name);
}
}

