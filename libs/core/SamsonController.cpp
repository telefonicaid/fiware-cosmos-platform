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

namespace ss {

/* ****************************************************************************
*
* SamsonController::SamsonController
*/
SamsonController::SamsonController(NetworkInterface* network, unsigned short port, char* setup, int workers, int endpoints) : 	data(this) , jobManager(this) , taskManager(this)
{
	this->network    = network;
	this->port       = port;
	this->setup      = setup;
	this->workers    = workers;
	this->endpoints  = endpoints;

	LM_M(("endpoints: %d", endpoints));

	// Init data manager ( recovering from crash if necessary )
	data.init();
		
	network->setPacketReceiverInterface(this);
		
	LM_T(LMT_CONFIG, ("calling loadSetup"));			
		
	int num_workers = SamsonSetup::shared()->getInt(SETUP_num_workers, -1);
	assert(num_workers != -1);
	LM_T(LMT_CONFIG, ("Num workers: %d", num_workers));
	network->initAsSamsonController(port, num_workers);


	LM_M(("endpoints: %d", endpoints));
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
	Packet p;
		
	network::HelpResponse *response = p.message.mutable_help_response();

	if( packet->message.help().queues() )
	{
		// Fill with queues information
		data.helpQueues( response , packet->message.help() );
		response->set_queues( true );
	}
	else
		response->set_queues( false );
		
	if (packet->message.help().datas())
	{
		// Fill with datas information
		modulesManager.helpDatas( response , packet->message.help() );
		response->set_datas(true);
	}
	else
		response->set_datas(false);

	if( packet->message.help().operations() )
	{
		// Fill with operations information
		modulesManager.helpOperations( response , packet->message.help() );
		response->set_operations(true);
	}
	else
		response->set_operations(false);
		
	LM_M(("help response with %d bytes", p.message.ByteSize()));
	network->send(this, fromId, Message::HelpResponse, &p);
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
			taskManager.notifyWorkerConfirmation(fromId, packet->message.worker_task_confirmation() );
			return 0;
			break;
		
		case Message::WorkerStatus:
			int workerId;
			
			workerId = network->getWorkerFromIdentifier(fromId);			
			if (workerId == -1)
				LM_RE(2, ("getWorkerFromIdentifier(%d) failed", fromId));
			status[workerId] = *((Message::WorkerStatusData*) packet->buffer->getData());
			return 0;
			break;
			
		case Message::LoadDataConfirmation:
		{
			// Uptade data and sent a LoadDataConfirmationResponde message
			
			bool error = false;	// By default, no error
			std::string error_message = "No error message";
			
			size_t job_id = data.getNewTaskId();
			data.beginTask(job_id, "Load process from Delilah");
			
			data.addComment( job_id , "Comments for load process...");
			
			const network::LoadDataConfirmation& loadDataConfirmation = packet->message.load_data_confirmation();
				
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
			Packet p;
			network::LoadDataConfirmationResponse * confirmationResponse = p.message.mutable_load_data_confirmation_response();
			confirmationResponse->set_process_id( packet->message.load_data_confirmation().process_id() );
			confirmationResponse->set_error( error );
			confirmationResponse->set_error_message( error_message );
			network->send(this, fromId, Message::LoadDataConfirmationResponse, &p);
		}
		break;

		case Message::StatusRequest:
		{
			Packet p;
			network::StatusResponse *response = p.message.mutable_status_response();
			response->set_title( "Controller" );
			response->set_response( getStatus(packet->message.status_request().command()) );
			network->send(this, fromId, Message::StatusResponse, &p);
			return 0;
			
		}
		break;
			
		case Message::Command:
		{
			// Create a new job with this command
			jobManager.addJob( fromId , packet->message.command().sender_id(), packet->message.command().command() );
			return 0;
		}

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
	
	
#pragma mark Sent messages
	


/* ****************************************************************************
*
* sendWorkerTasks - 
*/
void SamsonController::sendWorkerTasks( ControllerTask *task )
{
	// Send messages to the workers indicating the operation to do (waiting the confirmation from all of them)
		
	for (int i = 0 ; i < network->getNumWorkers() ; i++)
	{
		LM_T(LMT_TASK, ("Sending Message::WorkerTask to worker %d", i));
		sendWorkerTask(i, task->getId(), task);
	}
}	
	


/* ****************************************************************************
*
* sendWorkerTask - 
*/
void SamsonController::sendWorkerTask(int workerIdentifier, size_t task_id, ControllerTask *task  )
{
	// Get status of controller
	Packet p2;

	network::WorkerTask *t = p2.message.mutable_worker_task();
	t->set_task_id(task_id);
	t->set_operation(task->getCommand());
	
	// TODO: Complete with the rest of input / output parameters
		
	LM_T(LMT_TASK, ("Sending Message::WorkerTask to worker %d", workerIdentifier));
	network->send(this,  network->workerGetIdentifier(workerIdentifier) , Message::WorkerTask,  &p2);
}
	
	
#pragma mark Help messages
	


/* ****************************************************************************
*
* getStatus - 
*/
std::string SamsonController::getStatus(std::string command)
{
	
	std::ostringstream output;
		
	output << "** Memory: " << MemoryManager::shared()->getStatus() << std::endl;
	
	output << "** Data Manager:\n" << data.getStatus();
	output << "** Job Manager:\n" << jobManager.getStatus();
	output << "** Task Manager:\n" << taskManager.getStatus();
		
#if 0				
	Endpoint* ep;
	int       workers;
	int       ix;
	int       workersFound;
		
	// Information about each server
	workers      = network->getNumWorkers();
	workersFound = 0;
	ix           = 0;
	do
	{
		ep = network->endpointLookup(3 + ix); /* 0,1 and 2 occupied by ME, LISTEN and CONTROLLER */
		++ix;
		if (ep == NULL)
			continue;
		output << "Worker " << ix << std::endl;
		output << "\t- Cores:\t" << ep->status->cpuInfo.cores << std::endl;
		output << "\t- CPU Load:\t" << ep->status->cpuInfo.load << "%" << std::endl;
		output << "\t- Net (dev 0) Rcv Speed:\t" << ep->status->netInfo.iface[0].rcvSpeed << " bps" << std::endl;
		output << "\t- Net (dev 0) Snd Speed:\t" << ep->status->netInfo.iface[0].sndSpeed << " bps" << std::endl;
		output << "\t- Net (dev 1) Rcv Speed:\t" << ep->status->netInfo.iface[1].rcvSpeed << " bps" << std::endl;
		output << "\t- Net (dev 1) Snd Speed:\t" << ep->status->netInfo.iface[1].sndSpeed << " bps" << std::endl;
		++workersFound;
	} while (workersFound < workers);
#endif				
	
	return output.str();
}
}
