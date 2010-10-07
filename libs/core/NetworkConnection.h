#ifndef SAMSON_NETWORK_CONNECTION_H
#define SAMSON_NETWORK_CONNECTION_H

#include <sstream>               /* ostringstream                            */

#include "CommandResponse.h"     /* CommandResponse                          */



namespace ss {

	/**
	 Incoming connection
	 */
	
	class NetworkConnection : public au::ServerSocket
	{
		
	public:
		pthread_t thread;			//!< thread that is working with this connection
		bool finished;				//!< Flag to indicate that it is not finished
		
		NetworkConnection()
		{
			finished = false;
			set_non_blocking(true);	//!< non bloquing threads to be able to see if new messages are comming without bloquing
		}
		
		void processMessage( Packet &p );
		void processMessageForIncommingData( Packet &p );
		void processMessageCommand( Packet &p );
		void run_thread();

		std::string str( )
		{
			std::ostringstream o;
			
			o << "Connection ";
			if( finished )
				o << "(F)";
			
			return o.str();
		}
		
		
	};
}

#endif
