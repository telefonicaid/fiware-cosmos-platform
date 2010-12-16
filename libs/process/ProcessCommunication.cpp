

#include "ProcessCommunication.h"		// Own interface

namespace ss {


	/** 
	 Blocking read
	 */

	int ProcessCommunication::_read( network::ProcessMessage &p)
	{
		int length;
		char buffer[1000];
		
		// Read the length
		int ans = ::read( rFd , &length , sizeof(int) );
		if ( ans != sizeof(int) )
			return -1;
		
		// Read the content of the buffer
		ans = ::read( rFd , buffer , length );
		if ( ans != length)
			return -1;
		
		// Parse the buffer
		p.ParsePartialFromArray( buffer , length );
		
		return 0;
	}

	/**
	 Write
	 */
	int ProcessCommunication::_write(network::ProcessMessage &p)
	{
		int length = p.ByteSize();
		char buffer[1000];
		assert( length <= 1000);
		
		p.SerializeToArray( buffer , length );
		
		int ans = ::write( wFd , &length , sizeof(int) );
		
		if( ans != sizeof(int) )
			return -1;
			
		ans = ::write( wFd , buffer , length );
		
		if( ans != length )
			return -1;
		
		
		return 0;
			
	}
	
}



