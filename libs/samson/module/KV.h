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
        int key_size;       // Size of the key in bytes
        int value_size;     // Size of the value in bytes 
        
        int pos;  // Debuggin sort algorithm
        
	} KV;
}

#endif
