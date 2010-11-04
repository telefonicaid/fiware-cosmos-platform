#include <iostream>                     // std::cout ...

#include "logMsg.h"                     // lmInit, LM_*
#include "coreTracelevels.h"            // LMT_*
#include "traces.h"                     // Trace definition

#include "Message.h"                    // Message
#include "Macros.h"                     // EXIT, ...
#include "Packet.h"                     // ss::Packet
#include "Network.h"                    // NetworkInterface
#include "Endpoint.h"                   // Endpoint
#include "CommandLine.h"                // CommandLine
#include "WorkerDataManager.h"          // WorkerDataManager
#include "SamsonWorker.h"               // Own interfce



namespace ss {



/* ****************************************************************************
*
* Constructor
*/
SamsonWorker::SamsonWorker(int argC, const char* argV[]) : data(this), taskManager(this)
{
	logInit(argV[0]);
	parseArgs(argC, argV);
}



/* ****************************************************************************
*
* logInit - 
*/
void SamsonWorker::logInit(const char* pName)
{
	LmStatus  s;
	int       logFdIndex;
	
	progName = lmProgName((char*) pName, 1, true);
	
	if ((s = lmPathRegister("/tmp/", "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT", "DEF", &logFdIndex)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if ((s = lmFdRegister(1, "TYPE: TEXT", "DEF", "controller", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if  ((s = lmInit()) != LmsOk)
		EXIT(1, ("lmInit: %s", lmStrerror(s)));

	lmAux((char*) "father");

	// lmFdGet(logFdIndex, &logFd);
}



/* ****************************************************************************
*
* parseArgs - 
*/
void SamsonWorker::parseArgs(int argC, const char* argV[])
{
	au::CommandLine commandLine;
	LmStatus        s;

	commandLine.parse(argC, argV);

	commandLine.set_flag_int("port",           SAMSON_WORKER_DEFAULT_PORT);
	commandLine.set_flag_int("endpoints",      80);
	commandLine.set_flag_int("workers",         5);
	commandLine.set_flag_string("controller",  "no_controller");
	commandLine.set_flag_string("t",           "255");
	commandLine.set_flag_boolean("r");
	commandLine.set_flag_boolean("w");

	commandLine.parse(argC, argV);

	port       = commandLine.get_flag_int("port");
	controller = commandLine.get_flag_string("controller");
	traceV     = commandLine.get_flag_string("t");
	lmReads    = commandLine.get_flag_bool("r");
	lmWrites   = commandLine.get_flag_bool("w");
	endpoints  = commandLine.get_flag_int("endpoints");
	workers    = commandLine.get_flag_int("workers");

	if ((s = lmTraceSet((char*) traceV.c_str())) != LmsOk)
		LM_X(1, ("lmTraceSet: %s", lmStrerror(s)));

	if (controller == "no_controller")
	{
		std::cerr  << "Please specify controller direction with -controller server:port" << std::endl;
		exit(0);
	}
		
	LM_T(LMT_SAMSON_WORKER, ("Samson worker running at port %d controller: %s", port, controller.c_str()));
}



/* ****************************************************************************
*
* endpointMgrSet - 
*/
void SamsonWorker::endpointMgrSet(ss::EndpointMgr*  epMgr)
{
   this->epMgr = epMgr;

   this->epMgr->packetReceiverSet(this);
}



/* ****************************************************************************
*
* networkSet - 
*/
void SamsonWorker::networkSet(NetworkInterface* network)
{
	this->network = network;
	network->initAsSamsonWorker(port, controller);
}



/* ****************************************************************************
*
* Constructor
*/
SamsonWorker::SamsonWorker(int argc, const char* argv[], NetworkInterface *_network) : data(this), taskManager(this)
{
	network = _network;
	network->setPacketReceiverInterface(this);
		
	int          port;
	std::string  controller;
	std::string  trace;
		
	// Parse input command lines
	au::CommandLine commandLine;
	commandLine.parse(argc, argv);
		
	commandLine.set_flag_int("port",           SAMSON_WORKER_DEFAULT_PORT);
	commandLine.set_flag_string("controller", "no_controller");
	commandLine.set_flag_string("t",           "255");
	commandLine.set_flag_boolean("r");
	commandLine.set_flag_boolean("w");
		
	commandLine.parse(argc, argv);
		
	port       = commandLine.get_flag_int("port");
	controller = commandLine.get_flag_string("controller");
	lmReads    = commandLine.get_flag_bool("r");
	lmWrites   = commandLine.get_flag_bool("w");
		
	if (controller == "no_controller")
	{
		std::cerr  << "Please specify controller direction with -controller server:port" << std::endl;
		exit(0);
	}

	LM_T(LMT_SAMSON_WORKER, ("Samson worker running at port %d controller: %s", port, controller.c_str()));

	network->initAsSamsonWorker(port, controller);
}	
	


/* ****************************************************************************
*
* run - 
*/
void SamsonWorker::run()
{
	// Init the data manager
	data.initDataManager(data.getLogFileName());

	assert(network);
	network->run();
}



#if 0
void SamsonWorker::sendWorkerStatus()
{
	Packet                  p;
	network::WorkerStatus*  w = p.message.mutable_worker_status();
	
	// Fill with all data related to data
	data.fillWorkerStatus(w);
	
	// Fill to all data related with task manager
	taskManager.fillWorkerStatus(w);
	
	network->send(this, network->controllerGetIdentifier(), Message::WorkerStatus, NULL, 0, &p);
}
#endif
		


void SamsonWorker::sentConfirmationToController(size_t task_id)
{
	Packet                            p;
	network::WorkerTaskConfirmation*  confirmation = p.message.mutable_worker_task_confirmation();

	confirmation->set_task_id(task_id);
	confirmation->set_error(false);

	network->send(this, network->controllerGetIdentifier(), Message::WorkerTaskConfirmation, NULL, 0, &p);
}
	
	

int SamsonWorker::receive(int fromId, Message::MessageCode msgCode, void* dataP, int dataLen, Packet* packet)
{
	if (msgCode == Message::WorkerTask)
	{
		// A packet with a particular command is received (controller expect to send a confirmation message)
		LM_T(LMT_TASK, ("Got a WorkerTask message"));

		// Process the command in the data manager (right now this is just to test)
		data.beginTask(packet->message.worker_task().task_id());

		data.runOperationOfTask(packet->message.worker_task().task_id(), packet->message.worker_task().command());

		if (taskManager.addTask(packet->message.worker_task()))
		{
			data.finishTask(packet->message.worker_task().task_id());
			
			// Send a confirmation just to test everything is ok
			sentConfirmationToController(packet->message.worker_task().task_id());
			
			// Send update
			// sendWorkerStatus();
		}
	}

	return 0;
}



void SamsonWorker::notificationSent(size_t id, bool success)
{
	// Do something
}
}
