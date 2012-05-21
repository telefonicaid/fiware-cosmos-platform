

#include "au/ThreadManager.h"

#include "Service.h" // Own interface



namespace au
{
    namespace network
    {

        void* run_service_item( void*p )
        {
            ServiceItem* item = (ServiceItem*)p;
            
            // Main function in Service to define what to do with new connections
            item->service->run( item->socket_connection , &item->quit );
            
            // Notify finish of this thread
            item->service->finish(item);

            // Set this flag to false to inform Service that this thread is not runnign any more
            item->thread_running = false;
            
            return NULL;
        }
        
        
        void ServiceItem::stop()
        {
            socket_connection->close(); // Close connection
            quit = true;
        }
        
        void ServiceItem::runInBackground()
        {
            thread_running = true; // Set this flag to inform about this thread running...
            
            pthread_t t;
            ThreadManager::shared()->addThread("ServiceItem", &t, NULL, run_service_item, this);
        }
        
    }
}