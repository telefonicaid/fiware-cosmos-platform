#ifndef _H_ENGINE_NOTIFICATION
#define _H_ENGINE_NOTIFICATION

#include "au/Environment.h"                // ss::Environment
#include <set>                          // std::set
#include "au/map.h"                     // au::map
#include "au/Format.h"                     // au::Format
#include <assert.h>
#include "traceLevels.h"                // LmtEngine

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
    
    class Notification
    {
        std::string name;                       // Name of the notification
        
    public:

        Object* object;                         // Single object to be used as parameter
        au::Environment environment;            // Dictionary of properties for maximum flexibility
        
        
        std::string getName()
        {
            return name;
        }
        
        bool isName( std::string _name)
        {
            return (name == _name);
        }
        
        // Simples constructor
        Notification( std::string _name );
        
        // Constructor with one object as "main"
        Notification( std::string _name , Object * _object );
        
        // Get a string for debug
        std::string getDescription();
        std::string getShortDescription();
        
        // Destroy pending objects ( not processed by any listener )
        void destroyObjects();
        
    };
    
    /**
     Notification listener
     */
    
    class NotificationListener
    {
    public:
        
        virtual void notify( Notification* notification ) =0 ;
        virtual bool acceptNotification( Notification* notification )
        {
            // Function to filter what notifications I should receive
            return true;
        }
        
        // Callback received when a notification is not received by anyone
        virtual void calcelNotification( Notification * notification)
        {
            
        }
        
    };
    
    /**
     Engine delivery
     */
    
    class NotificationListenerSet
    {
        std::set<NotificationListener*> listeners;
        std::string name;
        
    public:
        
        NotificationListenerSet( std::string _name )
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
        
    };


    class EngineNotificationSystem
    {
        // Map of deliveries per channel
        au::map<std::string,NotificationListenerSet> listenersSets;
        
    public:
        
        ~EngineNotificationSystem()
        {
            // Destroy all the Enginedelivery elements ( delete is called for each one )
            listenersSets.clearMap();
        }
        
        void add( std::string name , NotificationListener* listener )
        {
            get(name)->add( listener );
        }
        
        void remove( std::string name , NotificationListener* listener )
        {
            get(name)->remove( listener );
        }

        // Remove this listener from all
        void remove( NotificationListener* listener )
        {
            LM_T(LmtEngineNotification , ("Engine removing general listener %p", listener ));
            
            for( au::map<std::string,NotificationListenerSet>::iterator d = listenersSets.begin() ; d != listenersSets.end() ; d++ )
                d->second->remove( listener );
        }
        
        void notify( Notification* notification )
        {
            get( notification->getName() )->notify( notification );
        }
        
        
    private:
        
        NotificationListenerSet* get( std::string name )
        {
            NotificationListenerSet* delivery = listenersSets.findInMap( name );
            if( !delivery )
            {
                delivery = new NotificationListenerSet(name);
                listenersSets.insertInMap(name, delivery);
            }
            return delivery;
        }
        
        
    };
    
}

#endif
