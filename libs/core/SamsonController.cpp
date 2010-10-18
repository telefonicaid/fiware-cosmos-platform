#include <iostream>				// std::cout ...

#include "logMsg.h"             // lmInit, LM_*

#include "Macros.h"             // EXIT, ...
#include "Packet.h"				// ss::Packet
#include "Network.h"			// NetworkInterface
#include "Endpoint.h"			// EndPoint
#include "CommandLine.h"		// CommandLine
#include "SamsonController.h"	// own interface ss::SamsonController
#include "ModulesManager.h"		// ss:ModulesManager
#define LMT_READY 21	


namespace ss {

	
	void* runTestSamsonController(void *p)
	{
		((SamsonController*)p)->test();
		return NULL;
	}
	
	void SamsonController::run()
	{
		pthread_t p;
		pthread_create(&p, NULL, runTestSamsonController, this);
		network->run();
	}
	
	void SamsonController::test()
	{
	
		// Testing receiving packets and sending to all the workers
		
		while( ! network->ready() )
		{
			LM_T(LMT_READY, ("Awaiting network interface ready"));
			sleep(5);
		}

		LM_T(LMT_READY, ("*********** READY !!! ***********"));
		return;

		while( true )
		{
			
			std::vector<Endpoint> workers = network->samsonWorkerEndpoints();
			
			Packet p;
			p.message.set_command("Hello there from controller");	// Init the command inside the message
			p.buffer.initPacketBuffer(200);							// Init with the buffer with 100 garbage bytes
			
			for (std::vector<Endpoint>::iterator e = workers.begin() ; e != workers.end() ; e++)
				network->send(&p, &*e, NULL);
			
			sleep(1);
		}
		
	}
	
	
	void SamsonController::sendDalilahAnswer( size_t sender_id ,  Endpoint *dalilahEndPoint , bool error , std::string answer_message  )
	{
		// Get status of controller
		Packet p2;
		p2.message.set_code( 0 );
		p2.message.set_answer( answer_message );
		p2.message.set_error( error );
		p2.message.set_sender_id( sender_id );
		network->send(&p2, dalilahEndPoint, this);
	}
	
	void SamsonController::receive( Packet *p , Endpoint* fromEndPoint )
	{
		
		au::CommandLine cmdLine;
		cmdLine.parse( p->message.command() );
		std::ostringstream output;					// General output string buffer
		
		if( cmdLine.get_num_arguments() == 0)
			return;

		// General status command
		if( cmdLine.get_argument(0) == "status" )
		{
			output << "Status of controller" << std::endl;			
			output << "====================" << std::endl;
			output << data.status();
			
			
			// Get status of controller
			sendDalilahAnswer( p->message.sender_id() , fromEndPoint , false , output.str() );
			return;
		}
		
		
		// Add queue command
		if ( cmdLine.get_argument(0) == "add_queue")
		{
			if( cmdLine.get_num_arguments() < 4 )
			{
				output << "Usage: add_queue name <keyFormat> <valueFormat>";
				sendDalilahAnswer( p->message.sender_id() , fromEndPoint , true , output.str() );
				return;
			}
			
			std::string name = cmdLine.get_argument( 1 );
			std::string keyFormat= cmdLine.get_argument( 2 );
			std::string	valueFormat = cmdLine.get_argument( 3 );
			
			if( !modulesManager.checkData( keyFormat ) )
			{
				output << "Unsupported data format " + keyFormat + "\n";
				sendDalilahAnswer( p->message.sender_id() , fromEndPoint , true , output.str() );
				return;
			}
			
			if( !modulesManager.checkData( valueFormat ) )
			{
				output << "Unsupported data format " + valueFormat + "\n";
				sendDalilahAnswer( p->message.sender_id() , fromEndPoint , true , output.str() );
				return;
			}
			
			data.addQueue( name , KVFormat::format( keyFormat , valueFormat ) );
			output << "OK\n";
			sendDalilahAnswer( p->message.sender_id() , fromEndPoint , false , output.str() );
			return;
		}		
	}

	void SamsonController::notificationSent( size_t id , bool success )
	{
		// Do something
	}
	
	
}



