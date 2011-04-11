#ifndef _H_ENGINE_NOTIFICATION
#define _H_ENGINE_NOTIFICATION

#include "samson/Environment.h"         // ss::Environment
#include <set>                          // std::set
#include "au_map.h"                     // au::map
#include <assert.h>
#include "Format.h"                     // au::Format
#include "traceLevels.h"                // LmtEngine

namespace ss
{
    
    /**
     Notification
     */
    
    typedef enum 
    {
        notification_nothing,                            // Notification used for nerver-ending loop of the engine
        
        notification_memory_request,                     // Request of memory        
        notification_memory_request_response,            // Response of a memory request

        notification_disk_operation_request,             // Request a disk operation
        notification_disk_operation_request_response,    // Response for a disk operation request
        
        notification_process_request,                    // Process request
        notification_process_request_response,           // Response of the process request
    
        notification_sub_task_finished,                  // Notification that a particular sub task has finished
    
        notification_task_finished,                      // Notification that a partitular task is finished ( only the Worker Task Manager should be interestes in this )

        notification_worker_update_files,                // Periodic notification to update files 
        
        notification_monitorization,                     // Take a sample for monitorization 
        notification_check_automatic_operations,         // Check for automatic operations   
        
    } NotificationChannel;
    
    /**
     Base class for all the objects that goes in a notification
     */
    
    class EngineNotificationObject : public Environment
    {
        public:        
            virtual ~EngineNotificationObject(){};      // Force virtual destrutor for correct release of memory
    };
    
    class EngineNotification : public Environment
    {
        
    public:
        
        NotificationChannel channel;
        std::vector<EngineNotificationObject*> object;         // Vector of objects to be used as parameters        

        // Simples constructor
        EngineNotification( NotificationChannel _channel );
        
        // Constructor with one object
        EngineNotification( NotificationChannel _channel , EngineNotificationObject * _object );
        
        // Get a string for debug
        std::string getDescription();
        std::string getShortDescription();
        
        // Destroy pending objects ( not processed by any listener )
        void destroyObjects();
        
        // Name of the notification channel
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
            listeners.insert( listener );
        }
        
        void remove( EngineNotificationListener* listener )
        {
            listeners.erase( listener );
        }
        
        void notify( EngineNotification* notification )
        {

            LM_T(LmtEngine, ("Delivering notification [%s] to %d listeners " , notification->getDescription().c_str() , (int) listeners.size() ));
            
            for ( std::set<EngineNotificationListener*>::iterator i = listeners.begin() ; i != listeners.end() ; i++)
            {
                LM_T(LmtEngine, ("Delibering notification [%s] to %p " , notification->getDescription().c_str() , *i ));
                
                if ( (*i)->acceptNotification( notification ) )
                    (*i)->notify( notification );
            }

            LM_T(LmtEngine, ("Destroying %d pending objects in the notification [%s]" 
                             , (int) notification->object.size() 
                             , notification->getDescription().c_str() ));
            
            // Destroy the rest of objects that are still inside the notification
            notification->destroyObjects();
            
            LM_T(LmtEngine, ("Finish Delibering notification [%s] to %d listeners " , notification->getDescription().c_str() , (int) listeners.size() ));
            
        }
        
    };
    
    class EngineNotificationSystem
    {
        au::map<NotificationChannel,EngineDelivery> deliveries;
        
    public:
        
        ~EngineNotificationSystem()
        {
            // Destroy all the Enginedelivery elements ( delete is called for each one )
            deliveries.clearMap();
        }
        
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
            LM_T(LmtEngine , ("Engine removing general listener %p", listener ));
            
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
