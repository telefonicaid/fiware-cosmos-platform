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


namespace ss {

	SamsonController::SamsonController(int arg, const char *argv[],  NetworkInterface *_network) : 	jobManager(this) , taskManager(this)
	{
		int          port;								// Local port where this controller listen
		
		network = _network;
		network->setPacketReceiverInterface(this);
		
		std::string  trace;
		
		// Parse input command lines
		au::CommandLine commandLine;
		commandLine.parse(arg, argv);
		
		commandLine.set_flag_int("port",      SAMSON_CONTROLLER_DEFAULT_PORT);
		commandLine.set_flag_string("setup",  SAMSON_SETUP_FILE);
		commandLine.set_flag_string("t",      "255");
		commandLine.set_flag_boolean("r");
		commandLine.set_flag_boolean("w");
		
		commandLine.parse(arg, argv);
		
		port			= commandLine.get_flag_int("port");
		lmReads			= commandLine.get_flag_bool("r");
		lmWrites		= commandLine.get_flag_bool("w");
		
		
		// Load setup
		LM_T(LMT_CONFIG, ("calling loadSetup"));			
		//std::vector <Endpoint> workerEndPoints = loadSetup(setupFileName);
		//std::vector <std::string> workerPeers = getworkerPeers(setupFileName);
		
		// Define the endpoints of the network interface
		
		int num_workers = SamsonSetup::shared()->getInt( SETUP_num_workers  , -1);
		assert( num_workers != -1 );
		LM_T(LMT_CONFIG, ("Num workers: %d", num_workers));
		network->initAsSamsonController(port, num_workers);
		//network->initAsSamsonController(myEndPoint, workerEndPoints);
	}	



	void SamsonController::run()
	{
		data.initDataManager(data.getLogFileName());			// Init the data manager
		network->run();											// Run the network interface (blocked)
	}

	int SamsonController::receive(int fromId, Message::MessageCode msgCode, void* dataP, int dataLen, Packet* packet)
	{
		au::CommandLine     cmdLine;

		switch (msgCode)
		{
			case Message::Command:
			{
				std::ostringstream output;
				
				std::string c = packet->message.command().command();	
				cmdLine.parse(c);
				
				if (cmdLine.get_num_arguments() == 0)
					LM_RE(1, ("No args ..."));
				
				if (cmdLine.get_argument(0) == "help")
				{
					getHelp( output , c );
					sendDelilahAnswer(packet->message.command().sender_id(), fromId, false, true, output.str());
					return 0;
				}
				
				// Try to schedule a job
				jobManager.addJob( fromId , packet->message.command().sender_id(), packet->message.command().command() );
					
				return 0;
			}

			break;

		case Message::WorkerTaskConfirmation:
			taskManager.notifyWorkerConfirmation(fromId, packet->message.worker_task_confirmation());
			break;

		case Message::WorkerStatus:
			int workerId;

			workerId = network->getWorkerFromIdentifier(fromId);			
			if (workerId == -1)
				LM_RE(2, ("getWorkerFromIdentifier(%d) failed", fromId));
			status[workerId] = *((Message::WorkerStatusData*) dataP);
			break;
			
		default:
			LM_X(1, ("msg code '%s' not treated ...", messageCode(msgCode)));
			break;
		}

		return 0;
	}

	void SamsonController::notificationSent(size_t id, bool success)
	{
		// Do something
	}
	
	void SamsonController::notifyWorkerDied( int worker )
	{
		// What to do when a worker died
	}
	
	
#pragma mark Sent messages
	
	
	void SamsonController::sendDelilahAnswer(size_t sender_id, int dalilahIdentifier, bool error, bool finished, std::string answer_message)
	{
		
		// Get status of controller
		Packet p2;
		
		network::CommandResponse *response = p2.message.mutable_command_response();
		response->set_response(answer_message);
		response->set_error(error);
		response->set_finish(finished);
		response->set_sender_id(sender_id);
		
		network->send(this, dalilahIdentifier, Message::CommandResponse, NULL, 0, &p2);
		
	}
	


	void SamsonController::sendWorkerTasks( ControllerTask *task )
	{
		// Send messages to the workers indicating the operation to do (waiting the confirmation from all of them)
		
		for (int i = 0 ; i < network->getNumWorkers() ; i++)
		{
			LM_T(LMT_TASK, ("Sending Message::WorkerTask to worker %d", i));
			sendWorkerTask(i, task->getId(), task);
		}
	}	
	


	void SamsonController::sendWorkerTask(int workerIdentifier, size_t task_id, ControllerTask *task  )
	{
		// Get status of controller
		Packet p2;

		network::WorkerTask *t = p2.message.mutable_worker_task();
		t->set_task_id(task_id);
		t->set_operation(task->getCommand());

		// TODO: Complete with the rest of input / output parameters
		
		LM_T(LMT_TASK, ("Sending Message::WorkerTask to worker %d", workerIdentifier));
		network->send(this,  network->workerGetIdentifier(workerIdentifier) , Message::WorkerTask, NULL, 0, &p2);
	}
	
	
#pragma mark Help messages
	

	void SamsonController::getHelp( std::ostringstream &output , std::string &c)
	{
		au::CommandLine cmdLine;
		cmdLine.set_flag_string("format" , "plain" );

		cmdLine.parse(c);
		std::string format = cmdLine.get_flag_string("format");
		
		if( cmdLine.get_argument(1) == "status" )
			getStatus( output );					// General status command
		else if ( cmdLine.get_argument(1) == "queues" )
			output << data.getQueuesStr(format );
		else if ( cmdLine.get_argument(1) == "modules" )
			output << modulesManager.showModules();
		else
			output << "No help for this";
	}
	
	void SamsonController::getStatus(std::ostringstream &output)
	{
		
		output << "Status of Controller" << std::endl;			
		output << "== ************** ==" << std::endl;
		output << data.status();
		output << taskManager.status();
		
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
		
	}

	
}
