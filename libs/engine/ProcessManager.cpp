

#include "engine/ProcessManager.h"



#include "logMsg/logMsg.h"				// LM_X
#include <time.h>
#include <sys/time.h>

#include "au/Descriptors.h"                         // au::Descriptors
#include "au/TokenTaker.h"                          // au::TokenTake
#include "au/xml.h"         // au::xml...

#include "engine/Notification.h"                    // engine::Notification
#include "engine/ProcessManager.h"                  // engine::Process
#include "engine/Engine.h"							// engine::Engine
#include "engine/EngineElement.h"					// engine::EngineElement
#include "engine/ProcessItem.h"                     // engine::ProcessItem
#include "engine/DiskOperation.h"					// engine::DiskOperation
#include "engine/NotificationElement.h"       // engine::EngineNotificationElement

NAMESPACE_BEGIN(engine)


static ProcessManager *processManager=NULL;

void ProcessManager::init( int _num_processes)
{
    if( processManager )
        LM_X(1,("Please, init processManager only once"));
    processManager = new ProcessManager( _num_processes );
    
}

void* run_check_background_processes(void *p)
{
    // Free resources automatically when this thread finish
    pthread_detach(pthread_self());
    
    ProcessManager* pm = (ProcessManager*) p;
    pm->check_background_processes();
    return  NULL;
    
}

ProcessManager* ProcessManager::shared()
{
    if( !processManager )
        LM_X(1, ("ProcessManager not initialiazed"));
    
    return processManager;
}

ProcessManager::ProcessManager( int _num_processes ) : 
          engine::EngineService("ProcessManager") 
          ,token("engine::ProcessManager")
{
    // By default only one process at a time
    num_processes = _num_processes;  

    // By default, we are not quitting    
    quitting = false;
            
    // Run thread in background to check new processes
    thread_running = true;
    pthread_create(&t_scheduler, NULL, run_check_background_processes, this);

    
    public_max_proccesses = num_processes;
    public_num_proccesses = 0;
    
}

ProcessManager::~ProcessManager()
{
}

void ProcessManager::notify( Notification* notification )
{
    LM_X(1,("Wrong notification at ProcessManager [Listener %lu] %s" , getEngineId() , notification->getDescription().c_str()));
}


void ProcessManager::haltProcessItem( ProcessItem *item )
{
    token_haltProcessItem( item );    
}   

void ProcessManager::add( ProcessItem *item , size_t listenerId  )
{
    // We mae sure, items always come with a listenerId
    item->addListenerId( listenerId );
    
    // set the pointer to myself
    item->processManager = this;
   
    // set the pointer to myself
    item->processManager = this;
   
    LM_T( LmtProcessManager , ("Adding ProcessItem") );
    
    // Add internally
    token_add( item );
    
    LM_T( LmtProcessManager , ("Engine state for background process: Pending %u Running %u Halted %u", items.size() , running_items.size() , halted_items.size()  ) );
    
    LM_T( LmtProcessManager , ("Finish Adding ProcessItem") );
    
}

void ProcessManager::finishProcessItem( ProcessItem *item )
{
    
    ProcessItem* item2 = token_finishProcessItem( item );
    
    if (!item2)
    {
        LM_W(("Finish of an engine proces item that was NOT in the list of running processses." ));
        item->setCanceled();    // Activate the canceled flag to make the process kill itself when possible
        delete item;
        return;
    }
    
    if( item2 != item )
        LM_X(1, ("Major error in Process Manager"));
    
    // Statistics
    if ( ( item->operation_name != "unknown" ) && ( item->working_size > 0 ) )
    {
        std::string name = item->operation_name;
        size_t working_size = item->working_size;
        int time_in_seconds = item->cronometer.getSeconds();
        
        LM_T(LmtProcessManager, ("Reported finish to task '%s' in %d seconds with size %lu", name.c_str() , time_in_seconds , working_size ));
        au::SimpleRateCollection * simple_rate = rates.findOrCreate( name , name );
        simple_rate->push( working_size , time_in_seconds );
    }
        
    // Notify this using the notification Mechanism
    Notification * notification = new Notification( notification_process_request_response , item , item->listeners );
    notification->environment.copyFrom( &item->environment );
    Engine::shared()->notify( notification );
    
}

void ProcessManager::cancel( ProcessItem *item )
{
    // Remove from the list of items
    ProcessItem* item2 = token_cancelProcessItem( item );
    
    if (!item2)
    {
        LM_W(("Cancelation for an engine proces item that was NOT in the list of pending processses.... it is possible running" ));
        item->setCanceled();    // Activate the canceled flag to make the process kill itself when possible
        return;
    }
    
    if( item2 != item )
        LM_X(1, ("Major error in Process Manager"));
    
    // Set this process with an error
    item->error.set( "ProcessItem canceled" );
    
    // Notify this using the notification Mechanism
    Notification * notification = new Notification( notification_process_request_response 
                                                   ,item 
                                                   ,item->listeners 
                                                   );
    notification->environment.copyFrom( &item->environment );
    notification->environment.set("error", "Canceled" );
    Engine::shared()->notify( notification );
    
}

void ProcessManager::check_background_processes()
{
    while( true )
    {
        // Get the next process item to process
        ProcessItem * item = token_getNextProcessItem();
        
        if( item )
        {
            // Run o re-run this item
            switch (item->state) 
            {
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
        else
            usleep( 100000 ); // Sleep 0.1 seconds
        
    }
    
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


void ProcessManager::getInfo( std::ostringstream& output)
{
    token_getInfo( output );    
}

void ProcessManager::quitEngineService()
{
    
    // Set flag to indicate backgroun process we are quitting...
    quitting = true;

    LM_V(("ProcessManager: Waiting main thread to finish"));
    while( thread_running )
        usleep(100000);
    LM_V(("ProcessManager: main thread finished"));
    
    items.clearSet();				// List of items to be executed ( all priorities  )
    halted_items.clearSet();		// Set of items currently being executed but halted
    
    while( running_items.size() > 0 )
    {
        LM_V(("ProcessManager: Waiting %d process to finish" , (int)running_items.size() ));
        sleep(1);
    }
    
    LM_V(("ProcessManager: Finish complete" , (int)running_items.size() ));
    
}

ProcessItem* ProcessManager::token_finishProcessItem( ProcessItem* item )
{
    //LM_M(("Token finish %s" , item->getDescription().c_str() ));
    
    au::TokenTaker tt( &token );
    return running_items.extractFromSet(item);
}

ProcessItem* ProcessManager::token_cancelProcessItem( ProcessItem* item )
{
    au::TokenTaker tt( &token );
    return items.extractFromSet( item );
}

void ProcessManager::token_haltProcessItem( ProcessItem* item )
{
    au::TokenTaker tt( &token );
    
    running_items.erase(item);
    halted_items.insert(item);
}


ProcessItem* ProcessManager::token_getNextProcessItem()
{
    
    au::TokenTaker tt( &token );

    // Update public information about the number of items running
    public_num_proccesses = running_items.size();
    
    //LM_M(("Getting next item to process? (halted: %lu pending: %lu) " , halted_items.size() , items.size() ));
    
    // Check if there are enougth slots..
    if( (int)running_items.size() >= num_processes )
        return NULL;
    
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
        ProcessItem * _item = *i;
        
        //LM_M(("Considering '%s'" , _item->getDescription().c_str() ));
        
        if( _item->isReady() )   // Let's check if the process is ready to be executed
        {
            if( !item )
                item = _item;
            else
            {
                if( _item->priority > item->priority )
                    item = *i;
            }
        }
    }
    
    if( item )
    {
        items.erase( item );	        // Remove form the pending list
        running_items.insert( item );	// Insert in the set of running processes
        
        //LM_M(("Token next item %s" , item->getDescription().c_str() ));
    }
    
    // It is null if no process is required to be executed
    return item;
}


void ProcessManager::token_add( ProcessItem* item )
{
    if ( !item )
        LM_X(1, ("Major error in Process Manager"));
    
    //LM_M(("Token added %s" , item->getDescription().c_str() ));
    
    // Protect multi-thread access
    au::TokenTaker tt( &token );
    
    // Insert in the list of items
    items.insert( item );
    
}

void ProcessManager::token_getInfo( std::ostringstream& output)
{
    au::TokenTaker tt( &token );
    au::xml_open(output, "process_manager");
    
    au::xml_iterate_list(output, "queued", items);
    au::xml_iterate_list(output, "running", running_items);
    au::xml_iterate_list(output, "halted", halted_items);
    
    // List of rates
    au::xml_iterate_map( output , "rates", rates );
    
    // General information
    au::xml_simple( output , "num_processes" ,  num_processes );
    au::xml_simple( output , "num_running_processes" ,  running_items.size() );
    
    au::xml_close(output, "process_manager");
    
}


NAMESPACE_END
