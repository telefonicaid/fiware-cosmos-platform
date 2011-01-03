
#ifndef _H_PROCESS_COMMUNICATION
#define _H_PROCESS_COMMUNICATION

#include "samson.pb.h"

namespace ss {


	class ProcessCommunication
	{
	public:

		// My own read and write channels	
		int rFd;     // file descriptor for reading
		int wFd;     // file descriptor for writing
		
		// Simple functions to read / write a code the the other side	
		
		int  _read(network::ProcessMessage& p);
		int  _write(network::ProcessMessage& p);
		void _wait(void);

		
		network::ProcessMessage processMessageWithCode( network::ProcessMessage::Code code )
		{
			network::ProcessMessage p;
			p.set_code( code );
			return p;
		}
		
		void closeFds()
		{
			close( rFd );
			close( wFd );
		}
		
	};

}

#endif
