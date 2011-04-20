

#include "EngineNotification.h"


namespace engine {
    
    
    // Simples constructor
    Notification::Notification( std::string _name )
    {
        name = _name;
        object = NULL;
    }
    
    // Constructor with one object
    Notification::Notification( std::string _name , Object * _object )
    {
        name = _name;
        object = _object;
    }
    
    std::string Notification::getDescription()
    {
      return au::Format::string("[ Notification %s %s ]" , name.c_str(),  environment.getEnvironmentDescription().c_str() );
    }    
    
  std::string Notification::getShortDescription()
    {
      return au::Format::string("[ Not: %s]" , name.c_str() );
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

}
