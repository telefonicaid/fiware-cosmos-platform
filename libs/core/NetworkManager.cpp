#include "samsonTuning.h"        /* NUM_SETS_PER_STORAGE                     */
#include "KVManager.h"
#include "KVSet.h"
#include "samson/Operation.h"    /* Operation                                */
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
	
	
	
#pragma mark SimpleNetworkConnection
	
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

