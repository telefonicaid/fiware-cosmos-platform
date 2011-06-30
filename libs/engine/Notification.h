#ifndef _H_ENGINE_NOTIFICATION
#define _H_ENGINE_NOTIFICATION

#include <set>                  // std::set

#include "au/Environment.h"     // au::Enrivonment

namespace engine
{
    
    class Object;
    
    
    /**
     Main class for Notifications
     */
    
    class Notification
    {
        friend class ObjectsManager;
        
        const char* name;                       // Name of the notification
        Object* object;                         // Single object to be used as parameter
        
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
        Notification( const char* _name , Object * _object );
        
        // Constructor with one object and a target listener
        Notification( const char* _name , Object * _object , size_t _listener_id );
        
        // Get a string for debug
        std::string getDescription();
        std::string getShortDescription();
        
        // Destroy pending objects ( not processed by any listener )
        void destroyObjects();
        
        // Extract the object of this notification
        Object* extractObject();
        
        // Check if there is an object in this notification
        bool containsObject();
        
    };
    
}

#endif
