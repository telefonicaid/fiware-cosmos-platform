

#include "engine/ProcessManager.h"



#include "logMsg.h"				// LM_X
#include <time.h>
#include <sys/time.h>

#include "au/Descriptors.h"                         // au::Descriptors

#include "engine/ProcessManager.h"                  // engine::Process
#include "engine/Engine.h"							// engine::Engine
#include "engine/EngineElement.h"					// engine::EngineElement
#include "engine/ProcessItem.h"                     // engine::ProcessItem
#include "engine/DiskOperation.h"					// engine::DiskOperation
#include "engine/EngineNotificationElement.h"       // engine::EngineNotificationElement

namespace engine
{
    
    static ProcessManager *processManager=NULL;
    
    void destroy_process_manager()
    {
        LM_M(("ProcessManager terminating"));
        
        if( processManager )
        {
            delete processManager;
            processManager = NULL;
        }
    }
    
    void ProcessManager::init( int _num_processes)
    {
        if( processManager )
            LM_X(1,("Please, init processManager only once"));
        processManager = new ProcessManager( _num_processes );

        atexit(destroy_process_manager);
        
    }

        
    ProcessManager::ProcessManager( int _num_processes )
    {
        pthread_mutex_init(&mutex, 0);			// Mutex to protect elements
        
        // By default only one process at a time
        num_processes = _num_processes;  
        
		// Add the processManager as a listener for process request
        Engine::add( notification_process_request , this );
        Engine::add( notification_process_manager_check_background_process , this );
        
    }
    
    ProcessManager::~ProcessManager()
    {
        pthread_mutex_destroy(&mutex);			// Mutex to protect elements
    }
    
    void ProcessManager::notify( Notification* notification )
    {
        pthread_mutex_lock(&mutex);

        if( notification->isName( notification_process_request ) )                 
        {
            if( !notification->containsObject() )
                LM_X(1,("ProcessManager received a notification_process_request without an object"));
            
            ProcessItem *item = (ProcessItem*) notification->extractObject();
            
            item->environment.copyFrom( &notification->environment );
            _addProcessItem( item );
        }
        else if( notification->isName( notification_process_manager_check_background_process ) )                 
                _checkBackgroundProcesses();
        else if( notification->isName( notification_process_cancel ) )
        {
            // Cancel process
            if( !notification->containsObject() )
                LM_X(1,("ProcessManager received a notification_process_cancel without an object"));
            
            ProcessItem *item = (ProcessItem*) notification->extractObject();
            
            if ( items.extractFromMap( item ) )
                delete item;
            else if ( running_items.extractFromMap( item ) )
            {
                //item->cancel();
                canceled_items.insert( item );
            }
            else if ( halted_items.extractFromMap( item ) )
            {
                //item->cancel();
                canceled_items.insert( item );
                
                item->unHalt();
                
            }

                
            
        }
        else        
                LM_X(1,("Wrong notification at ProcessManager"));
        
        pthread_mutex_unlock(&mutex);
        
    }

    void ProcessManager::finishProcessItem( ProcessItem *item )
    {
        pthread_mutex_lock(&mutex);
        
        running_items.erase(item);
        _checkBackgroundProcesses();
        
        pthread_mutex_unlock(&mutex);
        
        // Notify this using the notification
        Notification * notification = new Notification( notification_process_request_response , item );
        notification->environment.copyFrom( &item->environment );
        Engine::add( notification );
        
    }
    
    void ProcessManager::haltProcessItem( ProcessItem *item )
    {
        pthread_mutex_lock(&mutex);
        
        running_items.erase(item);
        halted_items.insert(item);
        
        _checkBackgroundProcesses();
        
        pthread_mutex_unlock(&mutex);
        
        
    }   
    
    void ProcessManager::_addProcessItem( ProcessItem *item )
    {
        LM_T( LmtProcessManager , ("Adding ProcessItem") );
        
        // set the pointer to myself
        item->processManager = this;
        
        
        items.insert( item );
        
        LM_T( LmtProcessManager , ("Engine state for background process: Pending %u Running %u Halted %u", items.size() , running_items.size() , halted_items.size()  ) );
        
        // Check background processes to see if it is necessary to run new stuff
        _checkBackgroundProcesses();
        
        LM_T( LmtProcessManager , ("Finish Adding ProcessItem") );
        
        
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
    
    void ProcessManager::_checkBackgroundProcesses()
    {
        
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
        
    }

    std::string ProcessManager::str()
    {
        if( processManager )
            return processManager->_str();
        else
            return "ProcessManager not initialized";
    }
    
    int ProcessManager::getNumCores()
    {
        if( processManager )
            return processManager->num_processes;
        else
            return 0;
        
    }
    
    int ProcessManager::getNumUsedCores()
    {
        if( processManager )
            return processManager->running_items.size();
        else
            return 0;
    }
    
    
    
    
    std::string ProcessManager::_str()
    {
        
        pthread_mutex_lock(&mutex);

        
        // Process Manager Status
        // ----------------------------------------------------------------------------
        std::ostringstream process_manager_status;
        
        process_manager_status << "\n\t\tRunning: ";
        {
            au::Descriptors descriptors;
            for ( std::set<ProcessItem*>::iterator i = running_items.begin () ; i != running_items.end() ; i++ )
                descriptors.add( (*i)->getStatus() );
            process_manager_status << descriptors.str();
        }
        
        process_manager_status << "\n\t\tHalted: ";
        {
            au::Descriptors descriptors;
            for ( std::set<ProcessItem*>::iterator i = halted_items.begin () ; i != halted_items.end() ; i++ )
                descriptors.add( (*i)->getStatus() );
        }
        
        process_manager_status << "\n\t\tQueued: ";
        {
            au::Descriptors descriptors;
            for ( std::set<ProcessItem*>::iterator i = items.begin () ; i != items.end() ; i++ )
                descriptors.add( (*i)->getStatus() );
            process_manager_status << descriptors.str();
        }
        pthread_mutex_unlock(&mutex);
        
        return process_manager_status.str();
        
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
