

#include "logMsg/logMsg.h"      // LM_M
#include "logMsg/traceLevels.h" // LmtEngineNotification

#include "Notification.h"       // engine::Notification
#include "Object.h"             // engine::Object

#include "ObjectsManager.h"     // Own interface

namespace engine {

    ObjectsManager::ObjectsManager() 
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
        
        o->engine_id = engine_id++;
        LM_T(LmtEngineNotification, ("Add object %p for id %lu", o , o->engine_id ));
        objects.insertInMap( o->engine_id , o );
    }
    
    void ObjectsManager::remove( Object* o )
    {
        
        LM_T(LmtEngineNotification, ("Remove completelly object %lu", o  , o->engine_id  ));
        objects.extractFromMap( o->engine_id );
        
        // Remove from all constans listeners
        au::map< const char* , IdsCollection , au::strCompare >::iterator c;
        for ( c = channels.begin() ; c != channels.end() ; c++)
            c->second->remove( o->engine_id );
    }
    
    void ObjectsManager::add( Object* o , const char* name )
    {
        
        LM_T(LmtEngineNotification, ("Add object %lu to channel %s", o->engine_id , name ));
        get(name)->add( o->engine_id );
    }
    
    void ObjectsManager::remove(Object* o , const char* name )
    {
        
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
        Object* o = objects.findInMap( target );
        if( o )
            o->notify( notification );
    }

    
}