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
/* ****************************************************************************
 *
 * FILE            Notification.h
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * Notification message in the engine library
 * It is basically an "object-C" message kind of thing
 *
 *
 * --> A notification is a "message" sent in the main-thread runloop to a set of objects ( targets )
 * --> The notification is also delivered to all the objects that "listen" 
 *     the notification chanel ( defined by notification's name)
 * --> Internall information is contained in environemnt variable and the optional object parameter
 *
 * ****************************************************************************/

#ifndef _H_ENGINE_NOTIFICATION
#define _H_ENGINE_NOTIFICATION

#include <set>                  // std::set

#include "au/Environment.h"     // au::Enrivonment
#include "au/namespace.h"
#include "au/Object.h"

#include <string.h>

NAMESPACE_BEGIN(engine)

class Object;


/**
 Main class for Notifications
 */

class Notification
{
    friend class ObjectsManager;
    
    const char* name;                       // Name of the notification
    
    // Single object to be used as parameter ( note retain / release model associated to au::Object )
    au::ObjectContainer<au::Object> object_container;   
    
    std::set<size_t> targets;               // Identifiers that should receive this notification
    
public:
    
    au::Environment environment;            // Dictionary of properties for maximum flexibility
    
    const char* getName()
    {
        return name;
    }
    
    bool isName( const char * _name )
    {
        return strcmp( name , _name ) == 0;
    }
    
    // Simples constructor
    Notification( const char* _name );
    
    // Constructor with one object as "main"
    Notification( const char* _name , au::Object * _object );
    
    // Constructor with one object and a target listener
    Notification( const char* _name , au::Object * _object , size_t _listener_id );
    
    // Constructor with one object and a multiple targets listener
    Notification( const char* _name , au::Object * _object , std::set<size_t>& _listeners_id );
    
    // Destructor
    ~Notification();
    
    // Get a string for debug
    std::string getDescription();
    std::string getShortDescription();
    
    // Extract the object of this notification
    au::Object* getObject();
    
    // Check if there is an object in this notification
    bool containsObject();
    
};

NAMESPACE_END

#endif
