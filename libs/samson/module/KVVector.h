#ifndef SAMSON_KV_VECTOR_H
#define SAMSON_KV_VECTOR_H

/* ****************************************************************************
*
* FILE            kVVector.h - vector of KVs
*
*
* NOTES
*   Variables num_kvs and kvs are not suppoused to be modified
*   from outside this class
*/
#include <string.h>              /* strerror                                 */
#include <errno.h>               /* errno                                    */

#include <samson/module/KV.h>                  /* KV                                       */



namespace samson
{
	class KVVector
	{
	public:
		KV*     kvs;
		size_t  num_kvs;
		
		KVVector(size_t _num_kvs)
		{
			num_kvs = _num_kvs;
			kvs     = (KV*) malloc(num_kvs * sizeof(KV));

			/*
			if (kvs == NULL)
				LOG_ERROR(("error allocating KV Vector of %llu items: %s", (unsigned long long int) _num_kvs, strerror(errno)));
			 */
		}
		
		~KVVector()
		{
			free(kvs);
		}		
	};
}

#endif
