
#include "engine/Engine.h"                         // engine::Engine
#include "engine/Object.h"             // engine::EngineNotification
#include "engine/Notification.h"                    // engine::Notification

#include "engine/NotificationElement.h"      // Own interface


NAMESPACE_BEGIN(engine)

NotificationElement::NotificationElement(  Notification * _notification )
{
    notification = _notification;
    description = au::str("Notification %s", notification->getDescription().c_str() );
    shortDescription = "Not:" + notification->getShortDescription();
}

NotificationElement::~NotificationElement()
{
    delete notification;
}


NotificationElement::NotificationElement(  Notification * _notification , int seconds ) : EngineElement( seconds )
{
    notification = _notification;
    description = au::str("Notification %s", notification->getDescription().c_str() );
    shortDescription = "Not:" + notification->getShortDescription();
}

void NotificationElement::run()
{
    LM_T(LmtEngineNotification, ("Running notification %s", notification->getDescription().c_str() ));
    Engine::shared()->send( notification );
}

NAMESPACE_END