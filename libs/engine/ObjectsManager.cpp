

#include "logMsg/logMsg.h"      // LM_M
#include "logMsg/traceLevels.h" // LmtEngineNotification

#include "Notification.h"       // engine::Notification
#include "Object.h"             // engine::Object

#include "ObjectsManager.h"     // Own interface

namespace engine {

    ObjectsManager::ObjectsManager() //: token("engine::ObjectsManager")
    {
        engine_id = 1;            
    }
    
    ObjectsManager::~ObjectsManager()
    {
        // Remove the vectors with ids for each channel
        channels.clearMap();
    }
    
    void ObjectsManager::add( Object* o )
    {
        //au::TokenTaker tt( &token );
        
        // Give a global id to this object    
        o->engine_id = engine_id++;
        
        // Try to register with the provided name if possible
        if ( o->engine_name )
        {
            if( objects_by_name.extractFromMap(o->engine_name) == NULL )
                objects_by_name.insertInMap(o->engine_name, o);
            else
                LM_W(("Not possible to register object with name %s. It was previously used..." , o->engine_name));
        }

        // Adding object to the general map by id
        objects.insertInMap( o->engine_id , o );
        
        LM_T(LmtEngineNotification, ("Add object %p for id %lu", o , o->engine_id ));
        
    }
    
    void ObjectsManager::remove( Object* o )
    {
        //au::TokenTaker tt( &token );
        
        LM_T(LmtEngineNotification, ("Remove completelly object %lu", o  , o->engine_id  ));
        objects.extractFromMap( o->engine_id );

        // Remove from the vector of objects by name
        if( o->engine_name )
            objects_by_name.extractFromMap(o->engine_name);
        
        // Remove from all constans listeners
        au::map< const char* , IdsCollection , au::strCompare >::iterator c;
        for ( c = channels.begin() ; c != channels.end() ; c++)
            c->second->remove( o->engine_id );
    }
    
    void ObjectsManager::add( Object* o , const char* name )
    {
        //au::TokenTaker tt( &token );
        
        LM_T(LmtEngineNotification, ("Add object %lu to channel %s", o->engine_id , name ));
        get(name)->add( o->engine_id );
    }
    
    void ObjectsManager::remove(Object* o , const char* name )
    {
        //au::TokenTaker tt( &token );
        
        LM_T(LmtEngineNotification, ("Remove object %lu to channel %s", o->engine_id , name ));
        get(name)->remove( o->engine_id );
    }
        
    // Get the collections of ids for a particular channel
    IdsCollection* ObjectsManager::get( const char* name )
    {
        IdsCollection* delivery;
        
        delivery = channels.findInMap( name );
        
        if( !delivery )
        {
            delivery = new IdsCollection();
            channels.insertInMap( name, delivery );
        }
        return delivery;
    }
    
    
    void ObjectsManager::send( Notification* notification )
    {
                
        // Send to each listener contained in listner_id in Notification
        std::set<size_t>::iterator t;
        for( t = notification->targets.begin() ; t != notification->targets.end() ; t++ )
            send( notification , *t );
        
        // Get the channel with all the ids listening this particular channel
        IdsCollection *channel = get( notification->getName() );

        // Notify to all the objects contained here
        for ( t = channel->ids.begin() ; t != channel->ids.end() ; t++ )
        {
            if( notification->targets.find( *t ) != notification->targets.end() )
                LM_W(("Notifying twice for listen and direct %d", notification->getDescription().c_str() ));
            
            send( notification , *t);
        }
        
    }

    // Run a notification in an object
    void ObjectsManager::send( Notification *notification, size_t target)
    {
        Object* o=NULL;
        {
            //au::TokenTaker tt( &token );
            o = objects.findInMap( target );
        }
        
        if( o )
            o->notify( notification );
    }

    // Get the object registered with this name
    Object* ObjectsManager::getObjectByName( const char *name )
    {
        return objects_by_name.findInMap(name);
    }
    
    
    
}