#ifndef KV_WRITER_H
#define KV_WRITER_H

#include "DataInstance.h"        /* DataInstance                             */



namespace ss {

	class KVWriter
	{
	public:
		virtual void emit(DataInstance* key, DataInstance* value) = 0;
	};
}

#endif
