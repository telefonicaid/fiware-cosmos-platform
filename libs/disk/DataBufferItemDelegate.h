#ifndef _H_DATA_BUFFER_DELEGATE
#define _H_DATA_BUFFER_DELEGATE

#include <cstring>			// size_t
#include <string>			// std::string
#include "au_map.h"			// au::map
#include "coding.h"			// ss::hg_info , ss::hg_size 

namespace ss {
	
	
	/** 
	 Delegate class to receive notification that a particular task is finished
	 */
	
	class DataBufferItemDelegate
	{
	public:
		virtual void addFile( size_t task_id , std::string fileName , std::string queue , hg_info info)=0;	// callback to notify that a new file is created for a queue for this process
		virtual void finishDataBufferTask( size_t task_id )=0;								// callback to notify that all the files are on disk now
	};		
	
}

#endif
