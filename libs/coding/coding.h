#ifndef _H_CODING
#define _H_CODING

/* ****************************************************************************
*
* FILE                 coding.h - 
*
*/
#include <samson/Data.h>

#define KV_BUFFER_SIZE			1024*1024*256
#define KV_BUFFER_MAX_NUM_KVS	 1024*1024*64
#define KV_MAX_SIZE				 	  64*1024
#define KV_NUM_HASHGROUPS			  64*1024	

#define KV_HASH_GROUP_VECTOR_SIZE	(sizeof(ss_hg_info)*KV_NUM_HASHGROUPS)


namespace ss {

	/**
	 
	 [ SIZE FOR EACH HASH GROUP ] [ DATA ]
	 
	 -- Sizes for each hash-group
	 -- Data buffer with all the key-values
	 */
	
	typedef unsigned short ss_hg;		// Hashgroup identifier			(16bits)
	typedef unsigned int ss_hg_size;	// Size of a hashgroup			(32bits)
	typedef unsigned int ss_hg_kvs;		// Num KVs inside a hashgroup	(32bits)

	typedef struct 
	{
		ss_hg_size size;
		ss_hg_kvs kvs;
	} ss_hg_info;						// Struct containing information about size / num_kvs

}

#endif
