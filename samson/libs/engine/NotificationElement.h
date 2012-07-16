/* ****************************************************************************
 *
 * FILE            EngineNotificationElement
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * EngineElement that encapsulated the delivery of a notification
 *
 * ****************************************************************************/

#ifndef _H_ENGINE_NOTIFICATION_ELEMENT
#define  _H_ENGINE_NOTIFICATION_ELEMENT

#include "au/namespace.h"

#include "engine/EngineElement.h"   // engine::EngineElement

NAMESPACE_BEGIN(engine)

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

NAMESPACE_END

#endif
