
#include "engine/Engine.h"             // engine::Engine
#include "engine/Notification.h"       // engine::Notification
#include "engine/NotificationListener.h"             // engine::EngineNotification

#include "engine/NotificationElement.h"      // Own interface


namespace engine {
NotificationElement::NotificationElement(Notification *notification)
  : EngineElement(au::str("notification_%s", notification->name())) {
  notification_      = notification;
  description       =  notification_->getDescription();
  shortDescription  =  notification_->getShortDescription();
}

NotificationElement::~NotificationElement() {
  delete notification_;
}

NotificationElement::NotificationElement(Notification *notification, int seconds)
  : EngineElement(au::str("notification_%s_repeated_%d", notification->name(), seconds), seconds) {
  notification_ = notification;
  description = au::str("%s", notification_->getDescription().c_str());
  shortDescription = "Not:" + notification_->getShortDescription();
}

void NotificationElement::run() {
  LM_T(LmtEngineNotification, ("Running notification %s", notification_->getDescription().c_str()));
  Engine::shared()->Send(notification_);
}
}
