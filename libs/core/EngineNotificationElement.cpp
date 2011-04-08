
#include "EngineNotificationElement.h"      // Own interface
#include "Engine.h"                         // ss::Engine
#include "EngineNotification.h"             // ss::EngineNotification

namespace ss
{
    EngineNotificationElement::EngineNotificationElement(  EngineNotification * _notification )
    {
        notification = _notification;
        description = au::Format::string("Notification %s", notification->getDescription().c_str() );

    }
    
    void EngineNotificationElement::run()
    {
        LM_T(LmtEngine, ("Running notification %s", notification->getDescription().c_str() ));
        Engine::shared()->notificationSystem.notify( notification );
    }
    
}
