
#include "logMsg/logMsg.h"      // Log system
#include "logMsg/traceLevels.h" // LmtEngine

#include "au/string.h"          // au::Format

#include "Object.h"         // engine::Object

#include "Notification.h"   // Own interface

NAMESPACE_BEGIN(engine)

// Simples constructor
Notification::Notification( const char* _name )
{
    name = _name;
}

// Constructor with one object
Notification::Notification( const char* _name , au::Object * _object )
{
    name = _name;
    object_container.setObject( _object );
}

Notification::Notification( const char* _name , au::Object * _object , size_t _listener_id )
{
    name = _name;
    object_container.setObject( _object );
    
    // Insert this as the first listener to receive this notification
    targets.insert( _listener_id );
}

Notification::Notification( const char* _name , au::Object * _object , std::set<size_t>& _listeners_id )
{
    name = _name;
    object_container.setObject( _object );
    
    // Insert this as the first listener to receive this notification
    targets.insert( _listeners_id.begin() , _listeners_id.end() );
}

Notification::~Notification()
{
}



std::string Notification::getDescription()
{
    std::ostringstream output;
    output << name << " [ Notification " << name << " ";
    output << "Targets: (";
    std::set<size_t>::iterator iterator_listener_id; 
    for ( iterator_listener_id = targets.begin() ; iterator_listener_id != targets.end() ; iterator_listener_id++)
        output  << *iterator_listener_id << " ";
    output << ") ";
    output << environment.getEnvironmentDescription().c_str() << " ]";
    return output.str();
}    

std::string Notification::getShortDescription()
{
    return au::str("[ Not: %s ]" , name );
}

// Extract the object of this notification
au::Object* Notification::getObject()
{
    return object_container.getObject();
}

// Check if there is an object in this notification
bool Notification::containsObject()
{
    return ( object_container.getObject() != NULL );
}

NAMESPACE_END
