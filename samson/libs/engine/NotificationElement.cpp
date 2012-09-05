
#include "engine/Engine.h"               // engine::Engine
#include "engine/Notification.h"       // engine::Notification
#include "engine/NotificationListener.h"  // engine::EngineNotification

#include "engine/NotificationElement.h"      // Own interface


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
  set_description(au::str("%s", notification_->GetDescription().c_str()));
  set_short_description("Not:" + notification_->GetShortDescription());
}

void NotificationElement::run() {
  LM_T(LmtEngineNotification, ("Running notification %s", notification_->GetDescription().c_str()));
  Engine::shared()->Send(notification_);
}
}
