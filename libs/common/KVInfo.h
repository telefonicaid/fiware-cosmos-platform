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
    
    KVInfo( std::vector<KVInfo>& info)
      {
	size=0;
	kvs=0;
	for (size_t i = 0 ; i < info.size() ; i++)
	  {
	    size += info[i].size;
	    kvs += info[i].kvs;
	  }
      }
    
    std::string str()
      {
		 std::ostringstream o;
		 o <<au::Format::string( kvs ) << "kvs in " <<  au::Format::string( size ) << " bytes ";
		 return o.str();
      }
  };
};

#endif
