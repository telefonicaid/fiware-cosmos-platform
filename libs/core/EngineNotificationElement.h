#ifndef _H_ENGINE_NOTIFICATION_ELEMENT
#define  _H_ENGINE_NOTIFICATION_ELEMENT

#include "EngineElement.h"

namespace ss
{
    class EngineNotification;
    
    /*
     Class to send a notification using the Engine itself
     */
    
    class EngineNotificationElement : public EngineElement
    {
        EngineNotification * notification;
        
    public:
        
        ~EngineNotificationElement();
        
        EngineNotificationElement(  EngineNotification * _notification );
        EngineNotificationElement(  EngineNotification * _notification , int seconds );
        void run();
    };
    
}

#endif
