#ifndef _H_ENGINE_NOTIFICATION
#define _H_ENGINE_NOTIFICATION

#include <assert.h>
#include <set>                          // std::set
#include <functional>
#include <cstring>

#include "au/Environment.h"             // samson::Environment
#include "au/map.h"                     // au::map
#include "au/Format.h"                  // au::Format

#include "logMsg/traceLevels.h"                // LmtEngine

#include "engine/Object.h"                  // engine::Object

namespace engine
{
    

    
        
    /**
     Main class for Notifications
     */
    
    class Notification
    {
        friend class EngineNotificationSystem;
        
        const char* name;                       // Name of the notification
        Object* object;                         // Single object to be used as parameter
        
        std::set<size_t> listener_id;           // Identifiers that should receive this notification
        
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
    
    /**
     Notification listener
     */
    
    class NotificationListener
    {
        friend class EngineNotificationSystem;
        
        // Set of notifications we are currently listening
        std::set< const char* , au::strCompare > notification_names;
        
        // Unique identifier of this listener
        size_t listener_id;
        
    public:
        
        NotificationListener();
        
        ~NotificationListener();
        
        virtual void notify( Notification* notification ) =0 ;
        virtual bool acceptNotification( Notification* notification )
        {
            // Function to filter what notifications I should receive
            return true;
        }

        // Start listening a particular notification
        void listen( const char* notification_name );
        
        // Get my id as listener
        size_t getListenerId()
        {
            return listener_id;
        }
        
    };
    
    /**
     Engine delivery
     */
    
    class NotificationListenerSet
    {
        
    public:
        
        std::set<size_t> listener_ids;
        
        
        void add( NotificationListener* listener )
        {
            listener_ids.insert( listener->getListenerId() );
        }
        
        void remove( NotificationListener* listener )
        {
            listener_ids.erase( listener->getListenerId() );
        }
        
    };


    class EngineNotificationSystem
    {
        size_t listener_id;
        // Map of all the listeners by id
        au::map < size_t , NotificationListener > listeners;
        
        // Map of deliveries per channel
        au::map< const char* , NotificationListenerSet , au::strCompare > listenersSets;
        
    public:
        
        ~EngineNotificationSystem()
        {
            
            listener_id = 1;
            
            // Destroy all the Enginedelivery elements ( delete is called for each one )
            listenersSets.clearMap(); 
            
        }
        void add( NotificationListener* listener )
        {
            LM_T(LmtEngineNotification, ("Add listener %p", listener ));
            listener->listener_id = listener_id++;
            
            listeners.insertInMap( listener->listener_id , listener );
        }
        
        void remove( NotificationListener* listener )
        {
            LM_T(LmtEngineNotification, ("Remove listener %p", listener  ));
            listeners.extractFromMap( listener->listener_id );
        }
        
        void add( const char* name , NotificationListener* listener )
        {
            LM_T(LmtEngineNotification, ("Add listener %p to %s", listener , name ));
            get(name)->add( listener );
        }
        
        void remove( const char* name , NotificationListener* listener )
        {
            LM_T(LmtEngineNotification, ("Remove listener %p to %s", listener , name ));
            get(name)->remove( listener );
        }
        
        void notify( Notification* notification )
        {
            
            // Send to each listener contained in listner_id in Notification
            std::set<size_t>::iterator iterator_listener_id;
            for( iterator_listener_id = notification->listener_id.begin() ; iterator_listener_id != notification->listener_id.end() ; iterator_listener_id++ )
            {
                
                size_t _listener_id = *iterator_listener_id;
                NotificationListener* listener = listeners.findInMap( _listener_id );
                if( listener )
                    listener->notify( notification );
            }
            
            // Send to the globally added as listeners
            NotificationListenerSet *set = get( notification->getName() );
            
            LM_T(LmtEngineNotification, ("Delivering notification [%s] to %d listeners" , notification->getDescription().c_str() , (int) listeners.size() ));
            
            for ( std::set<size_t>::iterator i = set->listener_ids.begin() ; i != set->listener_ids.end() ; i++ )
            {
                size_t _listener_id = *i;
                NotificationListener* listener = listeners.findInMap( _listener_id );
                
                if( !listener )
                    LM_X(1, ("Major error in engine. A listener is registered as global listener and is not defined as individual listner"));
                
                if ( listener->acceptNotification( notification ) )
                    listener->notify( notification );
            }
            
            notification->destroyObjects();
            
        }
        
    private:
        
        NotificationListenerSet* get( const char* name )
        {
            NotificationListenerSet* delivery;

            delivery = listenersSets.findInMap( name );
            
            if( !delivery )
            {
                delivery = new NotificationListenerSet();
                listenersSets.insertInMap( name, delivery );
            }
            return delivery;
        }
        
        
    };
    
}

#endif
