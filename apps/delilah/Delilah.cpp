#include <iostream>				// std::cout ...

#include "logMsg.h"             // lmInit, LM_*
#include "traceLevels.h"        // LMT_*

#include "Macros.h"             // EXIT, ...
#include "Packet.h"				// ss::Packet
#include "network.h"			// NetworkInterface
#include "Endpoint.h"			// Endpoint
#include "CommandLine.h"		// CommandLine
#include "Delilah.h"			// Own interfce
#include "Packet.h"				// ss::Packet

namespace ss {


	void* runThreadConsole(void *p)
	{
		Delilah *d = ((Delilah*)p);
		d->runConsole();
		return NULL;
	}
	
	void* runTest(void *p)
	{
		((Delilah*)p)->test();
		return NULL;
	}
	
	
	void Delilah::run()
	{
		// Run a test in a paralel thread
		pthread_t t;
		pthread_create(&t, NULL, runTest, this);
		
		// Run a console
		pthread_t t2;
		pthread_create(&t2, NULL, runThreadConsole, this);
		

		if( testFlag ) 
		{
			while( !finish )
				sleep(1);
		}
		else
		{
			// Main run_loop to the network interface
			network.run();	
		}
		
		
	}

	void Delilah::test()
	{
		while( ! network.ready() && !finish )
		{
			showMessage("Awaiting network interface ready");
			//LM_T(LMT_READY, ("Awaiting network interface ready"));
			sleep(1);
		}
		
		// Testing to sent something to the controller
		while( !finish )
		{
			Packet p;
			p.message.set_command("Hello there");	// Init the command inside the message
			p.buffer.initPacketBuffer(100);			// Init with the buffer with 100 garbage bytes
			
			network.send( &p, network.controllerGet(), NULL );
			
			sleep(2);
		}		
	}
	
	
	size_t Delilah::sendMessageToContorller(std::string message)
	{
		Packet p;
		p.message.set_code( 0 );
		p.message.set_sender_id( local_id++ );
		p.message.set_command( message );
		
		network.send(&p, network.controllerGet(), this);
		
		return (local_id-1);
	}
	
	void Delilah::receivedMessage( size_t id , std::string message )
	{
		// Todo with the received packet
		std::ostringstream txt;
		txt << "Received message for the packed " << id << ": " << message;
		console->writeOnConsole( txt.str()  );
	}
	

	void Delilah::receive( Packet *p , Endpoint* fromEndpoint )
	{
		// Do something with it
		std::cout << "Delilah received a packet from " << fromEndpoint->str() << " -> " << p->str();
		
		std::string message = p->message.answer();
		size_t id = p->message.sender_id();
		receivedMessage( id , message );
	}
	
	void Delilah::notificationSent( size_t id , bool success )
	{
		// Do something
	}

}

int main(int argc, const char *argv[])
{
	LmStatus  s;
	char*     trace = (char*) "0-255";

	progName = lmProgName((char*) argv[0], 1, false);

	if ((s = lmPathRegister("/tmp/", "DEF", "DEF", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if ((s = lmFdRegister(1, "TYPE: TEXT", "DEF", "controller", NULL)) != LmsOk)
		EXIT(1, ("lmPathRegister: %s", lmStrerror(s)));
	if  ((s = lmInit()) != LmsOk)
		EXIT(1, ("lmInit: %s", lmStrerror(s)));

	if ((argc >= 3) && (strcmp(argv[1], "-t") == 0))
		trace = (char*) argv[2];
	if ((s = lmTraceSet(trace)) != LmsOk)
		EXIT(1, ("lmTraceSet: %s", lmStrerror(s)));

	LM_F(("set trace levels to '%s'", trace));
	for (int ix = 0; ix < 256; ix++)
		LM_T(ix,  ("Testing trace level %d", ix));

	ss::Delilah delilah( argc, argv );
	delilah.run();
}
