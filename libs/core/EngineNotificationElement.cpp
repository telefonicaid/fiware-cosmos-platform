
#include "EngineNotificationElement.h"      // Own interface
#include "Engine.h"                         // ss::Engine
#include "EngineNotification.h"             // ss::EngineNotification

namespace ss
{
    EngineNotificationElement::EngineNotificationElement(  EngineNotification * _notification )
    {
        notification = _notification;
    }
    
    void EngineNotificationElement::run()
    {
        Engine::shared()->notificationSystem.notify( notification );
    }
    
}
