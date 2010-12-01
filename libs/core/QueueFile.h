
#ifndef _H_QUEUE_FILE
#define _H_QUEUE_FILE
#include "samson.pb.h"

namespace ss {

	class QueueFile
	{
		
	public:
		
		std::string fileName;
		int worker;
		KVInfo info;
		
		
		QueueFile( std::string& _fileName , int _worker , KVInfo _info )
		{
			fileName = _fileName;
			worker = _worker;
			info = _info;
		}
	
		void fill( network::File *file )
		{
			file->set_name( fileName );
			file->set_worker( worker );
			
			network::KVInfo *_info = file->mutable_info();
			_info->set_kvs( info.kvs );
			_info->set_size( info.size );
		}
		
	};
	
}

#endif
