#include <iostream>                 // std::cout ...

#include "logMsg/logMsg.h"                 // lmInit, LM_*
#include "logMsg/traceLevels.h"            // Trace Levels

#include "au/CommandLine.h"            // CommandLine
#include "au/ErrorManager.h"					// au::ErrorManager

#include "engine/Notification.h"

#include "samson/common/Macros.h"                 // EXIT, ...
#include "samson/common/SamsonSetup.h"            // samson::SamsonSetup


#include "samson/network/Message.h"                // Message::WorkerStatus, ...
#include "samson/network/Packet.h"                 // samson::Packet
#include "samson/network/Network.h"                // NetworkInterface
#include "samson/network/Endpoint.h"               // EndPoint

#include "samson/module/ModulesManager.h"         // ss:ModulesManager

#include "ControllerTaskManager.h"  // ss:ControllerTaskManager
#include "ControllerTask.h"         // ss:ControllerTask
#include "samson/controller/SamsonController.h"       // Own interface samson::SamsonController



#include "engine/Buffer.h"                 // samson::Buffer
#include "engine/MemoryManager.h"          // samson::MemoryManager
#include "engine/Engine.h"					// samson::Engine
#include "engine/DiskStatistics.h"			// samson::DiskStatistics


// Global periodic notification to check for the entire system ( disconected workers )
#define notification_check_controller  "notification_check_controller"      

namespace samson {

	/* ****************************************************************************
	*
	* SamsonController::SamsonController
	*/
		
	SamsonController::SamsonController( NetworkInterface*  _network ) : data(this), jobManager(this) , monitor(this)
	{
        // keep a pointer to the network interface
        network = _network;
        
        network->setNodeName("SamsonController");
        
        // Get initial time
		gettimeofday(&init_time, NULL);

        // set me as the receiver of the network interface
		network->setPacketReceiver(this);

		// Init session of data
		data.initSession();
		
		// Create space for the worker updates
		num_workers = network->getNumWorkers();
		if( num_workers <= 0)
			LM_X(1,("Internal error: SamsonController starts with %d workers",num_workers));
		
		worker_status		= (network::WorkerStatus**) malloc( sizeof(network::WorkerStatus*) * num_workers);
        worker_status_cronometer = new au::Cronometer[ num_workers ];  // Cronometer to count the last update from workers

		
		for (int i = 0 ; i < num_workers ; i++ )
		{
			worker_status[i] = new network::WorkerStatus();
		}	

		// Description for the PacketReceiver
		packetReceiverDescription = "samsonController";
		
        // Add as a listener to notifications    
        listen( notification_monitorization );
        engine::Engine::notify( new engine::Notification( notification_monitorization ) , 5  );
        
        // receive notification to check the entire controller system
        listen( notification_check_controller );
        engine::Engine::notify( new engine::Notification( notification_check_controller ) , 5  );
        
	}	
	
	SamsonController::~SamsonController()
	{
		
		for (int i = 0 ; i < num_workers ; i++ )
			delete worker_status[i];
        
		free(worker_status);
        delete[] worker_status_cronometer;
		
	}
    
    void SamsonController::notify( engine::Notification* notification )
    {
        //LM_M(("Controller received a notification  %s " , notification->getName() ));
        
        if( notification->isName(notification_monitorization) )
            monitor.takeSamples();
        else if( notification->isName(notification_check_controller) )
        {
            // Check controller
            
            size_t last_update = 0;

            for ( int w = 0 ; w < num_workers ; w++)
            {
                size_t tmp_time = worker_status_cronometer[w].diffTimeInSeconds();
                if( tmp_time > last_update )
                    last_update = tmp_time;
            }
        
            //LM_M(("Max time disconected... %lu", last_update));
            
            if( last_update > 10 )
            {
                // More than 10 seconds, kill all the tasks
                LM_W(("Killed all task sicne there is a worker that has been %lu seconds disconnected"));
                jobManager.killAll( au::Format::string("Error since a worker has been %lu seconds disconnected", last_update ) );
            }
        }
        else
            LM_X(1,("Unexpected notification channel at SamsonController (%s)" , notification->getName() ));
        
    }
    

	/* ****************************************************************************
	*
	* receive - 
	*/
	void SamsonController::receive( Packet* packet )
	{
        LM_T(LmtNodeMessages, ("SamsonController received %s" , packet->str().c_str()));
        
		int fromId = packet->fromId;
		Message::MessageCode msgCode = packet->msgCode;
		
		
		switch (msgCode)
		{
                
			case Message::WorkerTaskConfirmation:
			{
                // Message from a worker
				network::WorkerTaskConfirmation c = packet->message->worker_task_confirmation();
				int workerId = network->getWorkerFromIdentifier(fromId);			
				jobManager.notifyWorkerConfirmation(workerId, &c );
				return;
			}
				break;
			
			case Message::WorkerStatus:
			{
				int workerId = network->getWorkerFromIdentifier(fromId);			

				// Copy all the information here to be accessed when requesting that info
				if (workerId != -1)
				{
					worker_status[workerId]->CopyFrom( packet->message->worker_status() );
                    worker_status_cronometer[workerId].reset();
				}
                else
                {
                    LM_W(("Received a WorkerStatus from something different than a worker. From id %d", fromId )); 
                }
			}

			return;
				
			break;
	
			case Message::DownloadDataInit:
			{
				// Get a new task id for this operation
				size_t task_id = data.getNewTaskId();
				
				std::string queue = packet->message->download_data_init().queue();
				
				// Init the task at the data manager
				data.beginTask(task_id, "Download process for queue " + queue );
				
				// Comment to inform about this download proces
				data.addComment( task_id , "Download operation");
				
				// Check if queue exist
				
				std::stringstream command;
				command << "check " << queue;
				
				DataManagerCommandResponse r = data.runOperation(task_id, command.str());
				
				if( r.error )
				{
					// Error since queue does not exist
					data.cancelTask(task_id, "Queue does not exist");
					
					// Response message informing about the name of new files
					Packet *p = new Packet(Message::DownloadDataInitResponse);
					network::DownloadDataInitResponse * download_data_init_response = p->message->mutable_download_data_init_response();
					download_data_init_response->mutable_query()->CopyFrom( packet->message->download_data_init() );
					
					// Set the error
					download_data_init_response->mutable_error()->set_message("Queue does not exist");
					
					// Copy the delilah id of this task
					p->message->set_delilah_id( packet->message->delilah_id() );	// Get the same id
					
					// Send the message back to delilah
					network->send( fromId, p);
					
				}
				else
				{
					
					// Response message informing about the load_id ( necessary to download files )
					Packet *p = new Packet(Message::DownloadDataInitResponse);
					
					network::DownloadDataInitResponse * download_data_init_response = p->message->mutable_download_data_init_response();
					download_data_init_response->mutable_query()->CopyFrom( packet->message->download_data_init() );
					download_data_init_response->set_load_id( task_id );

					// Fill with the necessary files for this download
					data.fill( download_data_init_response, queue );
					
					// Create the upload operation
					loadManager.addDownload( task_id );
					
					// Copy the delilah id of this task
					p->message->set_delilah_id( packet->message->delilah_id() );	// Get the same id
					
					// Send the message back to delilah
					network->send( fromId, p);
				}
				
			}
				
			return;
			break;
				
			case Message::UploadDataInit:
			{
				
				
				// Get a new task id for this operation
				size_t task_id = data.getNewTaskId();

				std::string queue = packet->message->upload_data_init().queue();

				// Init the task at the data manager
				data.beginTask(task_id, "Upload process to queue " + queue );
				
				// Comment to inform about this upload proces
				data.addComment( task_id , "Upload operation");
				
				
				// Check if queue exist
				
				std::stringstream command;
				command << "check " << queue;
				
				DataManagerCommandResponse r = data.runOperation(task_id, command.str());
				
				if( r.error )
				{
					// Error since queue does not exist
					
					data.cancelTask(task_id, "Queue does not exist");
					
					// Response message informing about the name of new files
					Packet *p = new Packet(Message::UploadDataInitResponse);
					network::UploadDataInitResponse * upload_data_init_response = p->message->mutable_upload_data_init_response();
					upload_data_init_response->mutable_query()->CopyFrom( packet->message->upload_data_init() );
					upload_data_init_response->set_load_id( task_id );

					// Set the error
					upload_data_init_response->mutable_error()->set_message("Queue does not exist");
					
					// Copy the delilah id of this task
					p->message->set_delilah_id( packet->message->delilah_id() );	// Get the same id
					
					// Send the message back to delilah
					network->send( fromId, p );
					
					
				}
				else
				{
					
					// Create the upload operation
					loadManager.addUpload( task_id );
					
					// Response message informing about the load_id ( necessary to upload files )
					Packet *p = new Packet(Message::UploadDataInitResponse);
					network::UploadDataInitResponse * upload_data_init_response = p->message->mutable_upload_data_init_response();
					upload_data_init_response->mutable_query()->CopyFrom( packet->message->upload_data_init() );
					upload_data_init_response->set_load_id( task_id );

					// Copy the delilah id of this task
					p->message->set_delilah_id( packet->message->delilah_id() );	// Get the same id
					
					// Send the message back to delilah
					network->send( fromId, p);
				}
				
			}
			
			return;
			break;
				
			case Message::UploadDataFinish:
			{
				au::ErrorManager error;	// Error estructure for the entire process
				
				// Final message of the upload proces
				const network::UploadDataFinish& upload_data_finish = packet->message->upload_data_finish();
				
				// Recover the upload operation from the loadManager
				ControllerUploadOperation* uploadOperation = loadManager.extractUploadOperation( upload_data_finish.load_id() );

				if( !uploadOperation )
					error.set( "Unknown upload operation at the controller" );
				else
				{
					size_t task_id = uploadOperation->task_id;
					
					
					for (int i = 0 ; i < upload_data_finish.files_size() ; i++)
					{
						const network::File& file = upload_data_finish.files(i);
						
						
						std::string command = ControllerDataManager::getAddFileCommand(file.worker(), file.name(), file.info().size(), file.info().kvs(), upload_data_finish.queue());
						DataManagerCommandResponse response =  data.runOperation( task_id , command );
						
						if( response.error )
						{
							error.set( response.output );
							break;
						}
					}
					
					if( error.isActivated() )
						data.cancelTask(task_id, error.getMessage() );
					else
						data.finishTask(task_id);					
					
					// remove the opload operation once evrything is reported in data
					delete uploadOperation;
				}
					
				// A message is always sent back to delilah to confirm changes
				Packet *p = new Packet(Message::UploadDataFinishResponse);
				network::UploadDataFinishResponse * upload_data_finish_response = p->message->mutable_upload_data_finish_response();
                upload_data_finish_response->mutable_query()->CopyFrom(upload_data_finish);
				if( error.isActivated() )
					upload_data_finish_response->mutable_error()->set_message( error.getMessage() );
				
				p->message->set_delilah_id( packet->message->delilah_id() );	// Get the same id
				
				network->send( fromId, p);
			}
			break;
				
			case Message::Command:
			{
				
				// Special commands to get information
				std::string command = packet->message->command().command();

				au::CommandLine cmdLine;
				cmdLine.parse( command );
				
				if( cmdLine.get_num_arguments() == 0)
					return;

                if( command == "hello" )
                {
                    
                    // Only valid from workers
                    int worker = network->getWorkerFromIdentifier( packet->fromId );
                    
                    if( worker == -1 )
                    {
                        Packet *p2 = new Packet(Message::CommandResponse);
                        network::CommandResponse *response = p2->message->mutable_command_response();
                        response->mutable_command()->CopyFrom(  packet->message->command() );
                        response->set_finish_command(true);
                        p2->message->set_delilah_id( packet->message->delilah_id() );

                        response->set_error_message("Error. Command \"hello\" is not valid from a delilah client.");
                        
                        network->send( fromId,  p2);
                        
                    }
                    else
                    {
                        // Kill all current tasks
                        jobManager.killAll( au::Format::string("Worker %d restarted", worker )  );
                    }
                    
                    return;
                }
                
				// Spetial commands
				if( cmdLine.isArgumentValue( 0 , "ls" , "ls" ) )
				{
					// Send a message with the list of queues
					
					Packet *p2 = new Packet(Message::CommandResponse);
					network::CommandResponse *response = p2->message->mutable_command_response();
                    response->set_finish_command(true);
					response->mutable_command()->CopyFrom(packet->message->command());
					p2->message->set_delilah_id( packet->message->delilah_id() );

					samson::network::QueueList *ql = response->mutable_queue_list();
					data.fill( ql , command );
					
					// Complement with information about active upload-download operations
					loadManager.fill( ql );
					
					network->send( fromId,  p2);
					
					return;
				}				
				
				if( cmdLine.isArgumentValue( 0 , "d" , "datas" ) )
				{
					// Send a message with the list of datas
					
					Packet *p2 = new Packet(Message::CommandResponse);
					network::CommandResponse *response = p2->message->mutable_command_response();
                    response->set_finish_command(true);
					response->mutable_command()->CopyFrom(packet->message->command());
					p2->message->set_delilah_id( packet->message->delilah_id() );
					ModulesManager::shared()->fill( response->mutable_data_list() , command );
					network->send(fromId, p2);
					
					return;
				}

				if( cmdLine.isArgumentValue( 0 , "o" , "operations" ) )
				{
					// Send a message with the list of operations
					
					Packet *p2 = new Packet(Message::CommandResponse);
					network::CommandResponse *response = p2->message->mutable_command_response();
                    response->set_finish_command(true);
					response->mutable_command()->CopyFrom(packet->message->command());
					p2->message->set_delilah_id( packet->message->delilah_id() );
					ModulesManager::shared()->fill( response->mutable_operation_list() , command );
					network->send( fromId, p2);
					
					return;
				}

				if( cmdLine.isArgumentValue( 0 , "j" , "jobs" ) )
				{
					// Send a message with the list of jobs
					
					Packet *p2 = new Packet(Message::CommandResponse);
					network::CommandResponse *response = p2->message->mutable_command_response();
                    response->set_finish_command(true);
					response->mutable_command()->CopyFrom(packet->message->command());
					p2->message->set_delilah_id( packet->message->delilah_id() );
					jobManager.fill( response->mutable_job_list() , command );
					network->send( fromId, p2);
					
					return;
				}

				// Spetial commands
				if( cmdLine.isArgumentValue( 0 , "kill" , "k" ) )
				{
					if( cmdLine.get_num_arguments() > 1)
					{
						// Get the number of the job
						size_t job_id = atoll( cmdLine.get_argument(1).c_str() );
					
						jobManager.kill( job_id , "Killed by used" );
						
						// Send a message to delilah to confirm this operation ?
						
						Packet *p2 = new Packet(Message::CommandResponse);
						network::CommandResponse *response = p2->message->mutable_command_response();;
                        response->mutable_command()->CopyFrom(packet->message->command());
                        response->set_finish_command(true);
						p2->message->set_delilah_id( packet->message->delilah_id() );
						network->send( fromId, p2);
						 

					}
					
					return;
				}				
				
				if( cmdLine.isArgumentValue( 0 , "w" , "workers" ) )
				{
					// Send a message with the list of jobs
					
					Packet *p2 = new Packet(Message::CommandResponse);
					network::CommandResponse *response = p2->message->mutable_command_response();
                    response->set_finish_command(true);
					response->mutable_command()->CopyFrom(packet->message->command());
					p2->message->set_delilah_id( packet->message->delilah_id() );
					
                    network::SamsonStatus *samsonStatus = response->mutable_samson_status();
                    
					int i;
					
					for (i = 0 ; i < num_workers ; i++)
					{
						network::WorkerStatus *ws =samsonStatus->add_worker_status();
						ws->CopyFrom( *worker_status[i] );
                        // Modify the update time
						ws->set_update_time(  worker_status_cronometer[i].diffTimeInSeconds() );
					}
					
					fill( samsonStatus->mutable_controller_status() );
					
					network->send( fromId, p2);
					
					return;
				}

				if( cmdLine.isArgumentValue(0, "clear_jobs", "cj" ) )
				{
					// Clear finish or error jobs
					jobManager.removeAllFinishJobs();

					
                    Packet *p2 = new Packet( Message::CommandResponse);
                    network::CommandResponse *response = p2->message->mutable_command_response();;
					response->mutable_command()->CopyFrom(packet->message->command());
                    response->set_finish_command(true);
                    p2->message->set_delilah_id( packet->message->delilah_id() );
                    network->send( fromId, p2);
                    
                    return;
				}
				
				
				if ( cmdLine.isArgumentValue(0,"reload_modules" , "reload") )
				{
					// Reload modules
					ModulesManager::shared()->reloadModules();
					
					// Send a message to each worker to also reload modules
					for (int i = 0 ; i < num_workers ; i++)
					{
						Packet *p = new Packet(Message::WorkerTask);
						network::WorkerTask* wt=  p->message->mutable_worker_task();
						wt->set_operation( "reload_modules" );	// Spetial operation to reload modules

                        wt->set_task_id(0); // Special case for reload operation
                        
						network->send(  network->workerGetIdentifier(i) ,  p);
					}

                    Packet *p2 = new Packet(Message::CommandResponse);
                    network::CommandResponse *response = p2->message->mutable_command_response();;
					response->mutable_command()->CopyFrom(packet->message->command());
                    response->set_finish_command(true);
                    p2->message->set_delilah_id( packet->message->delilah_id() );
                    network->send( fromId, p2);
					
					
					return;
				}
				
				
				// Create a new job with the instructions given here
				jobManager.addJob( fromId ,  packet->message->command() , packet->message->delilah_id() );
				return;
				
				
			}
			break;
		default:
			LM_W(("msg code '%s' (%d) not treated ...", messageCode(msgCode), msgCode));
			break;
		}

	}

		
		void SamsonController::fill( network::ControllerStatus *status )
		{
			jobManager.fill( status );
            status->set_network_status(network->getState(""));
            
            ModulesManager::shared()->fill( status );
            
            // Set up time information
            status->set_up_time(au::Format::ellapsedSeconds(&init_time));

		}


		void SamsonController::pushSystemMonitor( MonitorBlock  *system)
		{
			size_t total_memory = 0;
			size_t used_memory = 0;
			
			size_t total_cores = 0;
			size_t used_cores = 0;

			size_t upload_size = 0;
			

			for (int i = 0 ; i < num_workers ; i++)
			{
				if( worker_status[i] )
				{
					total_memory += worker_status[i]->total_memory();			
					used_memory += worker_status[i]->used_memory();		
					
					used_cores  += worker_status[i]->used_cores();
					total_cores += worker_status[i]->total_cores();
					
				}
			}
			
			system->push( "memory"			, used_memory );
			system->push( "total_memory"	, total_memory );
			system->push( "cores"			, used_cores );
			system->push( "total_cores"		, total_cores );
			system->push( "TotalTxTSize"	, data.get_info_txt().size );
			system->push( "TotalKvs"		, data.get_info_kvs().kvs );
			system->push( "TotalSize"		, data.get_info_kvs().size + data.get_info_txt().size );
			system->push( "UploadSize"		, upload_size );
			
		}


	/* ****************************************************************************
	*
	* notificationSent - 
	*/
	void SamsonController::notificationSent(size_t id, bool success)
	{
		// Do something
	}
		


	/* ****************************************************************************
	*
	* notifyWorkerDied - 
	*/
	void SamsonController::notifyWorkerDied( int worker )
	{
		// What to do when a worker died
	}

	


	
}
