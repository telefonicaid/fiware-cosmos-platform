#ifndef SAMSON_KV_H
#define SAMSON_KV_H

/* ****************************************************************************
*
* FILE               KV.h - basic type to work with key-values
*
*
* It consists in a hash code of the key and pointers to the key and value raw data
*
*
*/
#include <sys/types.h>           /* size_t                                   */



namespace ss
{	
	typedef struct KV
	{
		size_t  key_size;
		size_t  value_size;
		char*   key;
		char*   value;
	} KV;
}

#endif
