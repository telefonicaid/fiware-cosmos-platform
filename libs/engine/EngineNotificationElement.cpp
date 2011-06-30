
#include "engine/Engine.h"                         // engine::Engine
#include "engine/Object.h"             // engine::EngineNotification
#include "engine/Notification.h"                    // engine::Notification

#include "engine/EngineNotificationElement.h"      // Own interface


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
        Engine::send( notification );
    }
    
}
