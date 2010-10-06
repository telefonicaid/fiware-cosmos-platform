#include "samsonTuning.h"        /* NUM_SETS_PER_STORAGE                     */
#include "KVManager.h"
#include "KVSet.h"
#include <samson/Operation.h>
#include "ModulesManager.h"
#include "TaskManager.h"
#include "Task.h"                /* Task                                     */
#include "NetworkManager.h"      /* Own interface                            */



namespace ss {

	NetworkManager	*networkManager = NULL;

	NetworkManager* NetworkManager::shared()
	{
		if( !networkManager )
			networkManager = new NetworkManager();
		
		return networkManager;
	}
	
	
	bool packet::read(au::Socket *socket)
	{
		message_header header;
		char * message_buffer;
		
		// Read header
		ssize_t size = socket->recv_waiting((char*)&header , sizeof( message_header ) );
		
		message_buffer = (char*) malloc( header.message_length );
		
		if( size != sizeof( message_header) )
			return false;
		
		// Message ( protocol buffers )
		socket->recv_waiting(message_buffer , header.message_length );
		
		// Parse from the buffer
		message.ParseFromArray(message_buffer, header.message_length);
		
		if( header.buffer_length > 0 )
		{
			// Create a txt based document
			KVSetTxTBuffer *kvSetBuffer = new KVSetTxTBuffer( header.buffer_length );
			
			// Get the buffer where income socket data is stored
			char *buffer = kvSetBuffer->getTxtDataBuffer();

			// Read data into the dedicated buffer
			socket->recv_waiting( buffer  , header.buffer_length );
			
			kvSetBuffer->close();
			
			KVSet *set = KVManager::shared()->addKVSet( 0, kvSetBuffer );
			KVManager::shared()->addKVSetToKVQueue( 0, set , message.queue() , rand()%NUM_SETS_PER_STORAGE );
			
			delete kvSetBuffer;
		}
		
		free( message_buffer );
		
		return true;
	}
	
	void packet::send( au::Socket *socket  )
	{
		message_header header;
		header.message_length = message.ByteSize();
		header.buffer_length = dataBuffer.size;

		// serialize the message
		char *message_buffer = (char*) malloc( header.message_length );		
		
		assert(message.IsInitialized());
		bool ans = message.SerializeToArray(message_buffer, header.message_length );
		if (!ans)
		{
			LOG_ERROR(("error serializing message"));
			assert(0);
		}

		// Send content to the socket
		socket->send( (char*)&header, sizeof( header ) );
		socket->send(message_buffer , header.message_length );
		if( dataBuffer.size > 0 )
			socket->send( dataBuffer.buffer  , dataBuffer.size );

		
		free( message_buffer );
	}	
	
	void NetworkConnection::run_thread()
	{
		try
		{
			while ( !finished )
			{  
				packet p;
				
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

	void NetworkConnection::processMessageCommand( packet &p )
	{
		if( !p.message.has_command() )
		{
			
			std::cout << "Error: Missing command" << std::endl;
			return;
		}
		
		CommandResponse response = TaskManager::shared()->runCommand( NULL , p.message.command() , false );
		//taskManager->addTask( new Task( p.message.command() ) );

		// Write back a message with the answer
		packet p2;
		p2.message.set_code(2);
		p2.message.set_answer( response.message );
		p2.message.set_error( response.error );
		p2.message.set_description( std::string("answer to: " +  p.message.command() ) );

		p2.send(this);
		
	}
	
	void NetworkConnection::processMessageForIncommingData( packet &p )
	{
		if( !p.message.has_queue() )
		{
			std::cout << "Error: Missing  queue" << std::endl;
			return;
		}
		
		std::string queue_name = p.message.queue();
	}
	
	
	void NetworkConnection::processMessage( packet &p )
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
	

#pragma mark SimpleNetworkConnectin
	
	int posReturn( char *buffer , int length )
	{
		for (int i = 0 ; i < length ; i++)
			if( ( buffer[i] == '\n' ) || ( buffer[i] == '\r' ) )
				return i;
		
		return -1;
	}
	
	void SimpleNetworkConnection::run_thread()
	{
		try
		{
			
			int pos=0;				// Position inside the buffer to write
			char buffer[1024];		// Buffer to read commands
			
			while ( !finished )
			{  
				// Read something
				
				ssize_t b = recv (  buffer+pos , 1024 - pos  );
				pos += b;
				

				// Position of the end of the command
				int pos_return = posReturn( buffer , pos );

				if( pos_return > 0)
				{
					buffer[pos_return] = '\0';
					std::string command = std::string( buffer );
					
					// Skip returns
					while( (pos_return < pos) && ( (buffer[pos_return] == '\0') || (buffer[pos_return] == '\n') || (buffer[pos_return] == '\r' )) )
						pos_return++;	 

					if( pos_return < pos )
						memmove(buffer, buffer+pos_return, pos-pos_return);
					pos -= pos_return;
					
					// Run this command
					if( command == "quit" )
					{
						// Close connection
						close();
						return;
					}
					
					CommandResponse response = TaskManager::shared()->runCommand( NULL ,command , false );
					
					// Send the answer back
					send( response.message.c_str() , response.message.length() );
	
					AUCommandLine cmdLine;
					cmdLine.set_flag_boolean("quit");
					cmdLine.parse( command );
					if( cmdLine.get_flag_bool("quit") )
					{
						close();
						return;
					}
					
					
				}
				
				//ssize_t recv_waiting ( char *buffer , size_t max_length ) const;
				//ssize_t send ( char *buffer , size_t length ) const;
				
				// Write something back
			}
		}
		catch ( au::SocketException& )
		{
			// End of the connection for some reasong
			finished = true;
		}
	}	
	
	
#pragma mark SIMPLENETWORKMANAGER
	
	SimpleNetworkManager* simpleNetworkManager = NULL;
	
	SimpleNetworkManager* SimpleNetworkManager::shared()
	{
		if( !simpleNetworkManager )
			simpleNetworkManager = new SimpleNetworkManager();
		return simpleNetworkManager;
	}	

	
	
}

