#ifndef SAMSON_KV_INFO_H
#define SAMSON_KV_INFO_H

/* ****************************************************************************
*
* FILE            KvInfo.h - information about the content of a Queue
*
*
* Receives the right notification to keep information about this queue
*
*/
#include <sys/types.h>           /* size_t                                   */
#include "KvInfoData.h"          /* KVInfoData                               */



namespace ss
{
	class KVInfo
	{
	public:
	    KVInfoData data;

		// Monitor to hold historics
		ParameterMonitor p_num_kvs;
		ParameterMonitor p_size;
		ParameterMonitor p_size_on_memory;
		ParameterMonitor p_size_on_disk;
		
		
		void sample()
		{
			p_num_kvs.push( (double)data.num_kvs / 1000000.0 );
			p_size.push( (double)data.size  / 1000000.0 );
			p_size_on_disk.push((double)data.size_on_disk  / ( (double)data.size + 1.0)  );
			p_size_on_memory.push( (double)data.size_on_memory  / ((double)data.size + 1.0) );
		}
		
		std::string strKVInfo()
		{
			std::ostringstream	o;
			o << " [ KVS: " << au::Format::string( data.num_kvs );
			o << " S: " << au::Format::string( data.size  );
			o << " MU: " << au::Format::string( data.size_on_memory );
			o << " DU: " << au::Format::string( data.size_on_disk );
			o << " ]" ;
			return o.str();
		}
		
		std::string getParameter( std::string name , int scale )
		{
			if( name == "size" )
				return p_size.str( scale );
			if( name == "num_kvs" )
				return p_size.str( scale );
			if( name == "size_on_memory" )
				return p_size_on_memory.str( scale );
			if( name == "size_on_disk" )
				return p_size_on_disk.str( scale );
			
			return "unknown parameter";
		}
		
		void addKVInfoData( KVInfoData *_data );
		void removeKVInfoData( KVInfoData *_data );
		
	};

}

#endif
