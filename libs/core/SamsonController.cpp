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



/* ****************************************************************************
*
* SamsonController::SamsonController
*/
SamsonController::SamsonController
(
	NetworkInterface*  network,
	unsigned short     port,
	char*              setup,
	int                workers,
	int                endpoints
) : data(this), jobManager(this) , monitor(this)
{
	this->network    = network;
	this->port       = port;
	this->setup      = setup;
	this->workers    = workers;
	this->endpoints  = endpoints;

	// Init data manager ( recovering from crash if necessary )
	data.init();
		
	network->setPacketReceiverInterface(this);
		
	int num_workers = SamsonSetup::shared()->num_workers;
	assert(num_workers != -1);
	LM_T(LMT_CONFIG, ("Num workers: %d", num_workers));

	network->initAsSamsonController(port, num_workers);
	
	
	// setup run-time status
	setStatusTile( "Samson Controller" , "controller" );
	//addChildrenStatus( MemoryManager::shared()->getStatus() );
	//addChildrenStatus( &data.getStatus() );
	//addChildrenStatus( &jobManager.getStatus() );
	//addChildrenStatus( &taskManager.getStatus() );
	//addChildrenStatus( FileManager::shared() );
	//addChildrenStatus( network );
	
	// Create space for the worker updates
	worker_status = new network::WorkerStatus[workers];
	
}	



/* ****************************************************************************
*
* run - 
*/
void SamsonController::run()
{
	network->run();											// Run the network interface (blocked)
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
			worker_status[workerId] = packet->message.worker_status();
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

		case Message::StatusRequest:
		{
			// Direct packet response
			Packet *p = new Packet();
			network::StatusResponse *response = p->message.mutable_status_response();
			response->set_title( "Controller" );
			response->set_response( getStatus(packet->message.status_request().command()) );
			network->send(this, fromId, Message::StatusResponse, p);
			
			
			// Send a request to each worker
			for (int i = 0 ; i < network->getNumWorkers() ; i++)
			{
				Packet *p = new Packet();
				network::StatusRequest *request = p->message.mutable_status_request();
				request->set_senderid( fromId );
				network->send(this, network->workerGetIdentifier(i) , Message::StatusRequest, p);
			}
			
			return 0;
			
		}
		break;
		
		case Message::StatusResponse:
		{
			// Forward packet response to delilah
			Packet *p = new Packet();
			network::StatusResponse *response = p->message.mutable_status_response();
			response->CopyFrom( packet->message.status_response() );					// Copy the entire response message

			// Get the sender identifier
			int senderId = packet->message.status_response().senderid();
			
			network->send(this, senderId , Message::StatusResponse, p);
			
			return 0;
			
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
			if( cmdLine.isArgumentValue( 0 , "q" , "queues" ) )
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
				for (int i = 0 ; i < workers ; i++)
					wl->add_worker_status()->CopyFrom( worker_status[i] );
				
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
