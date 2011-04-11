#include <iostream>                 // std::cout ...

#include "logMsg.h"                 // lmInit, LM_*
#include "traceLevels.h"            // Trace Levels

#include "Message.h"                // Message::WorkerStatus, ...
#include "workerStatus.h"           // Message::WorkerStatusData
#include "Macros.h"                 // EXIT, ...
#include "Packet.h"                 // ss::Packet
#include "Network.h"                // NetworkInterface
#include "Endpoint.h"               // EndPoint
#include "CommandLine.h"            // CommandLine
#include "ModulesManager.h"         // ss:ModulesManager
#include "ControllerTaskManager.h"  // ss:ControllerTaskManager
#include "ControllerTask.h"         // ss:ControllerTask
#include "SamsonController.h"       // Own interface ss::SamsonController
#include "SamsonSetup.h"            // ss::SamsonSetup
#include "Buffer.h"                 // ss::Buffer
#include "MemoryManager.h"          // ss::MemoryManager
#include "Error.h"					// au::Error
#include "Engine.h"					// ss::Engine
#include "DiskStatistics.h"			// ss::DiskStatistics

namespace ss {

	/* ****************************************************************************
	*
	* SamsonController::SamsonController
	*/
		
	SamsonController::SamsonController( NetworkInterface*  network ) : data(), jobManager(this) , monitor(this)
	{
        
        // Get initial time
		gettimeofday(&init_time, NULL);

		this->network = network;
		network->setPacketReceiver(this);

		// Init session of data
		data.initSession();
		
		// Create space for the worker updates
		num_workers = network->getNumWorkers();
		if( num_workers <= 0)
			LM_X(1,("Internal error: SamsonController starts with %d workers",num_workers));
		
		worker_status		= (network::WorkerStatus**) malloc( sizeof(network::WorkerStatus*) * num_workers);
		worker_status_time	= (struct timeval *) malloc( sizeof( struct timeval ) * num_workers ); 
		
		for (int i = 0 ; i < num_workers ; i++ )
		{
			worker_status[i] = new network::WorkerStatus();
			gettimeofday(&worker_status_time[i], NULL);
		}	

		// Description for the PacketReceiver
		packetReceiverDescription = "samsonController";
		
        
        Engine::shared()->notificationSystem.add(notification_monitorization, this);
        Engine::shared()->notificationSystem.add(notification_check_automatic_operations, this);


        {
            EngineNotification *notification = new EngineNotification( notification_monitorization );
            notification->set("target", "SamsonController" );
            Engine::shared()->notify( notification , 5  );
        }

        {
            EngineNotification * notification = new EngineNotification( notification_check_automatic_operations );
            notification->set("target", "SamsonController" );
            Engine::shared()->notify( notification , 5  );
        }
		
	}	
	
	SamsonController::~SamsonController()
	{
		
		for (int i = 0 ; i < num_workers ; i++ )
			delete worker_status[i];

		free(worker_status);
		free(worker_status_time);
		
	}

    void SamsonController::notify( EngineNotification* notification )
    {
        switch (notification->channel) {
            case notification_monitorization:
                monitor.takeSamples();
                break;
                
            case notification_check_automatic_operations:
                checkAutomaticOperations();
                break;
                
            default:
                LM_X(1,("Unexpected notification channel at SamsonController"));
                break;
        }
        
    }
    
    bool SamsonController::acceptNotification( EngineNotification* notification )
    {
        if( notification->get("target","") != "SamsonController" )
            return false;
        
        return true;
        
        
    }

    

	/* ****************************************************************************
	*
	* receive - 
	*/
	void SamsonController::receive( Packet* packet )
	{
		int fromId = packet->fromId;
		Message::MessageCode msgCode = packet->msgCode;
		
		
		switch (msgCode)
		{
			case Message::WorkerTaskConfirmation:
			{
				network::WorkerTaskConfirmation c = packet->message->worker_task_confirmation();
				int workerId = network->getWorkerFromIdentifier(fromId);			
				jobManager.notifyWorkerConfirmation(workerId, &c );
				return;
			}
				break;
			
			case Message::WorkerStatus:
			{
				int workerId = network->getWorkerFromIdentifier(fromId);			

				// Copy all the information here to be access when requesting that info
				if (workerId != -1)
				{
					worker_status[workerId]->CopyFrom( packet->message->worker_status() );
					gettimeofday(&worker_status_time[workerId], NULL);
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
					Packet *p = new Packet();
					network::DownloadDataInitResponse * download_data_init_response = p->message->mutable_download_data_init_response();
					download_data_init_response->mutable_query()->CopyFrom( packet->message->download_data_init() );
					
					// Set the error
					download_data_init_response->mutable_error()->set_message("Queue does not exist");
					
					// Copy the delilah id of this task
					p->message->set_delilah_id( packet->message->delilah_id() );	// Get the same id
					
					// Send the message back to delilah
					network->send(this, fromId, Message::DownloadDataInitResponse, p);
					
				}
				else
				{
					
					// Response message informing about the load_id ( necessary to download files )
					Packet *p = new Packet();
					
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
					network->send(this, fromId, Message::DownloadDataInitResponse, p);
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
					Packet *p = new Packet();
					network::UploadDataInitResponse * upload_data_init_response = p->message->mutable_upload_data_init_response();
					upload_data_init_response->mutable_query()->CopyFrom( packet->message->upload_data_init() );

					// Set the error
					upload_data_init_response->mutable_error()->set_message("Queue does not exist");
					
					// Copy the delilah id of this task
					p->message->set_delilah_id( packet->message->delilah_id() );	// Get the same id
					
					// Send the message back to delilah
					network->send(this, fromId, Message::UploadDataInitResponse, p);
					
					
				}
				else
				{
					
					// Create the upload operation
					loadManager.addUpload( task_id );
					
					// Response message informing about the load_id ( necessary to upload files )
					Packet *p = new Packet();
					network::UploadDataInitResponse * upload_data_init_response = p->message->mutable_upload_data_init_response();
					upload_data_init_response->mutable_query()->CopyFrom( packet->message->upload_data_init() );
					upload_data_init_response->set_load_id( task_id );

					// Copy the delilah id of this task
					p->message->set_delilah_id( packet->message->delilah_id() );	// Get the same id
					
					// Send the message back to delilah
					network->send(this, fromId, Message::UploadDataInitResponse, p);
				}
				
			}
			
			return;
			break;
				
			case Message::UploadDataFinish:
			{
				au::Error error;	// Error estructure for the entire process
				
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
				Packet *p = new Packet();
				network::UploadDataFinishResponse * upload_data_finish_response = p->message->mutable_upload_data_finish_response();
                upload_data_finish_response->mutable_query()->CopyFrom(upload_data_finish);
				if( error.isActivated() )
					upload_data_finish_response->mutable_error()->set_message( error.getMessage() );
				
				p->message->set_delilah_id( packet->message->delilah_id() );	// Get the same id
				
				network->send(this, fromId, Message::UploadDataFinishResponse, p);
			}
			break;
				
			case Message::Command:
			{
				
				// Spetial commands to get information
				std::string command = packet->message->command().command();

				au::CommandLine cmdLine;
				cmdLine.parse( command );
				
				if( cmdLine.get_num_arguments() == 0)
					return;

				// Spetial commands
				if( cmdLine.isArgumentValue( 0 , "ls" , "ls" ) )
				{
					// Send a message with the list of queues
					
					Packet *p2 = new Packet();
					network::CommandResponse *response = p2->message->mutable_command_response();
                    response->set_finish_command(true);
					response->set_command( command );
					p2->message->set_delilah_id( packet->message->delilah_id() );

					ss::network::QueueList *ql = response->mutable_queue_list();
					data.fill( ql , command );
					
					// Complement with information about active upload-download operations
					loadManager.fill( ql );
					
					network->send(this, fromId, Message::CommandResponse, p2);
					
					return;
				}

				if( cmdLine.isArgumentValue( 0 , "automatic_operations" , "ao" ) )
				{
					// Send a message with the list of queues
					
					Packet *p2 = new Packet();
					network::CommandResponse *response = p2->message->mutable_command_response();
                    response->set_finish_command(true);
					response->set_command( command );
					p2->message->set_delilah_id( packet->message->delilah_id() );
					data.fill( response->mutable_automatic_operation_list() , command );
					network->send(this, fromId, Message::CommandResponse, p2);
					
					return;
				}
				
				
				if( cmdLine.isArgumentValue( 0 , "d" , "datas" ) )
				{
					// Send a message with the list of datas
					
					Packet *p2 = new Packet();
					network::CommandResponse *response = p2->message->mutable_command_response();
                    response->set_finish_command(true);
					response->set_command( command );
					p2->message->set_delilah_id( packet->message->delilah_id() );
					ModulesManager::shared()->fill( response->mutable_data_list() , command );
					network->send(this, fromId, Message::CommandResponse, p2);
					
					return;
				}

				if( cmdLine.isArgumentValue( 0 , "o" , "operations" ) )
				{
					// Send a message with the list of operations
					
					Packet *p2 = new Packet();
					network::CommandResponse *response = p2->message->mutable_command_response();
                    response->set_finish_command(true);
					response->set_command( command );
					p2->message->set_delilah_id( packet->message->delilah_id() );
					ModulesManager::shared()->fill( response->mutable_operation_list() , command );
					network->send(this, fromId, Message::CommandResponse, p2);
					
					return;
				}

				if( cmdLine.isArgumentValue( 0 , "j" , "jobs" ) )
				{
					// Send a message with the list of jobs
					
					Packet *p2 = new Packet();
					network::CommandResponse *response = p2->message->mutable_command_response();
                    response->set_finish_command(true);
					response->set_command( command );
					p2->message->set_delilah_id( packet->message->delilah_id() );
					jobManager.fill( response->mutable_job_list() , command );
					network->send(this, fromId, Message::CommandResponse, p2);
					
					return;
				}

				// Spetial commands
				if( cmdLine.isArgumentValue( 0 , "kill" , "k" ) )
				{
					if( cmdLine.get_num_arguments() > 1)
					{
						// Get the number of the job
						size_t job_id = atoll( cmdLine.get_argument(1).c_str() );
					
						jobManager.kill( job_id );
						
						// Send a message to delilah to confirm this operation ?
						
						Packet *p2 = new Packet();
						network::CommandResponse *response = p2->message->mutable_command_response();;
						response->set_command( command );
                        response->set_finish_command(true);
						p2->message->set_delilah_id( packet->message->delilah_id() );
						network->send(this, fromId, Message::CommandResponse, p2);
						 

					}
					
					return;
				}				
				
				if( cmdLine.isArgumentValue( 0 , "w" , "workers" ) )
				{
					// Send a message with the list of jobs
					
					Packet *p2 = new Packet();
					network::CommandResponse *response = p2->message->mutable_command_response();
                    response->set_finish_command(true);
					response->set_command( command );
					p2->message->set_delilah_id( packet->message->delilah_id() );
					
                    network::SamsonStatus *samsonStatus = response->mutable_samson_status();
                    
					int i;
					
					for (i = 0 ; i < num_workers ; i++)
					{
						network::WorkerStatus *ws =samsonStatus->add_worker_status();
						ws->CopyFrom( *worker_status[i] );
                        // Modify the update time
						ws->set_update_time(  DiskStatistics::timeSince( &worker_status_time[i] ) );
					}
					
					fill( samsonStatus->mutable_controller_status() );
					
					network->send(this, fromId, Message::CommandResponse, p2);
					
					return;
				}

				if( cmdLine.isArgumentValue(0, "clear_jobs", "cj" ) )
				{
					// Clear finish or error jobs
					jobManager.removeAllFinishJobs();

					
                    Packet *p2 = new Packet();
                    network::CommandResponse *response = p2->message->mutable_command_response();;
                    response->set_command( command );
                    response->set_finish_command(true);
                    p2->message->set_delilah_id( packet->message->delilah_id() );
                    network->send(this, fromId, Message::CommandResponse, p2);
                    
                    return;
				}
				
				
				if ( cmdLine.isArgumentValue(0,"reload_modules" , "reload") )
				{
					// Reload modules
					ModulesManager::shared()->reloadModules();
					
					// Send a message to each worker to also reload modules
					for (int i = 0 ; i < num_workers ; i++)
					{
						Packet *p = new Packet();
						network::WorkerTask* wt=  p->message->mutable_worker_task();
						wt->set_operation( "reload_modules" );	// Spetial operation to reload modules

                        wt->set_task_id(0); // Special case for reload operation
                        
						network->send(this,  network->workerGetIdentifier(i) , Message::WorkerTask,  p);
					}

                    Packet *p2 = new Packet();
                    network::CommandResponse *response = p2->message->mutable_command_response();;
                    response->set_command( command );
                    response->set_finish_command(true);
                    p2->message->set_delilah_id( packet->message->delilah_id() );
                    network->send(this, fromId, Message::CommandResponse, p2);
					
					
					return;
				}
				
				
				// Create a new job with the instructions given here
				jobManager.addJob( fromId ,  packet->message->command() , packet->message->delilah_id() );
				return;
				
				
			}
			break;
		default:
			LM_X(1, ("msg code '%s' not treated ...", messageCode(msgCode)));
			break;
		}

	}

		
		void SamsonController::fill( network::ControllerStatus *status )
		{
			jobManager.fill( status );
            status->set_network_status(network->getState(""));
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

	
	void SamsonController::checkAutomaticOperations()
	{
			
		// Get a list of automatic operations
		// Get the next automatic operation
		std::vector<AOInfo> info = data.getNextAutomaticOperations();
		
		for ( size_t i = 0 ; i < info.size() ; i++ )
		{
			std::cout << "Running automatic operation: " << info[i].command  << " id " << info[i].id <<  " \n";
			
			network::Command *command = new network::Command();
			command->set_command( info[i].command );
			jobManager.addJob(-1, *command, info[i].id );
			delete command;
		}
				 
	}

	
}
