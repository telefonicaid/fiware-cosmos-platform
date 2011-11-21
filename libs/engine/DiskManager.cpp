

#include <time.h>
#include <sys/time.h>

#include "logMsg/logMsg.h"				// LM_X

#include "au/xml.h"         // au::xml...

#include "engine/Engine.h"							// engine::Engine
#include "engine/EngineElement.h"					// engine::EngineElement
#include "engine/Notification.h"                    // engine::Notification
#include "engine/ProcessItem.h"                     // engine::ProcessItem
#include "engine/DiskOperation.h"					// engine::DiskOperation
#include "engine/NotificationElement.h"      // engine::EngineNotificationElement
#include "au/Descriptors.h"                         // au::Descriptors

#include "engine/DiskManager.h"                    // Own interface


namespace engine
{
    
    
#pragma ---
    
    static DiskManager *diskManager = NULL;
    
    void DiskManager::init( int _num_disk_operations )
    {
        // Now it is only possible to run with 1
        _num_disk_operations = 1;
        
        if( diskManager )
            LM_X(1,("Please, init diskManager only once"));
        
        diskManager = new DiskManager (_num_disk_operations );
    }
 
    void DiskManager::destroy( )
    {
        
        if( !diskManager )
            LM_X(1,("Please, init diskManager before destroying it"));
        
        delete diskManager;
        diskManager = NULL;
    }
    
    DiskManager* DiskManager::shared()
    {
        if (!diskManager)
            LM_X(1, ("Please init DiskManager before using it"));
        return diskManager;
    }
    
#pragma mark DiskManager
    
    DiskManager::DiskManager( int _num_disk_operations )
    {
		pthread_mutex_init(&mutex, 0);			// Mutex to protect elements
        
        // Number of parallel disk operations
        num_disk_operations = _num_disk_operations;
    }
    
    DiskManager::~DiskManager()
    {
		pthread_mutex_destroy(&mutex);			// Mutex to protect elements
    }
    
	void DiskManager::add( DiskOperation *operation )
	{
        // Set the pointer to myself
        operation->diskManager = this;
        
		pthread_mutex_lock(&mutex);
        
		// Insert the operation in the queue of pending operations
        pending_operations.insert( _find_pos(operation), operation );
        
		pthread_mutex_unlock(&mutex);
        
		// Check if we can start this operation
		checkDiskOperations();
	}

	void DiskManager::cancel( DiskOperation *operation )
	{
        
		pthread_mutex_lock(&mutex);

        if( pending_operations.extractFromList( operation ) )
        {
            // Add a notification for this operation ( removed when delegate is notified )
            Notification *notification = new Notification( notification_disk_operation_request_response , operation , operation->listeners );
            notification->environment.copyFrom( &operation->environment );        // Recover the environment variables to identify this request
            Engine::shared()->notify(notification);            
        }
        
		pthread_mutex_unlock(&mutex);
        
		// Check if we can start this operation
		checkDiskOperations();
	}
	
    
	void DiskManager::finishDiskOperation( DiskOperation *operation )
	{
        // Callback received from background process
        
		pthread_mutex_lock(&mutex);
        
		running_operations.erase( operation );
		diskStatistics.add( operation->getType() , operation->getSize() );
		
		pthread_mutex_unlock(&mutex);
		
		LM_T( LmtDisk , ("DiskManager::finishDiskOperation erased and ready to send notification on file:%s", operation->fileName.c_str() ));

		// Add a notification for this operation to the required target listener
        Notification *notification = new Notification( notification_disk_operation_request_response , operation , operation->listeners );
        notification->environment.copyFrom( &operation->environment );        // Recover the environment variables to identify this request
        LM_T( LmtDisk , ("DiskManager::finishDiskOperation notification sent on file:%s and ready to share and checkDiskOperations", operation->fileName.c_str() ));
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

    int DiskManager::getNumOperations()
    {
		pthread_mutex_lock(&mutex);
        size_t num_operations = pending_operations.size() + diskManager->running_operations.size();
		pthread_mutex_unlock(&mutex);
        
        return num_operations;
        
    }

    
    size_t DiskManager::getReadRate()
    {
		pthread_mutex_lock(&mutex);
        size_t rate = diskStatistics.item_read.getLastMinuteRate();
		pthread_mutex_unlock(&mutex);
        
        return rate;
    }
    
    size_t DiskManager::getWriteRate()
    {
		pthread_mutex_lock(&mutex);
        size_t rate = diskStatistics.item_write.getLastMinuteRate();
		pthread_mutex_unlock(&mutex);
        
        return rate;
    }

    void DiskManager::getInfo( std::ostringstream& output)
    {
		pthread_mutex_lock(&mutex);
		
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
        
		pthread_mutex_unlock(&mutex);
        
    }
    
    std::string DiskManager::str()
	{
		
		pthread_mutex_lock(&mutex);
		
        
		// Disk Manager
		// ----------------------------------------------------------------------------
		std::ostringstream disk_manager_status;
        
		disk_manager_status << "\n\tRunning: ";
		for ( std::set<DiskOperation*>::iterator i = running_operations.begin() ; i != running_operations.end() ; i++)
			disk_manager_status << "[" << (*i)->getDescription() << "] ";
		
		disk_manager_status << "\n\tQueued: ";
        
		for ( std::list<DiskOperation*>::iterator i = pending_operations.begin() ; i != pending_operations.end() ; i++)
			disk_manager_status << "[" << (*i)->getShortDescription() << "] ";
        
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

    
    au::list<DiskOperation>::iterator DiskManager::_find_pos( DiskOperation *diskOperation )
    {
        for (au::list<DiskOperation>::iterator i = pending_operations.begin() ; i != pending_operations.end() ; i++)
        {
            if( diskOperation->compare(*i) )
                return i;
        }
        return pending_operations.end();
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
