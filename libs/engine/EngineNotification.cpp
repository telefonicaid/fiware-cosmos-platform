

#include "engine/EngineNotification.h"  // Own interface
#include "engine/Engine.h"              // engine::Engine

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
        listener_id.insert( _listener_id );
    }
    
    
    std::string Notification::getDescription()
    {
      return au::Format::string("[ Notification %s %s ]" , name,  environment.getEnvironmentDescription().c_str() );
    }    
    
  std::string Notification::getShortDescription()
    {
      return au::Format::string("[ Not: %s]" , name );
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
    }
    
    
    #pragma mark NotificationListener
    
    NotificationListener::NotificationListener()
    {
        // Add myself as a listener ( giving to me a unique identifier )
        Engine::add( this );
    }
    
    void NotificationListener::listen( const char* notification_name )
    {
        if( notification_names.find(notification_name) == notification_names.end() )
        {
            // Att this listener for a particular notification channel
            Engine::add( notification_name , this );

            // Add to the list of active notifications
            notification_names.insert( notification_name );
        }
    }

    NotificationListener::~NotificationListener()
    {
        std::set< const char* , au::strCompare >::iterator iter;
        for ( iter = notification_names.begin() ; iter != notification_names.end() ; iter++)
        {
            const char *notification_name = *iter;
            Engine::remove(notification_name, this);
        }
    }

    
}
