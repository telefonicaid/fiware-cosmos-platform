#include "Packet.h"              /* Packet                                   */
#include "TaskManager.h"         /* TaskManager::shared()->runCommand        */
#include "NetworkConnection.h"   /* Own interface                            */



namespace ss {

	void NetworkConnection::run_thread()
	{
		try
		{
			while ( !finished )
			{  
				Packet p;
				
				if( p.read( this ) )
				{
					processMessage( p );
					
					// Free the KVSet if any ( this set is created at the "read" )
					//if ( p.set )
					//	kvManager->releaseSystemTemporalKVSet(p.set);
				}
				else
					finished = true;
				
			}
		}
		catch ( au::SocketException& )
		{
			// End of the connection for some reasong
			finished = true;
		}
	}	

	void NetworkConnection::processMessageCommand( Packet &p )
	{
		if( !p.message.has_command() )
		{
			
			std::cout << "Error: Missing command" << std::endl;
			return;
		}
		
		CommandResponse response = TaskManager::shared()->runCommand( NULL , p.message.command() , false );
		//taskManager->addTask( new Task( p.message.command() ) );

		// Write back a message with the answer
		Packet p2;
		p2.message.set_code(2);
		p2.message.set_answer( response.message );
		p2.message.set_error( response.error );
		p2.message.set_description( std::string("answer to: " +  p.message.command() ) );

		p2.send(this);
		
	}
	
	void NetworkConnection::processMessageForIncommingData( Packet &p )
	{
		if( !p.message.has_queue() )
		{
			std::cout << "Error: Missing  queue" << std::endl;
			return;
		}
		
		std::string queue_name = p.message.queue();
	}
	
	
	void NetworkConnection::processMessage( Packet &p )
	{
		// Processing this message
		switch (p.message.code() ) {
			case 0:
				processMessageForIncommingData(p);
				break;
			case 1:
				processMessageCommand(p);
				break;
			default:
				std::cout << "Unknown packet: " << p.message.description() << std::endl;
				break;
		}
		
	}
}
