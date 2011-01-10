#include <iostream>				        // std::cout ...

#include "logMsg.h"                     // lmInit, LM_*
#include "coreTracelevels.h"            // LMT_*

#include "Message.h"                    // Message::WorkerStatus, ...
#include "workerStatus.h"               // Message::WorkerStatusData
#include "Macros.h"                     // EXIT, ...
#include "Packet.h"				        // ss::Packet
#include "Network.h"			        // NetworkInterface
#include "Endpoint.h"			        // EndPoint
#include "CommandLine.h"		        // CommandLine
#include "ModulesManager.h"		        // ss:ModulesManager
#include "ControllerTaskManager.h"		// ss:ControllerTaskManager
#include "ControllerTask.h"				// ss:ControllerTask
#include "SamsonController.h"	        // Own interface ss::SamsonController
#include "SamsonSetup.h"				// ss::SamsonSetup
#include "Buffer.h"						// ss::Buffer
#include "MemoryManager.h"				// ss::MemoryManager
#include "FileManager.h"				// ss::FileManager

namespace ss {


	
	void* runBackgroundThreadAutomaticOperation(void* p)
	{
		((SamsonController*)p)->runAutomaticOperationThread();
		assert( false ); // It is not suppoused to return
		return NULL;
	}
	
	
	/* ****************************************************************************
	*
	* SamsonController::SamsonController
	*/
		
	SamsonController::SamsonController( NetworkInterface*  network ) : data(this), jobManager(this) , monitor(this)
	{
		this->network = network;
		network->setPacketReceiverInterface(this);

		// Init session of data
		data.initSession();
		
		// Create space for the worker updates
		num_workers = network->getNumWorkers();
		assert( num_workers > 0);
		
		worker_status		= (network::WorkerStatus**) malloc( sizeof(network::WorkerStatus*) * num_workers);
		worker_status_time	= (struct timeval *) malloc( sizeof( struct timeval ) * num_workers ); 
		
		for (int i = 0 ; i < num_workers ; i++ )
		{
			worker_status[i] = new network::WorkerStatus();
			gettimeofday(&worker_status_time[i], NULL);
		}	
		
	}	

	void SamsonController::runBackgroundProcesses()
	{
		// run the monitor thread in background
		monitor.runInBackground();
		
		// Init background thread
		pthread_t t;
		pthread_create(&t, NULL, runBackgroundThreadAutomaticOperation, this);
	}
	
	
	/* ****************************************************************************
	*
	* receiveHelp - 
	*/
	int SamsonController::receiveHelp(int fromId, Packet* packet)
	{
		// Prepare the help message and sent back to Delilah
		Packet *p = new Packet();
			
		network::HelpResponse *response = p->message.mutable_help_response();
		response->mutable_help()->CopyFrom( packet->message.help() );
		
		// We check if queues or data_queues is selected inside
		data.helpQueues( response , packet->message.help() );
			
		if ( packet->message.help().datas() )
		{
			// Fill with datas information
			modulesManager.helpDatas( response , packet->message.help() );
		}

		if( packet->message.help().operations() )
		{
			// Fill with operations information
			modulesManager.helpOperations( response , packet->message.help() );
		}
			
		// copy the id when returning
		p->message.set_delilah_id( packet->message.delilah_id() );
		
		network->send(this, fromId, Message::HelpResponse, p);
		return 0;
	}
		
		

	/* ****************************************************************************
	*
	* receive - 
	*/
	int SamsonController::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		switch (msgCode)
		{
			case Message::Help:
			{
				receiveHelp( fromId , packet );
				return 0;
				break;
			}
		
			case Message::WorkerTaskConfirmation:
			{
				network::WorkerTaskConfirmation c = packet->message.worker_task_confirmation();
				jobManager.notifyWorkerConfirmation(fromId, &c );
				return 0;
			}
				break;
			
			case Message::WorkerStatus:
			{
				int workerId = network->getWorkerFromIdentifier(fromId);			

				/*
				if (workerId == -1)
					LM_RE(2, ("getWorkerFromIdentifier(%d) failed", fromId));
				status[workerId] = *((Message::WorkerStatusData*) packet->buffer->getData());
				*/
				
				// Copy all the information here to be access when requesting that info
				if (workerId != -1)
				{
					worker_status_lock.lock();
					worker_status[workerId]->CopyFrom( packet->message.worker_status() );
					gettimeofday(&worker_status_time[workerId], NULL);
					worker_status_lock.unlock();

				}
			}

			return 0;
			break;
				
			case Message::UploadDataConfirmation:
			{
				// Uptade data and sent a LoadDataConfirmationResponde message
				
				bool error = false;	// By default, no error
				std::string error_message = "No error message";
				
				size_t job_id = data.getNewTaskId();
				data.beginTask(job_id, "Load process from Delilah");
				
				data.addComment( job_id , "Comments for load process...");
				
				const network::UploadDataConfirmation& loadDataConfirmation = packet->message.upload_data_confirmation();
					
				for (int i = 0 ; i < loadDataConfirmation.file_size() ; i++)
				{
					const network::File& file = loadDataConfirmation.file(i);
					
					std::stringstream command;	
					command << "add_data_file " << file.worker() << " " << file.name() << " " << file.info().size() << " " << loadDataConfirmation.queue();
					DataManagerCommandResponse response =  data.runOperation( job_id , command.str() );
						
					if( response.error )
					{
						error = true;
						error_message = response.output;
						break;
					}
				}
					
				if( error )
					data.cancelTask(job_id, error_message);
				else
					data.finishTask(job_id);
					
				// A message is always sent back to delilah to confirm changes
				Packet *p = new Packet();
				network::UploadDataConfirmationResponse * confirmationResponse = p->message.mutable_upload_data_confirmation_response();
				confirmationResponse->set_error( error );
				confirmationResponse->set_error_message( error_message );
				
				p->message.set_delilah_id( packet->message.delilah_id() );	// Get the same id
				
				network->send(this, fromId, Message::UploadDataConfirmationResponse, p);
			}
			break;

			
				
			case Message::Command:
			{
				
				// Spetial commands to get information
				std::string command = packet->message.command().command();

				au::CommandLine cmdLine;
				cmdLine.parse( command );
				
				if( cmdLine.get_num_arguments() == 0)
					return 0;

				// Spetial commands
				if( cmdLine.isArgumentValue( 0 , "ls" , "ls" ) )
				{
					// Send a message with the list of queues
					
					Packet *p2 = new Packet();
					network::CommandResponse *response = p2->message.mutable_command_response();
					response->set_command( command );
					p2->message.set_delilah_id( packet->message.delilah_id() );
					data.fill( response->mutable_queue_list() , command );
					network->send(this, fromId, Message::CommandResponse, p2);
					
					return	 0;
				}

				if( cmdLine.isArgumentValue( 0 , "automatic_operations" , "ao" ) )
				{
					// Send a message with the list of queues
					
					Packet *p2 = new Packet();
					network::CommandResponse *response = p2->message.mutable_command_response();
					response->set_command( command );
					p2->message.set_delilah_id( packet->message.delilah_id() );
					data.fill( response->mutable_automatic_operation_list() , command );
					network->send(this, fromId, Message::CommandResponse, p2);
					
					return	 0;
				}
				
				
				if( cmdLine.isArgumentValue( 0 , "d" , "datas" ) )
				{
					// Send a message with the list of datas
					
					Packet *p2 = new Packet();
					network::CommandResponse *response = p2->message.mutable_command_response();
					response->set_command( command );
					p2->message.set_delilah_id( packet->message.delilah_id() );
					modulesManager.fill( response->mutable_data_list() , command );
					network->send(this, fromId, Message::CommandResponse, p2);
					
					return	 0;
				}

				if( cmdLine.isArgumentValue( 0 , "o" , "operations" ) )
				{
					// Send a message with the list of operations
					
					Packet *p2 = new Packet();
					network::CommandResponse *response = p2->message.mutable_command_response();
					response->set_command( command );
					p2->message.set_delilah_id( packet->message.delilah_id() );
					modulesManager.fill( response->mutable_operation_list() , command );
					network->send(this, fromId, Message::CommandResponse, p2);
					
					return	 0;
				}

				if( cmdLine.isArgumentValue( 0 , "j" , "jobs" ) )
				{
					// Send a message with the list of jobs
					
					Packet *p2 = new Packet();
					network::CommandResponse *response = p2->message.mutable_command_response();
					response->set_command( command );
					p2->message.set_delilah_id( packet->message.delilah_id() );
					jobManager.fill( response->mutable_job_list() , command );
					network->send(this, fromId, Message::CommandResponse, p2);
					
					return	 0;
				}

				if( cmdLine.isArgumentValue( 0 , "w" , "workers" ) )
				{
					// Send a message with the list of jobs
					
					Packet *p2 = new Packet();
					network::CommandResponse *response = p2->message.mutable_command_response();
					response->set_command( command );
					p2->message.set_delilah_id( packet->message.delilah_id() );
					
					network::WorkerStatusList *wl = response->mutable_worker_status_list();
					int i;
					
					worker_status_lock.lock();
					for (i = 0 ; i < num_workers ; i++)
					{
						network::WorkerStatus *ws =wl->add_worker_status();
						ws->CopyFrom( *worker_status[i] );
						ws->set_time(  DiskStatistics::timeSince( &worker_status_time[i] ) );
					}
					worker_status_lock.unlock();
					
					fill( response->mutable_controller_status() );
					
					network->send(this, fromId, Message::CommandResponse, p2);
					
					return	 0;
				}

				jobManager.addJob( fromId ,  packet->message.command() , packet->message.delilah_id() );
				return 0;
				
				
			}
			break;
		default:
			LM_X(1, ("msg code '%s' not treated ...", messageCode(msgCode)));
			break;
		}


		return 0;
	}

		
		void SamsonController::fill( network::ControllerStatus *status )
		{
			jobManager.fill( status );
		}


		void SamsonController::pushSystemMonitor( MonitorBlock  *system)
		{
			size_t total_memory = 0;
			size_t used_memory = 0;
			
			size_t total_cores = 0;
			size_t used_cores = 0;

			size_t upload_size = 0;
			
			worker_status_lock.lock();

			for (int i = 0 ; i < num_workers ; i++)
			{
				if( worker_status[i] )
				{
					total_memory += worker_status[i]->total_memory();			
					used_memory += worker_status[i]->used_memory();		
					
					used_cores  += worker_status[i]->used_cores();
					total_cores += worker_status[i]->total_cores();
					
					
					upload_size += worker_status[i]->upload_size();
				}
			}
			
			worker_status_lock.unlock();
			
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

	
	void SamsonController::runAutomaticOperationThread()
	{
		while( true )
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
				 
			
			sleep(1);
			
		}
	}

	
}
