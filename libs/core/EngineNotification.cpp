

#include "EngineNotification.h"


namespace ss {
    
    
    // Simples constructor
    EngineNotification::EngineNotification( NotificationChannel _channel )
    {
        channel = _channel;

    }
    
    // Constructor with one object
    EngineNotification::EngineNotification( NotificationChannel _channel , EngineNotificationObject * _object )
    {
        channel = _channel;
        object.push_back(_object );
    }
    
    std::string EngineNotification::getDescription()
    {
        return au::Format::string("[ Notification %s %s]" , notificationChannelName() , getEnvironmentDescription().c_str() );
    }    
    
    void EngineNotification::destroyObjects()
    {
        // Removing pending objects in the object vector
        for ( size_t i = 0 ; i < object.size() ; i++ )
            delete object[i];
        object.clear();        
    }
    
    const char * EngineNotification::notificationChannelName()
    {
        switch (channel) {
            case notification_memory_request: return "notification_memory_request"; break;
            case notification_memory_request_response: return "notification_memory_request_response"; break;
            case notification_disk_operation_request: return "notification_disk_operation_request"; break;
            case notification_disk_operation_request_response: return "notification_disk_operation_request_response"; break;
            case notification_process_request: return "notification_process_request"; break;
            case notification_process_request_response: return "notification_process_request_response"; break;
            case notification_sub_task_finished: return "notification_sub_task_finished"; break;
            case notification_task_finished: return "notificaiton_task_finished"; break;
        }   
        return "Unknown";
    }


}