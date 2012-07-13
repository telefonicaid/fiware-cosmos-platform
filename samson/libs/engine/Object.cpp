

#include "Engine.h"                 // register_object

#include "engine/Notification.h"    // engine::Notification

#include "Object.h"                 // Own interface

NAMESPACE_BEGIN(engine)

size_t Object::getEngineId()
{
    return engine_id;
}

Object::Object()
{
    // No internal engine name
    engine_name = NULL;
    
    // Add myself as an engine_objects receiving my id
    Engine::shared()->register_object( this );
    
    if( engine_id == 0)
        LM_X(1,("Wrong notification listener id"));
}

Object::Object( const char *_engine_name )
{
    
    // Set my engine name internally
    engine_name = _engine_name;
    
    // Add myself as an engine_objects receiving my id and assigning to a name
    Engine::shared()->register_object( this );
    
    if( engine_id == 0)
        LM_X(1,("Wrong notification listener id"));
}

Object::~Object()
{
    // Unregister this object
    Engine::shared()->unregister_object(this);
}

void Object::notify( Notification* notification )
{
    LM_W(("Notification %s not handled since it has not been overwritted method notify" , notification->getDescription().c_str() ));
}

void Object::listen( const char* notification_name )
{
    Engine::shared()->register_object_for_channel( this,  notification_name );
}


NAMESPACE_END

