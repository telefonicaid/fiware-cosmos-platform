#ifndef SAMSON_KV_QUEUE_WRITER_H
#define SAMSON_KV_QUEUE_WRITER_H

#include "samson_lib.h"
#include "samson.h"



namespace ss {
	
	class Operation;
	class KVQueue;	
	class KVSet;
	class KVQueueMonitor;
	class KVSetBuffer;
	class Task;
	

	
	/**
	 Class Used to generate content to a particular queue
	 */
	
	class KVQueueWriter : public KVWriter
	{
		
	public:
		
		KVSetBuffer* writers[NUM_SETS_PER_STORAGE];		//!< Writers to emit key-values
		std::string queue_name;							//!< Name of the target queue
		KVFormat queue_format;							//!< Format of this queue ( to create writers to it)
		Task *task;
		
		KVQueueWriter( Task *task ,  std::string queue_name , KVFormat queue_format );
		~KVQueueWriter();
		
		virtual void emit( DataInstance * key , DataInstance * value);
		
		void close();
	};
}

#endif
