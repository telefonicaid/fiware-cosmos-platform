

#include "SamsonWorker.h"		// Own interfce
#include "packet.h"				// ss::Packet


namespace ss {


	void SamsonWorker::run()
	{
		
		while( ! network.ready() )
		{
			std::cout << "Waiting for the network interface to be ready" << std::endl;
			sleep(1);
		}
		
		// Testing to sent something to the controller
		while( true )
		{
			ss::Packet p;
			p.message.set_command("Hello there");	// Init the command inside the message
			p.buffer.initPacketBuffer(100);			// Init with the buffer with 100 garbage bytes
			
			network.send( &p , network.controller() , NULL );
			
			sleep(2);
		}
		
	}


	void SamsonWorker::receive( Packet *p , EndPoint fromEndPoint )
	{
		// Do something with it
		std::cout << "Packet received from " << fromEndPoint.str() << " -> " << p->str();
	}
	
	void SamsonWorker::notificationSent( size_t id , bool success )
	{
		// Do something
	}
	
	
}