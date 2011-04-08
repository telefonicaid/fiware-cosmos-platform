

#include "ProcessManager.h"



#include "logMsg.h"				// LM_X
#include <time.h>
#include <sys/time.h>

#include "ProcessManager.h"                 // ss::Process
#include "Engine.h"							// ss::Engine
#include "EngineElement.h"					// ss::EngineElement
#include "SamsonSetup.h"					// ss::SamsonSetup
#include "ProcessItem.h"					// ss::ProcessItem
#include "DiskOperation.h"					// ss::DiskOperation
#include "EngineNotificationElement.h"      // ss::EngineNotificationElement

namespace ss
{
        
    ProcessManager::ProcessManager()
    {
        pthread_mutex_init(&mutex, 0);			// Mutex to protect elements
        
        // Take the num of process from setup and init the current number of concurrent process
        num_processes = SamsonSetup::shared()->num_processes;
    }
    
    ProcessManager::~ProcessManager()
    {
        pthread_mutex_destroy(&mutex);			// Mutex to protect elements
    }
    
    void ProcessManager::notify( EngineNotification* notification )
    {
        if( notification->channel != notification_process_request )
            LM_X(1,("Wrong notification at ProcessManager"));
        
        if( notification->object.size() != 1 )
            LM_X(1,("ProcessManager received a notification_process_request without an object"));
        
        ProcessItem *item = (ProcessItem*) notification->object[0];
        notification->object.clear();   // Remove from the vector, it would be automatically deleted by engine
        
        item->environment.copyFrom( notification );
        addProcessItem( item );
    }
    
    
    void ProcessManager::addProcessItem( ProcessItem *item )
    {
        LM_T( LmtEngine , ("Adding ProcessItem") );
        
        pthread_mutex_lock(&mutex);
        
        items.insert( item );
        
        LM_T( LmtEngine , ("Engine state for background process: Pending %u Running %u Halted %u", items.size() , running_items.size() , halted_items.size()  ) );
        
        pthread_mutex_unlock(&mutex);
        
        
        // Check background processes to see if it is necessary to run new stuff
        checkBackgroundProcesses();
        
    }
    
    ProcessItem* ProcessManager::_getNextItemToRun()
    {
        
        ProcessItem* item = NULL;
        
        // Halted process comes first 
        
        for (std::set<ProcessItem*>::iterator i = halted_items.begin() ; i != halted_items.end() ; i++ )
        {
            if( (*i)->isReady() )
            {
                item = *i;
                halted_items.erase( item );
                return item;
            }
        }
        
        // If not process in the halt list is ready,
        // we get the highest priority element in the queue of pending processes
        
        for ( std::set<ProcessItem*>::iterator i =  items.begin() ; i!= items.end() ; i++)
        {
            if( (*i)->isReady() )   // Let's check if the process is ready to be executed
            {
                if( !item  )
                    item = *i;
                else
                {
                    if( (*i)->priority > item->priority )
                        item = *i;
                }
            }
        }
        
        if( item )
            items.erase( item );	// Remove form the pending list
        
        // It is null if no process is required to be executed
        return item;
        
    }	
    
    void ProcessManager::finishProcessItem( ProcessItem *item )
    {
        pthread_mutex_lock(&mutex);
        running_items.erase(item);
        pthread_mutex_unlock(&mutex);
        
        // Notify this using the notification
        EngineNotification * notification = new EngineNotification( notification_process_request_response , item );
        notification->copyFrom( &item->environment );
        Engine::shared()->notify( notification );
        
        checkBackgroundProcesses();
    }
    
    void ProcessManager::haltProcessItem( ProcessItem *item )
    {
        pthread_mutex_lock(&mutex);
        
        running_items.erase(item);
        halted_items.insert(item);
        
        pthread_mutex_unlock(&mutex);
        
        checkBackgroundProcesses();
        
    }
    
    void ProcessManager::checkBackgroundProcesses()
    {
        pthread_mutex_lock(&mutex);
        
        // Get the next process item to process ( if CPU slots available )
        ProcessItem * item = NULL;
        
        do
        {
            
            item = NULL;
            if( (int)running_items.size() < num_processes )
            {
                // Space for another process running... get the item to be executed or continued
                item = _getNextItemToRun();
            }
            
            if( item )
            {
                running_items.insert( item );	// Insert in the set of running processes
                
                switch (item->state) {
                    case ProcessItem::queued:
                        item->state = ProcessItem::running;
                        item->runInBackground();
                        break;
                    case ProcessItem::halted:
                        item->state = ProcessItem::running;
                        item->unHalt();
                        break;
                    default:
                        LM_X(1,("Unexpected state running item at Engine"));
                        break;
                }
            }
            
        } while ( item != NULL);
        
        LM_T( LmtEngine , ("Engine state for background process: Pending %u Running %u Halted %u", items.size() , running_items.size() , halted_items.size()  ) );
        
        pthread_mutex_unlock(&mutex);
    }
    
    
    void ProcessManager::fill(network::WorkerStatus*  ws)
    {
        
        pthread_mutex_lock(&mutex);

        // Basic information about the number of cores running
        ws->set_used_cores(running_items.size());
        ws->set_total_cores(num_processes);
        
        // Process Manager Status
        // ----------------------------------------------------------------------------
        std::ostringstream process_manager_status;
        
        process_manager_status << "\n\t\tRunning: ";
        for ( std::set<ProcessItem*>::iterator i = running_items.begin () ; i != running_items.end() ; i++ )
            process_manager_status << "[" << (*i)->getStatus() << "] ";
        
        process_manager_status << "\n\t\tHalted: ";
        for ( std::set<ProcessItem*>::iterator i = halted_items.begin () ; i != halted_items.end() ; i++ )
            process_manager_status << "[" << (*i)->getStatus() << "] ";
        
        process_manager_status << "\n\t\tQueued: ";
        for ( std::set<ProcessItem*>::iterator i = items.begin () ; i != items.end() ; i++ )
            process_manager_status << "[" << (*i)->getStatus() << "] ";
        
        ws->set_process_manager_status( process_manager_status.str() );
        
        pthread_mutex_unlock(&mutex);
        
    }	
    
    
    void ProcessManager::quit()
    {
        
		items.clearSet();				// List of items to be executed ( all priorities  )
		halted_items.clearSet();		// Set of items currently being executed but halted
        
        while( running_items.size() > 0 )
        {
            LM_M(("Waiting to finish running background processes"));
            sleep(1);
        }
        
    }
    
}