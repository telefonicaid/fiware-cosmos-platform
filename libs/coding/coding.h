#ifndef _H_CODING
#define _H_CODING

/* ****************************************************************************
*
* FILE                 coding.h - 
*
*/
#include <samson/Data.h>
#include "Format.h"				// au::Format
#include "samson/KVFormat.h"	// ss::KVFormat

/*
#define KV_BUFFER_SIZE			1024*1024*256									
#define KV_BUFFER_MAX_NUM_KVS	 1024*1024*64									
 */

#define KV_MAX_SIZE					64*1024														// Max size for an individual key-value
#define KV_MAX_FILE_SIZE			1024*1024*1024												// 1 GB max file size
#define KV_NUM_HASHGROUPS			64*1024														// Number of hash-groups
#define KV_HASH_GROUP_VECTOR_SIZE_NETWORK	(sizeof(NetworkKVInfo)*(KV_NUM_HASHGROUPS+1))		// Size of the structure of every network message ) 
#define KV_HASH_GROUP_VECTOR_SIZE_FILE		(sizeof(FileKVInfo)*(KV_NUM_HASHGROUPS+1))			// Size of the structure of every file


#define KV_TOTAL_FILE_HEADER_SIZE sizeof( FileHeader ) + KV_HASH_GROUP_VECTOR_SIZE_FILE	// Size of the structure of every file

namespace ss {

	/**
	 
	 [ TOTAL SIZE ] [ SIZE FOR EACH HASH GROUP ] [ DATA ]
	
	*/


	// Unsigned types with different lengths in bits
	typedef size_t uint64; 
	typedef unsigned int uint32;
	typedef unsigned short uint16;
	typedef unsigned char uint8;
	
	typedef uint16 ss_hg;	// Hashgroup identifier			(16bits)
	typedef uint16 ss_kv_size;	// Size for a particular KV
	
	/****************************************************************
	 Network interface
	 ****************************************************************/
	
	typedef uint32 hg_net_size;			// Size of a hashgroup			(32bits)
	typedef uint32 hg_net_kvs;			// Num KVs inside a hashgroup	(32bits)

	struct NetworkKVInfo
	{
		hg_net_kvs kvs;		// Total number of kvs
		hg_net_size size;	// Total size
		
		void init()
		{
			kvs = 0;
			size = 0;
		}
		
		void update( uint32 _kvs , uint32 _size)
		{
			kvs += _kvs;
			size += _size;
		}
		
		std::string str()
		{
			std::ostringstream o;
			o <<  "( " << au::Format::string( kvs ) << "kvs in " << au::Format::string( size ) << "bytes )";  
			return o.str();
		}
		
	};	
	
	/****************************************************************
	 File interface
	 ****************************************************************/
	
	typedef uint32 hg_file_size;		// Size of a hashgroup			(32bits)
	typedef uint32 hg_file_kvs;			// Num KVs inside a hashgroup	(32bits)
	
	struct FileKVInfo
	{
		hg_file_kvs kvs;	// Total number of kvs
		hg_file_size size;	// Total size
		
		void init()
		{
			kvs = 0;
			size = 0;
		}
		
		void update( uint64 _kvs , uint64 _size)
		{
			kvs += _kvs;
			size += _size;
		}
		
		std::string str()
		{
			std::ostringstream o;
			o <<  "( " << au::Format::string( kvs ) << "kvs in " << au::Format::string( size ) << "bytes )";  
			return o.str();
		}
		
	};	

	/****************************************************************
	 File interface
	 ****************************************************************/
	
	struct FileHeader
	{
		int magic_number;
		char keyFormat[100];
		char valueFormat[100];
		
		void init( KVFormat format)
		{
			magic_number =  4652783;
			snprintf(keyFormat, 100, "%s", format.keyFormat.c_str());
			snprintf(valueFormat, 100, "%s", format.valueFormat.c_str());
		}
		
		KVFormat getFormat()
		{
			return KVFormat( keyFormat , valueFormat );
		}
		
		bool check()
		{
			return ( magic_number == 4652783);
		}
		
	};
	
	
}

#endif
