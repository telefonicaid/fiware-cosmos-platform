#ifndef _H_ENGINE_NOTIFICATION_ELEMENT
#define  _H_ENGINE_NOTIFICATION_ELEMENT

#include "EngineElement.h"

namespace engine
{
    class Notification;
    
    /*
     Class to send a notification using the Engine itself
     */
    
    class NotificationElement : public EngineElement
    {
        Notification * notification;
        
    public:
        
        ~NotificationElement();
        
        NotificationElement(  Notification * _notification );
        NotificationElement(  Notification * _notification , int seconds );
        void run();
    };
    
}

#endif
