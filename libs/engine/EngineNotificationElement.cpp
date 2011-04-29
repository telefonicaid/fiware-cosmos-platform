
#include "engine/EngineNotificationElement.h"      // Own interface
#include "engine/Engine.h"                         // engine::Engine
#include "engine/EngineNotification.h"             // engine::EngineNotification

namespace engine
{
    NotificationElement::NotificationElement(  Notification * _notification )
    {
        notification = _notification;
        description = au::Format::string("Notification %s", notification->getDescription().c_str() );
        shortDescription = "Not:" + notification->getShortDescription();
    }
 
    NotificationElement::~NotificationElement()
    {
        delete notification;
    }
    
    
    NotificationElement::NotificationElement(  Notification * _notification , int seconds ) : EngineElement( seconds )
    {
        notification = _notification;
        description = au::Format::string("Notification %s", notification->getDescription().c_str() );
        shortDescription = "Not:" + notification->getShortDescription();
    }
    
    void NotificationElement::run()
    {
        LM_T(LmtEngineNotification, ("Running notification %s", notification->getDescription().c_str() ));
        Engine::_notify( notification );
    }
    
}
