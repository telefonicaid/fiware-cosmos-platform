#ifndef SAMSON_DATA_BUFFER_H
#define SAMSON_DATA_BUFFER_H

/* ****************************************************************************
*
* FILE                  DataBuffer - buffer for message sending
*
*
*/
#include <sys/types.h>           /* size_t                                   */




namespace ss {
	typedef struct DataBuffer
	{
		char *buffer;
		size_t size;
	} DataBuffer;
}

#endif
