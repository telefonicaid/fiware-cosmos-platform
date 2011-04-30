

#include "engine/EngineNotification.h"  // Own interface


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
    
    
}
