

#include "logMsg.h"				// LM_X
#include <time.h>
#include <sys/time.h>

#include "engine/DiskManager.h"                    // Own interface
#include "engine/Engine.h"							// engine::Engine
#include "engine/EngineElement.h"					// engine::EngineElement
#include "engine/ProcessItem.h"                     // engine::ProcessItem
#include "engine/DiskOperation.h"					// engine::DiskOperation
#include "engine/EngineNotificationElement.h"      // engine::EngineNotificationElement
#include "au/Descriptors.h"                         // au::Descriptors


namespace engine
{
    
    
#pragma ---
    
    static DiskManager *diskManager = NULL;
    
    void destroy_diskManager()
    {
        LM_M(("DiskManager terminating"));

        if( diskManager )
        {
            delete diskManager;
            diskManager = NULL;
        }
    }
    
    void DiskManager::init( int _num_disk_operations )
    {
        // Now it is only possible to run with 1
        _num_disk_operations = 1;
        
        if( diskManager )
            LM_X(1,("Please, init diskManager only once"));
        
        atexit(destroy_diskManager);
        
        diskManager = new DiskManager (_num_disk_operations );
    }
    
#pragma mark DiskManager
    
    DiskManager::DiskManager( int _num_disk_operations )
    {
		pthread_mutex_init(&mutex, 0);			// Mutex to protect elements
        
        // Number of parallel disk operations
        num_disk_operations = _num_disk_operations;
        
		// Add the diskManager as a listener for disk operations
        Engine::add( notification_disk_operation_request , this );
        
    }
    
    DiskManager::~DiskManager()
    {
		pthread_mutex_destroy(&mutex);			// Mutex to protect elements
    }
    
    void DiskManager::notify( Notification* notification )
    {
        if( ! notification->isName(notification_disk_operation_request) )
            LM_X(1,("DiskManager received a wrong notification"));
        
        if ( !notification->containsObject() )
            LM_X(1,("DiskManager received a notification without object" ));
        
        // Get the objecy ( not leave in the vector since it would be automatically deleted by engine )
        DiskOperation *diskOperation = (DiskOperation*) notification->extractObject();
        
        // Copy all the environment variables for the notification comming back
        diskOperation->environment.copyFrom( &notification->environment );    
        
        // add the operation to the queue
        add( diskOperation );
    }
    
    
	void DiskManager::add( DiskOperation *operation )
	{
        // Set the pointer to myself
        operation->diskManager = this;
        
		pthread_mutex_lock(&mutex);
		pending_operations.push_back( operation );
		pthread_mutex_unlock(&mutex);
        
		// Check if we can start this operation
		checkDiskOperations();
	}
	
	void DiskManager::finishDiskOperation( DiskOperation *operation )
	{
        // Callback received from background process
        
		pthread_mutex_lock(&mutex);
        
		running_operations.erase( operation );
		diskStatistics.add( operation );
		
		pthread_mutex_unlock(&mutex);
		
		// Add a notification for this operation ( removed when delegate is notified )
        Notification *notification = new Notification( notification_disk_operation_request_response , operation );
        notification->environment.copyFrom( &operation->environment );        // Recover the environment variables to identify this request
        Engine::add(notification);    
		
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

    std::string DiskManager::str()
    {
        if ( diskManager )
            return  diskManager->_str();
        else
            return "DiskManager not initialized";
    }
    
    int DiskManager::getNumOperations()
    {
        if ( diskManager )
            return  diskManager->pending_operations.size() + diskManager->pending_operations.size();
        else
            return 0;
    }
    
    
    
    std::string DiskManager::_str()
	{
		
		pthread_mutex_lock(&mutex);
		
        
		// Disk Manager
		// ----------------------------------------------------------------------------
		std::ostringstream disk_manager_status;
        
		disk_manager_status << "\n\tRunning: ";
		for ( std::set<DiskOperation*>::iterator i = running_operations.begin() ; i != running_operations.end() ; i++)
			disk_manager_status << "[" << (*i)->getDescription() << "] ";
		
		disk_manager_status << "\n\tQueued: ";
        {
            au::Descriptors descriptors;
            
            for ( au::list<DiskOperation>::iterator i = pending_operations.begin() ; i != pending_operations.end() ; i++)
            {
                switch ( (*i)->getType() ) {
                    case DiskOperation::read:
                        descriptors.add("reads");
                        break;
                    case DiskOperation::write:
                        descriptors.add("writes");
                        break;
                    case DiskOperation::append:
                        descriptors.add("appends");
                        break;
                    case DiskOperation::remove:
                        descriptors.add("removes");
                        break;
                }
            }
            
            disk_manager_status << descriptors.str();
            
        }
        
		disk_manager_status << "\n\tStatistics: ";
		disk_manager_status << diskStatistics.getStatus();
		
		pthread_mutex_unlock(&mutex);
        
        return disk_manager_status.str();
		
	}	

    void DiskManager::quit()
    {
		pending_operations.clearList();		// Remove pending operations ( will never be notified )
		
        while( running_operations.size() > 0)
        {
            LM_M(("Waiting %d running disk operations to finish " , running_operations.size() ));
            sleep(1);
        }
        
    }
    
}
