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



namespace samson
{	
	typedef struct KV
	{
		char*   key;		// Pointer to key data
		char*   value;		// Pointer to value data
		int input;			// Index of the input ( Used internally in the platform for reduce operations )
	} KV;
}

#endif
