

#include "Engine.h"                 // register_object
#include "engine/Notification.h"    // engine::Notification

#include "NotificationListener.h"                 // Own interface

namespace engine {
size_t NotificationListener::engine_id() {
  return engine_id_;
}

NotificationListener::NotificationListener() {

  // Add myself as an engine_objects receiving my id
  Engine::shared()->AddListener(this);

  if (engine_id_ == 0) {
    LM_X(1, ("Wrong notification listener id"));
  }
}


NotificationListener::~NotificationListener() {
  // Unregister this object
  Engine::shared()->RemoveListener(this);
}

void NotificationListener::notify(Notification *notification) {
  LM_W(("Notification %s not handled since it has not been overwritted method notify",
        notification->GetDescription().c_str()));
}

void NotificationListener::listen(const char *notification_name) {
  Engine::shared()->AddListenerToChannel(this,  notification_name);
}
}

