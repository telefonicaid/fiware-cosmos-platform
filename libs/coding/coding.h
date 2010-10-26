#ifndef _H_CODING
#define _H_CODING

/* ****************************************************************************
*
* FILE                 coding.h - 
*
*/
#include <samson/Data.h>
/*
#define KV_BUFFER_SIZE			1024*1024*256									
#define KV_BUFFER_MAX_NUM_KVS	 1024*1024*64									
 #define KV_MAX_SIZE				 	  64*1024									// Max size for an individual key-value
*/
 
#define KV_MAX_FILE_SIZE				1024*1024*1024							// 1 GB max file size

#define KV_NUM_HASHGROUPS			  64*1024									// Number of hash-groups
#define KV_HASH_GROUP_VECTOR_SIZE	(sizeof(hg_info)*(KV_NUM_HASHGROUPS+1))		// Size of the structure of every file ( and network message ) 

namespace ss {

	/**
	 
	 [ TOTAL SIZE ] [ SIZE FOR EACH HASH GROUP ] [ DATA ]
	 
	 -- Sizes for each hash-group
	 -- Data buffer with all the key-values
	 */
	
	typedef unsigned short ss_hg;	// Hashgroup identifier			(16bits)
	typedef size_t hg_size;			// Size of a hashgroup			(64bits)
	typedef size_t hg_kvs;			// Num KVs inside a hashgroup	(64bits)
	
	/**
	 Structure used as header in all files / network data messages
	 Informs about the number of kvs and size used for each hash-group
	 */
	
	typedef struct 
	{
		hg_size size;
		hg_kvs kvs;
	} hg_info;	

	
	
	
}

#endif
