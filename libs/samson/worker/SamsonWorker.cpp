#include <iostream>                     // std::cout ...

#include "logMsg/logMsg.h"                     // lmInit, LM_*
#include "logMsg/traceLevels.h"                // Trace Levels
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>

#include "samson/network/Message.h"                    // Message
#include "samson/common/Macros.h"                     // EXIT, ...
#include "samson/network/Packet.h"                     // samson::Packet
#include "samson/network/Network.h"                    // NetworkInterface
#include "samson/network/Endpoint.h"                   // Endpoint
#include "au/CommandLine.h"                // CommandLine
#include "samson/worker/SamsonWorker.h"               // Own interfce
#include "samson/common/SamsonSetup.h"				// samson::SamsonSetup
#include "au/Format.h"						// au::Format

#include "engine/MemoryManager.h"				// samson::SharedMemory

#include "engine/Engine.h"						// engine::Engine
#include "engine/DiskOperation.h"				// samson::DiskOperation
#include "engine/DiskManager.h"                // Notifications
#include "engine/ProcessManager.h"             // engine::ProcessManager
#include "samson/worker/SharedMemoryManager.h"        // engine::SharedMemoryManager

#include "samson/common/MemoryTags.h"                 // MemoryInput , MemoryOutputNetwork ,...

#define notification_worker_update_files    "notification_worker_update_files"

namespace samson {
	
	/* ****************************************************************************
	 *
	 * Constructor
	 */
	SamsonWorker::SamsonWorker( NetworkInterface* network ) :  taskManager(this) , loadDataManager(this)
	{
        // Get initial time
		gettimeofday(&init_time, NULL);
        
		// Description for the PacketReceiver
		packetReceiverDescription = "samsonWorker";
		
		this->network = network;
		network->setPacketReceiver(this);
		
		srand((unsigned int) time(NULL));
        
        // Add SamsonWorker as listener of the update files
        engine::Engine::add(notification_worker_update_files, this);
        
        // Add samsonWorker as listere to send an update of the satatus
        engine::Engine::add(notification_samson_worker_send_status_update, this);
        
        // Add samsonWorker as lister to send traces to delilahs
        engine::Engine::add(notification_samson_worker_send_trace, this);
        
        
        // Notification of the files
        {
        engine::Notification *notification = new engine::Notification(notification_worker_update_files);
        notification->environment.set("target", "SamsonWorker");
        notification->environment.setInt("worker", network->getWorkerId() );
        engine::Engine::add( notification, 5 );
        }
        
        // Notification to update state
        {
        engine::Notification *notification = new engine::Notification(notification_samson_worker_send_status_update);
        notification->environment.set("target", "SamsonWorker");
        notification->environment.setInt("worker", network->getWorkerId() );
        engine::Engine::add( notification, 3 );
        }
        
        
	}
	
	
	/* ****************************************************************************
	 *
	 * run - 
	 */
	
	void SamsonWorker::sendFilesMessage()
	{        
		Packet*           p = new Packet(Message::Command);
		network::Command* c = p->message->mutable_command();
		c->set_command( "ls" );
		p->message->set_delilah_id( 0 ); // At the moment no sence at the controller
		//copyEnviroment( &environment , c->mutable_environment() );
		network->send(this, network->controllerGetIdentifier(), p);
	}
	
	
	/* ****************************************************************************
	 *
	 * SamsonWorker::sendWorkerStatus - 
	 */
	void SamsonWorker::sendWorkerStatus(void)
	{
		Packet*                  p  = new Packet( Message::WorkerStatus );
		network::WorkerStatus*   ws = p->message->mutable_worker_status();
		
		// Fill to all data related with task manager
		taskManager.fill(ws);
		
		// Fill information related with file manager and disk manager
        ws->set_engine_status( engine::Engine::str() );

        // Memory manager
        std::ostringstream memory_status;

        memory_status << "\n";
        engine::MemoryManager *mm = engine::MemoryManager::shared();
        
            size_t memory = mm->getMemory();
            memory_status << "\t Total memory: " << au::Format::string( memory ) << "\n";

        {
            size_t size = mm->getUsedMemoryByTag( MemoryInput );
            memory_status << "\t\t Input memory    " << au::Format::string( size  ) << " ( " << au::Format::percentage_string( size, memory ) << " )\n";
        }

        {
            size_t size = mm->getUsedMemoryByTag( MemoryOutputDisk );
            memory_status << "\t\t Output disk     " << au::Format::string( size  ) << " ( " << au::Format::percentage_string( size, memory ) << " )\n";
        }

        {
            size_t size = mm->getUsedMemoryByTag( MemoryOutputNetwork );
            memory_status << "\t\t Output network  " << au::Format::string( size  ) << " ( " << au::Format::percentage_string( size, memory ) << " )\n";
        }

        {
            size_t size = mm->getUsedMemoryByTag( MemoryAccumulated );
            memory_status << "\t\t Accumulated     " << au::Format::string( size  ) << " ( " << au::Format::percentage_string( size, memory ) << " )\n";
        }
        
        {
            size_t size = mm->getUsedMemoryByTag( MemoryBlocks );
            memory_status << "\t\t Blocks          " << au::Format::string( size  ) << " ( " << au::Format::percentage_string( size, memory ) << " )\n";
        }
        
        
        ws->set_memory_status( memory_status.str() );

        // Process manager
        ws->set_process_manager_status( engine::ProcessManager::str() );

        // Disk manager
        ws->set_disk_manager_status( engine::DiskManager::str() );
        
		// Load data manager
		loadDataManager.fill( ws );
		
        // Network status
        ws->set_network_status( network->getState("") );
        
        // Shared memory manager status
        ws->set_shared_memory_status( engine::SharedMemoryManager::str() );

        // Set up time information
        ws->set_up_time(au::Format::ellapsedSeconds(&init_time));
        
        // Set information about queues
        ws->set_queues_status( queuesManager.getStatus() );
        
        
        // Numerical information for better presentation
        
        ws->set_total_memory( engine::MemoryManager::shared()->getMemory() );
        ws->set_used_memory( engine::MemoryManager::shared()->getUsedMemory());
        
        ws->set_total_cores(engine::ProcessManager::getNumCores());
        ws->set_used_cores(engine::ProcessManager::getNumUsedCores());

        ws->set_disk_pending_operations(engine::DiskManager::getNumOperations());

        // Send the message    
		network->send(this, network->controllerGetIdentifier(), p);
        
	}
	
	/* ****************************************************************************
	 *
	 * SamsonWorker::receive - 
	 */
	void SamsonWorker::receive( Packet* packet )
	{
		int fromId = packet->fromId;
		Message::MessageCode msgCode = packet->msgCode;
		
		if (msgCode == Message::WorkerTask)
		{
			// A packet with a particular command is received (controller expect to send a confirmation message)
			LM_T(LmtTask, ("Got a WorkerTask message"));
			
			if( packet->message->worker_task().operation() == "reload_modules" )
			{
				// Spetial operation to reload modules
				ModulesManager::shared()->reloadModules();
				return;
			}
			
			// add task to the task manager
			taskManager.addTask( packet->message->worker_task() );
			return;
		}
		
		if (msgCode == Message::WorkerTaskKill)
		{
			// A packet with a particular command is received (controller expect to send a confirmation message)
			LM_T(LmtTask, ("Got a WorkerTaskKill message"));
			
			// add task to the task manager
			taskManager.killTask( packet->message->worker_task_kill() );
			return;
		}

		if (msgCode == Message::WorkerTaskRemove)
		{
            
            size_t task_id = packet->message->worker_task_remove().task_id();
			// A packet with a particular command is received (controller expect to send a confirmation message)
			LM_T(LmtTask, ("Got a WorkerTaskRemove message with task id %lu", task_id));
			
			// add task to the task manager
			taskManager.removeTask( task_id  );
			return;
		}
		
        
		// List of local file ( remove unnecessary files )
		if (msgCode == Message::CommandResponse)
		{
			processListOfFiles( packet->message->command_response().queue_list() );
			return;
		}
		
		
		// Load data files to be latter confirmed to controller
		if (msgCode == Message::UploadDataFile)
		{
			loadDataManager.addUploadItem(fromId, packet->message->upload_data_file(), packet->message->delilah_id() , packet->buffer );
			return;
		}
		
		// Download data files
		if (msgCode == Message::DownloadDataFile)
		{
			loadDataManager.addDownloadItem(fromId, packet->message->download_data_file() , packet->message->delilah_id() );
			return;
		}
		
		/**
		 Data packets go directly to the DataBuffer to be joined and flushed to disk
		 DataManager is notifyed when created a new file or finish everything 
		 */
		
		if( msgCode == Message::WorkerDataExchange )
		{
			// New data packet for a particular queue inside a particular task environment
			
			size_t task_id = packet->message->data().task_id();			
            network::WorkerDataExchange data = packet->message->data();
			taskManager.addBuffer( task_id , data, packet->buffer );
			
			return;
		}
		
		/** 
		 Data Close message is sent to notify that no more data will be generated
		 We have to wait for "N" messages ( one per worker )
		 */
		
		if( msgCode == Message::WorkerDataExchangeClose )
		{
			
			size_t task_id = packet->message->data_close().task_id();
            int worker_from = network->getWorkerFromIdentifier( fromId );
            
			taskManager.finishWorker( worker_from , task_id );
            
			return;
		}
		
		
	}
	
    // Receive notifications
    void SamsonWorker::notify( engine::Notification* notification )
    {
        if ( notification->isName(notification_worker_update_files) )
            sendFilesMessage();
        else if ( notification->isName(notification_samson_worker_send_status_update))
			sendWorkerStatus();
        else if ( notification->isName(notification_samson_worker_send_trace))
        {
            if ( !notification->containsObject() )
            {
                LM_W(("SamsonWorker: Send trace without an object"));
                return;
            }
            else
            {
                //LM_M(("SamsonWorking sending a trace to all delilahs..."));
                Packet *p = (Packet*) notification->extractObject();
                network->delilahSend( this , p );
            }
        }
        else
            LM_X(1, ("SamsonWorker received an unexpected notification %s", notification->getDescription().c_str()));
    }
    
    bool SamsonWorker::acceptNotification( engine::Notification* notification )
    {
        // Spetial case: Accept always
        if( notification->isName(notification_samson_worker_send_trace) )
            return true;
        
        // Only accept notifications for my worker. This is only necessary when testing samsonLocal with multiple workers
        
        if( notification->environment.get("target","") != "SamsonWorker" )
            return  false;
        
        if( notification->environment.getInt("worker", -1) != network->getWorkerId() )
            return false;
        
        return true;
        
    }    
	
	/**
	 Process the list of files removing unnecessary files
	 */
	
	void SamsonWorker::processListOfFiles( const ::samson::network::QueueList& ql)
	{
        
		// Generate list of local files ( to not remove them )
		std::set<std::string> files;
		std::set<size_t> load_id;
		
		for (int q = 0 ; q < ql.queue_size() ; q++)
		{
			const network::FullQueue& queue = ql.queue(q);
			for (int f = 0 ; f < queue.file_size() ; f++)
			{
				const network::File &file =  queue.file(f);
                files.insert( file.name() );
			}
		}
		
		// Get the files from the active tasks to not remove them
		for (int t = 0 ; t < ql.tasks_size() ; t++)
			for (int f = 0 ; f < ql.tasks(t).filename_size() ; f++)
				files.insert( ql.tasks(t).filename(f)  );
		
		// Get the list of active load_ids to not remove temporal files of these upload operations
		for (int t = 0 ; t < ql.load_id_size() ; t++)
			load_id.insert(ql.load_id(t));
		
		
		// Get the list of files to be removed
		
		std::set< std::string > remove_files;
		
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir( SamsonSetup::shared()->dataDirectory.c_str() )) == NULL) {
			
			// LOG and error to indicate that data directory cannot be access
			return;
		}
		
		while ((dirp = readdir(dp)) != NULL) {
			
			std::string path = SamsonSetup::shared()->dataDirectory + "/" + dirp->d_name;
			
			struct ::stat info;
			stat(path.c_str(), &info);
			
			
			if( S_ISREG(info.st_mode) )
			{
				
				// Get modification date to see if it was just created
				time_t now;
				time (&now);
				double age = difftime ( now , info.st_mtime );
				
				if( age > 7200 ) // Get some time to avoid cross-message between controller and worker
				{
					
					// Get the task from the file name 
					std::string file_name = dirp->d_name;
					size_t pos = file_name.find("_");
					size_t task = 0;
					if( pos != std::string::npos )
						task = atoll( file_name.substr( 0 , pos ).c_str() );
					
					if( files.find( file_name ) == files.end() )
					{
						// If the file is not in the list remove this
						
						if( file_name.substr( 0 , 14 ) == "file_updaload_" )
						{
							// Exception: temporal upload files
							size_t pos = file_name.find("_" , 14);
							size_t _load_id = atoll( file_name.substr(14 , pos).c_str() );
							if( load_id.find(_load_id ) == load_id.end() )
							{
								LM_T(LmtDisk,("Removing file %s since the id (%lu) is not in the list of active load operations (size:%d)", file_name.c_str() , _load_id , load_id.size() ));
								remove_files.insert( dirp->d_name );
							}
						}
						else
						{
							LM_T(LmtDisk,("Remove file %s since it is not in any queue", file_name.c_str()));
							remove_files.insert( dirp->d_name );
						}
					}
				}
			}
		}
		closedir(dp);
		
		// Remove the selected files
		for ( std::set< std::string >::iterator f = remove_files.begin() ; f != remove_files.end() ; f++)
		{
			// Add a remove opertion to the engine
            engine::DiskOperation * operation =  engine::DiskOperation::newRemoveOperation(  SamsonSetup::dataFile(*f) );
            engine::Notification *notification = new engine::Notification( notification_disk_operation_request , operation );
            notification->environment.set("target","no-where");
			engine::Engine::add( notification );
		}
		
		
	}
	
}
