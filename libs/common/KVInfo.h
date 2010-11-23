#ifndef _H_KV_INFO
#define _H_KV_INFO

#include <cstring>	   // size_t
#include <sstream>	   // std::ostringstream

#include "Format.h"	   // au::Format


namespace ss
{
	class KVInfo  
	{
	public:
		size_t size;
		size_t kvs;

		KVInfo()
		{
			size=0;
			kvs=0;
		}
		
		void init()
		{
			size=0;
			kvs=0;
		}

		KVInfo( size_t _size , size_t _kvs )
		{
			size = _size;
			kvs = _kvs;
		}
		
		void append( KVInfo info )
		{
			size += info.size;
			kvs += info.kvs;
		}
		
		std::string str()
		{
			std::ostringstream o;
			o << au::Format::string( kvs ) << "kvs in " <<  au::Format::string( size ) << " bytes ";
			return o.str();
		}
	};
};

#endif
