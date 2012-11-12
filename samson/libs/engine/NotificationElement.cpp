/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#include "engine/Engine.h"                         // engine::Engine
#include "engine/Object.h"             // engine::EngineNotification
#include "engine/Notification.h"                    // engine::Notification

#include "engine/NotificationElement.h"      // Own interface


NAMESPACE_BEGIN(engine)

NotificationElement::NotificationElement(  Notification * _notification ) 
: EngineElement( au::str("notification_%s" , _notification->getName()) )
{
    notification      = _notification;
    description       =  notification->getDescription();
    shortDescription  =  notification->getShortDescription();
}

NotificationElement::~NotificationElement()
{
    delete notification;
}


NotificationElement::NotificationElement(  Notification * _notification , int seconds ) 
: EngineElement( au::str("notification_%s_repeated_%d" , _notification->getName()  , seconds ) , seconds )
{
    notification = _notification;
    description = au::str("%s", notification->getDescription().c_str() );
    shortDescription = "Not:" + notification->getShortDescription();
}

void NotificationElement::run()
{
    LM_T(LmtEngineNotification, ("Running notification %s", notification->getDescription().c_str() ));
    Engine::shared()->send( notification );
}

NAMESPACE_END
