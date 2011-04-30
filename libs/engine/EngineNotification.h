#ifndef _H_ENGINE_NOTIFICATION
#define _H_ENGINE_NOTIFICATION

#include <assert.h>
#include <set>                          // std::set

#include "au/Environment.h"             // ss::Environment
#include "au/map.h"                     // au::map
#include "au/Format.h"                  // au::Format

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
    
    
    /** 
     Function to use when creating a map with const char* type for keys
     */
    
    struct strCompare : public std::binary_function<const char*, const char*, bool> {
    public:
        bool operator() (const char* str1, const char* str2) const
        { return std::strcmp(str1, str2) < 0; }
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
    public:
        
        virtual void notify( Notification* notification ) =0 ;
        virtual bool acceptNotification( Notification* notification )
        {
            // Function to filter what notifications I should receive
            return true;
        }
        
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
        
    };


    class EngineNotificationSystem
    {
        // Map of deliveries per channel
        std::map< const char* , NotificationListenerSet* , strCompare > listenersSets;
        
    public:
        
        ~EngineNotificationSystem()
        {
            
            // Destroy all the Enginedelivery elements ( delete is called for each one )
            
            std::map< const char* , NotificationListenerSet* , strCompare >::iterator iter;
			for (iter = listenersSets.begin() ; iter != listenersSets.end() ; iter++)
				delete iter->second;
            listenersSets.clear();
            
        }
        
        void add( const char* name , NotificationListener* listener )
        {
            get(name)->add( listener );
        }
        
        void remove( const char* name , NotificationListener* listener )
        {
            get(name)->remove( listener );
        }

        // Remove this listener from all
        void remove( NotificationListener* listener )
        {
            LM_T(LmtEngineNotification , ("Engine removing general listener %p", listener ));
            
            std::map< const char* , NotificationListenerSet* , strCompare >::iterator d;
            
            for( d = listenersSets.begin() ; d != listenersSets.end() ; d++ )
                d->second->remove( listener );
        }
        
        void notify( Notification* notification )
        {
            get( notification->getName() )->notify( notification );
        }
        
        
    private:
        
        NotificationListenerSet* get( const char* name )
        {
            NotificationListenerSet* delivery;
            
            std::map< const char* , NotificationListenerSet* , strCompare >::iterator d;
            d = listenersSets.find( name );
            
            if( d == listenersSets.end() )
                delivery = NULL;
            else
                delivery = d->second;
            
            if( !delivery )
            {
                delivery = new NotificationListenerSet(name);
                listenersSets.insert( std::pair< const char* , NotificationListenerSet*>(name, delivery) );
            }
            return delivery;
        }
        
        
    };
    
}

#endif
