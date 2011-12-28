

#include <time.h>
#include <sys/time.h>

#include "logMsg/logMsg.h"				// LM_X

#include "au/xml.h"         // au::xml...
#include "au/TokenTaker.h"         // au::TokenTaker...

#include "engine/Engine.h"							// engine::Engine
#include "engine/EngineElement.h"					// engine::EngineElement
#include "engine/Notification.h"                    // engine::Notification
#include "engine/ProcessItem.h"                     // engine::ProcessItem
#include "engine/DiskOperation.h"					// engine::DiskOperation
#include "engine/NotificationElement.h"      // engine::EngineNotificationElement
#include "au/Descriptors.h"                         // au::Descriptors

#include "engine/DiskManager.h"                    // Own interface


NAMESPACE_BEGIN(engine)


static DiskManager *diskManager = NULL;

void DiskManager::init( int _num_disk_operations )
{
    if( diskManager )
        LM_X(1,("Please, init diskManager only once"));
    
    diskManager = new DiskManager ( _num_disk_operations );
}

DiskManager* DiskManager::shared()
{
    if (!diskManager)
        LM_X(1, ("Please init DiskManager before using it"));
    return diskManager;
}

void DiskManager::reset()
{
    if( diskManager )
    {
        diskManager->quitEngineService();
        diskManager = NULL;    
    }
    
}

#pragma mark DiskManager

void* run_disk_manager_worker( void* p )
{
    DiskManager* dm = (DiskManager*) p;
    dm->run_worker();
    return NULL;
}

DiskManager::DiskManager( int _num_disk_operations ) : engine::EngineService("DiskManager"), token("engine::DiskManager")
{
    // Number of parallel disk operations
    num_disk_operations = _num_disk_operations;
    
    // Init counter for number of workers
    num_disk_manager_workers = 0;
    
    quitting = false;

    // Create as many threads as required
    createThreads();

}


void DiskManager::createThreads()
{
    // Create as many workers as necessary
    while( get_num_disk_manager_workers() < num_disk_operations )
    {
        // Increse the counter of worker
        add_worker();
        
        pthread_t t;
        pthread_create(&t, NULL, run_disk_manager_worker, this);
    }

}

DiskManager::~DiskManager()
{
}

void DiskManager::quitEngineService()
{
    LM_V(("Waiting for background threads of the disk manager"));
    quitting = true;
    while( get_num_disk_manager_workers() > 0 )
        usleep(100000);
    LM_V(("Done .... Waiting for background threads of the disk manager"));
}


void DiskManager::add( DiskOperation *operation )
{
    // Mutex protection
    au::TokenTaker tt(&token);
    
    // Set the pointer to myself
    operation->diskManager = this;
    
    // Insert the operation in the queue of pending operations
    pending_operations.insert( _find_pos(operation), operation );
    
}

void DiskManager::cancel( DiskOperation *operation )
{
    au::TokenTaker tt(&token);
    if( pending_operations.extractFromList( operation ) )
    {
        // Add a notification for this operation ( removed when delegate is notified )
        Notification *notification = new Notification( notification_disk_operation_request_response , operation , operation->listeners );
        notification->environment.copyFrom( &operation->environment );        // Recover the environment variables to identify this request
        Engine::shared()->notify(notification);            
    }
}

void DiskManager::finishDiskOperation( DiskOperation *operation )
{
    // Callback received from background process
    
    // Mutex protection
    au::TokenTaker tt(&token);
    
    running_operations.erase( operation );
    diskStatistics.add( operation->getType() , operation->getSize() );
    
    LM_T( LmtDisk , ("DiskManager::finishDiskOperation erased and ready to send notification on file:%s", 
                     operation->fileName.c_str() 
                     ));
    
    // Add a notification for this operation to the required target listener
    Notification *notification = new Notification( notification_disk_operation_request_response , operation , operation->listeners );
    notification->environment.copyFrom( &operation->environment );        // Recover the environment variables to identify this request
    
    LM_T( LmtDisk , ("DiskManager::finishDiskOperation notification sent on file:%s and ready to share and checkDiskOperations", operation->fileName.c_str() ));
    Engine::shared()->notify(notification);    
}


DiskOperation * DiskManager::getNextDiskOperation(  )
{
    // Mutex protection
    au::TokenTaker tt(&token);
    
    if( pending_operations.size() == 0 )
        return NULL;
    
    // Extract the next operation
    DiskOperation *operation = pending_operations.extractFront();
    
    // Insert in the running list
    running_operations.insert( operation );

    return operation;
}


int DiskManager::get_num_disk_manager_workers(  )
{
    // Mutex protection
    au::TokenTaker tt(&token);
    return num_disk_manager_workers;
}


void DiskManager::add_worker(  )
{
    // Mutex protection
    au::TokenTaker tt(&token);
    
    num_disk_manager_workers ++;
}


// Check if we can run more disk operations
void DiskManager::run_worker()
{
    while( true )
    {
        // Check if I should quit
        if( check_quit_worker() )
        {
            return;
        }
        
        DiskOperation* operation = getNextDiskOperation();
        if (operation)
            operation->run();
        else
            usleep(100000);
    }
    
    
}

bool DiskManager::check_quit_worker(  )
{
    // Mutex protection
    au::TokenTaker tt(&token);
    
    if (quitting)
    {
        num_disk_manager_workers--;
        return true;
    }

    if( num_disk_manager_workers > num_disk_operations)
    {
        num_disk_manager_workers--;
        return true;
    }
    
    return false;
    
}

int DiskManager::getNumOperations()
{
    // Mutex protection
    au::TokenTaker tt(&token);
    
    return pending_operations.size() + diskManager->running_operations.size();
}

void DiskManager::setNumOperations( int _num_disk_operations )
{
    {
        // Mutex protection
        au::TokenTaker tt(&token);
        num_disk_operations = _num_disk_operations;
    }
    
    // Create as many threads as required
    createThreads();
    
}



void DiskManager::getInfo( std::ostringstream& output)
{
    // Mutex protection
    au::TokenTaker tt(&token);
    
    output << "<disk_manager>\n";
    
    au::xml_simple( output , "num_pending_operations" , pending_operations.size() );
    au::xml_simple( output , "num_running_operations" , pending_operations.size() );
    
    output << "<running>\n";
    for ( std::set<DiskOperation*>::iterator i = running_operations.begin() ; i != running_operations.end() ; i++)
        (*i)->getInfo(output);
    output << "</running>\n";
    
    output << "<queued>\n";
    for ( std::list<DiskOperation*>::iterator i = pending_operations.begin() ; i != pending_operations.end() ; i++)
        (*i)->getInfo(output);
    output << "</queued>\n";
    
    output << "<statistics>\n";
    diskStatistics.getInfo( output );
    output << "</statistics>\n";
    
    output << "</disk_manager>\n";
    
}

au::list<DiskOperation>::iterator DiskManager::_find_pos( DiskOperation *diskOperation )
{
    for (au::list<DiskOperation>::iterator i = pending_operations.begin() ; i != pending_operations.end() ; i++)
    {
        if( diskOperation->compare(*i) )
            return i;
    }
    return pending_operations.end();
}
 
NAMESPACE_END
