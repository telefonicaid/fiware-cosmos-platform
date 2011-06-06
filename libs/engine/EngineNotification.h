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

namespace engine
{
    
    /**
     Base class for all the objects that goes in a notification
     */
    
    class Object
    {
    public:        
        virtual ~Object(){};      // Force virtual destrutor for correct release of memory at the end of the notification
    };
    
        
    /**
     Main class for Notifications
     */
    
    class Notification
    {
        const char* name;                       // Name of the notification
        Object* object;                         // Single object to be used as parameter
        
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
        // Set of notifications we are currently listening
        std::set< const char* , au::strCompare > notification_names;
        
    public:
        
        ~NotificationListener();
        
        virtual void notify( Notification* notification ) =0 ;
        virtual bool acceptNotification( Notification* notification )
        {
            // Function to filter what notifications I should receive
            return true;
        }

        // Start listening a particular notification
        void listen( const char* notification_name );
        
        
    };
    
    /**
     Engine delivery
     */
    
    class NotificationListenerSet
    {
        std::set<NotificationListener*> listeners;
        const char* name;   // Name of the notification we are listening
        
    public:
        
        NotificationListenerSet( const char* _name )
        {
            name = _name;
        }
        
        void add( NotificationListener* listener )
        {
            listeners.insert( listener );
        }
        
        void remove( NotificationListener* listener )
        {
            listeners.erase( listener );
        }
        
        void notify( Notification* notification )
        {

            LM_T(LmtEngineNotification, ("Delivering notification [%s] to %d listeners" , notification->getDescription().c_str() , (int) listeners.size() ));
            
            for ( std::set<NotificationListener*>::iterator i = listeners.begin() ; i != listeners.end() ; i++)
            {
                LM_T(LmtEngineNotification, ("Delibering notification [%s] to listener %p " , notification->getDescription().c_str() , *i ));
                
                if ( (*i)->acceptNotification( notification ) )
                    (*i)->notify( notification );
            }

            notification->destroyObjects();
            
        }
        
        int getNumListeners()
        {
            return listeners.size();
        }
        
    };


    class EngineNotificationSystem
    {
        // Map of deliveries per channel
        au::map< const char* , NotificationListenerSet , au::strCompare > listenersSets;
        
    public:
        
        ~EngineNotificationSystem()
        {
            
            // Destroy all the Enginedelivery elements ( delete is called for each one )
            listenersSets.clearMap(); 
            
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
            NotificationListenerSet *set = get( notification->getName() );

            LM_T(LmtEngineNotification, ("Notifying %s ( Current set of listeners %d ) ", notification->getDescription().c_str() , set->getNumListeners() ));
            
            set->notify( notification );
        }
        
        
    private:
        
        NotificationListenerSet* get( const char* name )
        {
            NotificationListenerSet* delivery;

            delivery = listenersSets.findInMap( name );
            
            if( !delivery )
            {
                delivery = new NotificationListenerSet(name);
                listenersSets.insertInMap( name, delivery );
            }
            return delivery;
        }
        
        
    };
    
}

#endif
