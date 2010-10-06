#ifndef SAMSON_KV_INFO_DATA_H
#define SAMSON_KV_INFO_DATA_H

/* ****************************************************************************
*
* FILE            KvInfoData.h - basic information to add or remove counters
*
*
*
*
*/
#include <sys/types.h>           /* size_t                                   */



namespace ss
{
	class KVInfoData
	{
	public:
		size_t num_kvs;
		size_t size;
		size_t size_on_memory;
		size_t size_on_disk;
		
		KVInfoData()
		{
			num_kvs         = 0;
			size            = 0;
			size_on_memory  = 0;
			size_on_disk    = 0;
		}
		
		KVInfoData(size_t _num_kvs, size_t _size, size_t _size_on_memory, size_t _size_on_disk)
		{
			num_kvs         = _num_kvs;
			size            = _size;
			size_on_memory  = _size_on_memory;
			size_on_disk    = _size_on_disk;
		}
		
		void add( KVInfoData *data )
		{
			num_kvs         += data->num_kvs;
			size            += data->size;
			size_on_memory  += data->size_on_memory;
			size_on_disk    += data->size_on_disk;
		}
		
		void remove( KVInfoData *data )
		{
			num_kvs         -= data->num_kvs;
			size            -= data->size;
			size_on_memory  -= data->size_on_memory;
			size_on_disk    -= data->size_on_disk;
		}
		
	};
}

#endif
