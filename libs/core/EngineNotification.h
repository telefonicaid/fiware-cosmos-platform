#ifndef _H_ENGINE_NOTIFICATION
#define _H_ENGINE_NOTIFICATION

#include "samson/Environment.h"         // ss::Environment
#include <set>                          // std::set
#include "au_map.h"                     // au::map
#include <assert.h>
#include "Format.h"                     // au::Format

namespace ss
{
    
    /**
     Notification
     */
    
    typedef enum 
    {
        
        notification_memory_request,                     // Request of memory        
        notification_memory_request_response,            // Response of a memory request

        notification_disk_operation_request,             // Request a disk operation
        notification_disk_operation_request_response,    // Response for a disk operation request
        
        notification_process_request,                    // Process request
        notification_process_request_response,           // Response of the process request
    
        notification_sub_task_finished,                  // Notification that a particular sub task has finished
    
        notification_task_finished,                      // Notification that a partitular task is finished ( only the Worker Task Manager should be interestes in this )
        
    } NotificationChannel;
    
    /**
     Base class for all the objects that goes in a notification
     */
    
    class EngineNotificationObject : public Environment
    {
        public:        
            virtual void destroy(){};
            virtual ~EngineNotificationObject(){};      // Force virtual destrutor for correct release of memory
    };
    
    class EngineNotification : public Environment
    {
        
    public:
        
        NotificationChannel channel;
        std::vector<EngineNotificationObject*> object;                          // Vector of objects to be used as parameters        

        // Simples constructor
        EngineNotification( NotificationChannel _channel );
        
        // Constructor with one object
        EngineNotification( NotificationChannel _channel , EngineNotificationObject * _object );
        
        // Get a string for debug
        std::string getDescription();
      
        const char * notificationChannelName();
        
    };
    
    
    /**
     Notification listener
     */
    
    class EngineNotificationListener
    {
    public:
        
        virtual void notify( EngineNotification* notification )=0;
        virtual bool acceptNotification( EngineNotification* notification )
        {
            // Function to filter the notification I receive
            return true;
        }
    };
    
    /**
     Engine delivery
     */
    
    class EngineDelivery
    {
        std::set<EngineNotificationListener*> listeners;
        int channel;
        
    public:
        
        EngineDelivery( NotificationChannel _channel )
        {
            channel = _channel;
        }
        
        void add( EngineNotificationListener* listener )
        {
            listeners.insert( listener);
        }
        
        void remove( EngineNotificationListener* listener )
        {
            listeners.erase( listener);
        }
        
        void notify( EngineNotification* notification )
        {
            if( listeners.size() == 0)
            {
                // Delete objects inside the notification ( otherwise we will have memory leaks )
                for ( size_t i = 0 ; i < notification->object.size() ; i++)
                {
                    notification->object[i]->destroy();
                    delete notification->object[i];
                }
                notification->object.clear();
                
            }
            else
            {
                for ( std::set<EngineNotificationListener*>::iterator i = listeners.begin() ; i != listeners.end() ; i++)
                    if ( (*i)->acceptNotification( notification ) )
                        (*i)->notify( notification );
            }
            
            
            delete notification;
        }
        
    };
    
    class EngineNotificationSystem
    {
        au::map<NotificationChannel,EngineDelivery> deliveries;
        
    public:
        
        void add( NotificationChannel channel , EngineNotificationListener* listener )
        {
            get(channel)->add( listener );
        }
        
        void remove( NotificationChannel channel , EngineNotificationListener* listener )
        {
            get(channel)->remove( listener );
        }

        // Remove this listener from all
        void remove( EngineNotificationListener* listener )
        {
            for( au::map<NotificationChannel,EngineDelivery>::iterator d = deliveries.begin() ; d != deliveries.end() ; d++ )
                d->second->remove( listener );
        }
        
        void notify( EngineNotification* notification )
        {
            get( notification->channel )->notify( notification );
        }
        
        
    private:
        
        EngineDelivery* get( NotificationChannel channel )
        {
            EngineDelivery* delivery = deliveries.findInMap( channel );
            if( !delivery )
            {
                delivery = new EngineDelivery(channel);
                deliveries.insertInMap(channel, delivery);
            }
            return delivery;
        }
        
        
    };
    
}

#endif
