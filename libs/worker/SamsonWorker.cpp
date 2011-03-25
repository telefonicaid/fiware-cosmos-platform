#include <iostream>                     // std::cout ...

#include "logMsg.h"                     // lmInit, LM_*
#include "traceLevels.h"                // Trace Levels
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>

#include "Message.h"                    // Message
#include "Macros.h"                     // EXIT, ...
#include "Packet.h"                     // ss::Packet
#include "Network.h"                    // NetworkInterface
#include "Endpoint.h"                   // Endpoint
#include "CommandLine.h"                // CommandLine
#include "SamsonWorker.h"               // Own interfce
#include "SamsonSetup.h"				// ss::SamsonSetup
#include "Format.h"						// au::Format

#include "MemoryManager.h"				// ss::SharedMemory

#include "Engine.h"						// ss::Engine
#include "DiskOperation.h"				// ss::DiskOperation

namespace ss {
	
	/* ****************************************************************************
	 *
	 * Constructor
	 */
	SamsonWorker::SamsonWorker( NetworkInterface* network ) :  taskManager(this) , loadDataManager(this)
	{

		// Description for the PacketReceiver
		packetReceiverDescription = "samsonWorker";
		
		this->network = network;
		network->setPacketReceiver(this);
		
		srand((unsigned int) time(NULL));
		
		// Add the updater to the Engine
		Engine::shared()->add( new SamsonWorkerStatusUpdater(this) );
		
		// Add the file updater to the Engine
		Engine::shared()->add( new SamsonWorkerFileUpdater(this) );

	}
	
	
	/* ****************************************************************************
	 *
	 * run - 
	 */
	
	void SamsonWorker::sendFilesMessage()
	{
		Packet*           p = new Packet();
		network::Command* c = p->message->mutable_command();
		c->set_command( "ls" );
		p->message->set_delilah_id( 0 ); // At the moment no sence at the controller
		//copyEnviroment( &environment , c->mutable_environment() );
		network->send(this, network->controllerGetIdentifier(), Message::Command, p);
	}
	
	
	/* ****************************************************************************
	 *
	 * SamsonWorker::sendWorkerStatus - 
	 */
	void SamsonWorker::sendWorkerStatus(void)
	{
		Packet*                  p  = new Packet();
		network::WorkerStatus*   ws = p->message->mutable_worker_status();
		
		// Fill to all data related with task manager
		taskManager.fill(ws);
		
		// Fill information related with file manager and disk manager
		Engine::shared()->fill( ws );
		
		loadDataManager.fill( ws );
		
		ws->set_used_memory( Engine::shared()->memoryManager.getUsedMemory() );
		
		network->send(this, network->controllerGetIdentifier(), Message::WorkerStatus, p);
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
			taskManager.finishWorker( task_id );
			return;
		}
		
		
	}
	
	
	/**
	 Process the list of files removing unnecessary files
	 */
	
	void SamsonWorker::processListOfFiles( const ::ss::network::QueueList& ql)
	{
		// Get my identifier as worker
		_myWorkerId = network->getWorkerId();
		
		// Generate list of local files ( to not remove them )
		std::set<std::string> files;
		std::set<size_t> load_id;
		
		for (int q = 0 ; q < ql.queue_size() ; q++)
		{
			const network::FullQueue& queue = ql.queue(q);
			for (int f = 0 ; f < queue.file_size() ; f++)
			{
				const network::File &file =  queue.file(f);
				if( file.worker() == _myWorkerId )
				{
					files.insert( file.name() );
				}
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
				
				if( age > 60 ) // Get some time to avoid cross-message between controller and worker
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
			Engine::shared()->add( DiskOperation::newRemoveOperation(*f , NULL) );
			
		}
		
		
	}
	
}
