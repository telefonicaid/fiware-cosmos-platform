#include "KVSetBuffer.h"
#include "samson.h"
#include "KVManager.h"

#include "Task.h"                /* Task                                     */
#include "KVQueueWriter.h"       /* Own interface                            */



namespace ss {
	#pragma mark KVQueueWriter

	KVQueueWriter::KVQueueWriter( Task * _task , std::string _queue_name , KVFormat _queue_format )
	{
		// Save the queue name to emit KVSets at the output
		queue_name = _queue_name;
		queue_format = _queue_format;
		
		//Keep task to report with its id
		task = _task;
			
		// No writers by default
		for (int i = 0 ; i < NUM_SETS_PER_STORAGE ; i++)
			writers[i] = NULL;
	}

	KVQueueWriter::~KVQueueWriter()
	{
	}

	void KVQueueWriter::emit( DataInstance * key , DataInstance * value)
	{
		int hash = key->getPartition(NUM_SETS_PER_STORAGE);

		if ( !writers[hash] )
			writers[hash] = new KVSetBuffer(queue_format);
		
		writers[hash]->emit( key , value );

		
		if( writers[hash]->getDataBuffer().size > SAMSON_MAX_KVSET_SIZE )
		{
			// Add this Buffer to the system
			writers[hash]->close();
			
			KVSet *set = KVManager::shared()->addKVSet(task->getId(), writers[hash] );
			KVManager::shared()->addKVSetToKVQueue( task->getId() ,  set , queue_name , hash ) ;
			
			delete writers[hash];
			writers[hash] = NULL;
			
		}
		
	}

	void KVQueueWriter::close()
	{
		for (int hash = 0 ; hash < NUM_SETS_PER_STORAGE ; hash++)
			if( writers[hash] )
			{
				writers[hash]->close();
				KVSet *set = KVManager::shared()->addKVSet( task->getId(), writers[hash] );
				KVManager::shared()->addKVSetToKVQueue( task->getId() , set , queue_name , hash ) ;
				delete writers[hash];
				writers[hash] = NULL;
			}
	}
	
	

}
