

#include "logMsg.h"				// LM_X
#include <time.h>
#include <sys/time.h>

#include "DiskManager.h"                    // Own interface
#include "Engine.h"							// ss::Engine
#include "EngineElement.h"					// ss::EngineElement
#include "SamsonSetup.h"					// ss::SamsonSetup
#include "ProcessItem.h"					// ss::ProcessItem
#include "DiskOperation.h"					// ss::DiskOperation
#include "EngineNotificationElement.h"      // ss::EngineNotificationElement

namespace ss
{
    
    
#pragma mark DiskManager
    
    DiskManager::DiskManager()
    {
		pthread_mutex_init(&mutex, 0);			// Mutex to protect elements
        
        // Number of parallel disk operations
        num_disk_operations = 1;
    }
    
    DiskManager::~DiskManager()
    {
		pthread_mutex_destroy(&mutex);			// Mutex to protect elements
    }
    
    void DiskManager::notify( EngineNotification* notification )
    {
        if( notification->channel != notification_disk_operation_request )
            LM_X(1,("DiskManager received a wrong notification"));
        
        if ( notification->object.size() != 1)
            LM_X(1,("DiskManager received a notification with a wrong number of parameters (%d instead of 1)", notification->object.size() ));
        
        // Get the objecy ( not leave in the vector since it would be automatically deleted by engine )
        DiskOperation *diskOperation = (DiskOperation*) notification->object[0];
        notification->object.clear();
        
        diskOperation->copyFrom( notification );    // Copy all the environment variables for the notification comming back
        add( diskOperation );
    }
    
    
	void DiskManager::add( DiskOperation *operation )
	{
		pthread_mutex_lock(&mutex);
		pending_operations.push_back( operation );
		pthread_mutex_unlock(&mutex);
        
		// Check if we can start this operation
		checkDiskOperations();
	}
	
	void DiskManager::finishDiskOperation( DiskOperation *operation )
	{
		pthread_mutex_lock(&mutex);
		running_operations.erase( operation );
		diskStatistics.add( operation );
		
		pthread_mutex_unlock(&mutex);
		
		// Add a notification for this operation ( removed when delegate is notified )
        EngineNotification *notification = new EngineNotification( notification_disk_operation_request_response , operation );
        notification->copyFrom( operation );        // Recover the environment variables to identify this request
        Engine::shared()->notify(notification);    
		
		// Check if there are more operation to be executed
		checkDiskOperations();
	}
	
	// Check if we can run more disk operations
	void DiskManager::checkDiskOperations()
	{
		pthread_mutex_lock(&mutex);
		
		while( ( pending_operations.size() > 0 ) && ( running_operations.size() < (size_t)num_disk_operations ) )
		{
			// Extract the next operation
			DiskOperation *operation = pending_operations.extractFront();
			
			running_operations.insert( operation );
			
			// Run in background
			operation->runInBackGround();
		}
		
		pthread_mutex_unlock(&mutex);
		
	}
    
    
	void DiskManager::fill(network::WorkerStatus*  ws)
	{
		
		pthread_mutex_lock(&mutex);
		
        ws->set_disk_pending_operations( pending_operations.size() + running_operations.size() );
        
		// Disk Manager
		// ----------------------------------------------------------------------------
		std::ostringstream disk_manager_status;
        
		disk_manager_status << "\n\t\tRunning: ";
		for ( std::set<DiskOperation*>::iterator i = running_operations.begin() ; i != running_operations.end() ; i++)
			disk_manager_status << "[" << (*i)->getDescription() << "] ";
		
		disk_manager_status << "\n\t\tQueued: ";
		for ( au::list<DiskOperation>::iterator i = pending_operations.begin() ; i != pending_operations.end() ; i++)
			disk_manager_status << "[" << (*i)->getShortDescription() << "] ";
        
		disk_manager_status << "\n\t\tStatistics: ";
		disk_manager_status << diskStatistics.getStatus();
		
		ws->set_disk_manager_status( disk_manager_status.str() );
		
		pthread_mutex_unlock(&mutex);
		
	}	
    
}