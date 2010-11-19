
#include "Queue.h"		// Own interface
#include "QueueFile.h"	

namespace ss
{
	void Queue::addFile( int worker, std::string _fileName , KVInfo info )
	{
		// Upodate global info
		_info.append( info );
		
		// Insert file in the local list
		files.push_back( new QueueFile( _fileName , worker , info) ); 	
	}

}