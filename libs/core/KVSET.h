#ifndef SAMSON_KV_SET_H
#define SAMSON_KV_SET_H

/* ****************************************************************************
*
* FILE               KVSET.h - basic type to work with key-values
*
*
*
*
*/
#include <sys/types.h>           /* size_t                                   */

#include "KV.h"                  /* KV                                       */



namespace ss
{	
	typedef struct KVSET
	{
		KV**    kvs;       // Vector containing Key-values (pointers to row data and hash code)
		size_t  num_kvs;
	} KVSET;
}

#endif
