

#include "au/ThreadManager.h"

#include "Service.h" // Own interface



namespace au
{
    namespace network
    {

        void* run_service_item( void*p )
        {
            ServiceItem* item = (ServiceItem*)p;
            item->service->run( item->socket_connection , &item->quit );

            // Close the socket if not already closed
            item->socket_connection->close();
            
            // Notify finish of this thread
            item->service->finish(item);
            
            return NULL;
        }
        
        
        void ServiceItem::runInBackground()
        {
            pthread_t t;
            ThreadManager::shared()->addThread("ServiceItem", &t, NULL, run_service_item, this);
        }

        
    }
}