#ifndef SAMSON_KV_SET_STRUCT_H
#define SAMSON_KV_SET_STRUCT_H

/* ****************************************************************************
*
* FILE               KVSetStruct.h - basic type to work with key-values
*
*
*
*
*/
#include <sys/types.h>           /* size_t                                   */

#include <samson/module/KV.h>                  /* KV                                       */



namespace samson
{	
	typedef struct KVSetStruct
	{
		KV**    kvs;			// Vector containing Key-values (pointers to row data and hash code)
		size_t  num_kvs;		// Number of key-values	
		
	} KVSetStruct;
	
	
	
	
}

#endif