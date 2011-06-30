

#include "Engine.h"         // register_object

#include "engine/Notification.h"   // engine::Notification

#include "Object.h"     // Own interface

namespace engine
{
    
    size_t Object::getEngineId()
    {
        return engine_id;
    }

    Object::Object()
    {
        // Add myself as an engine_objects receiving my id
        Engine::register_object( this );
        
        if( engine_id == 0)
            LM_X(1,("Wrong notification listener id"));
    }
    
    Object::~Object()
    {
        // Unregister this object
        Engine::unregister_object(this);
    }

    void Object::notify( Notification* notification )
    {
        LM_W(("Notification %s not handled since it has not been overwritted method notify" , notification->getDescription().c_str() ));
    }
    
    
    void Object::listen( const char* notification_name )
    {
        Engine::register_object_for_channel( this,  notification_name );
    }
    

    
}

