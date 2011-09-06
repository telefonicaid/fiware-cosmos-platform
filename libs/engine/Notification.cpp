
#include "logMsg/logMsg.h"      // Log system
#include "logMsg/traceLevels.h" // LmtEngine

#include "au/string.h"          // au::Format

#include "Object.h"         // engine::Object

#include "Notification.h"   // Own interface

namespace engine {
    // Simples constructor
    Notification::Notification( const char* _name )
    {
        name = _name;
        object = NULL;
    }
    
    // Constructor with one object
    Notification::Notification( const char* _name , Object * _object )
    {
        name = _name;
        object = _object;
    }
    
    Notification::Notification( const char* _name , Object * _object , size_t _listener_id )
    {
        name = _name;
        object = _object;
        
        // Insert this as the first listener to receive this notification
        targets.insert( _listener_id );
    }

    Notification::Notification( const char* _name , Object * _object , std::set<size_t>& _listeners_id )
    {
        name = _name;
        object = _object;

        // Insert this as the first listener to receive this notification
        targets.insert( _listeners_id.begin() , _listeners_id.end() );
    }
    
    
    
    std::string Notification::getDescription()
    {
        std::ostringstream output;
        
        output << "[ Notification " << name << " ";
        
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
        return au::str("[ Not: %s]" , name );
    }
    
    
    void Notification::destroyObjects()
    {
        
        if( object )
        {
            LM_T(LmtEngine, ("Destroying object of a notification [%s]", getDescription().c_str() ));
            delete object;
            object = NULL;
        }
    }
    
    // Extract the object of this notification
    Object* Notification::extractObject()
    {
        Object *tmp = object;
        object = NULL;
        return tmp;
    }
    
    // Check if there is an object in this notification
    bool Notification::containsObject()
    {
        return ( object != NULL );
    }}