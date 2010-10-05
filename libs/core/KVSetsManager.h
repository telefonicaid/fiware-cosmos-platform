#ifndef SAMSON_KV_SETS_MANAGER_H
#define SAMSON_KV_SETS_MANAGER_H

#include <map>
#include "au.h"



namespace ss
{

	class KVSet;
	class KVFormat;
	class MonitoringInfo;
	class KVSetBuffer;
	class KVSetBufferBase;
	class Task;
	
	/**
	 Double linked list of KVSets
	 - Designed for fast reordering of a KVSet*
	 - Esasy extraction of KVSets
	 
	 It has no lock for multiple threads because it is operated by KVManager
	 
	*/

	class KVSetsManager 
	{
		KVSet *top;
		KVSet *bottom;

		size_t current_KVSetId;						//!< Id for the next KVSet
		
	public:
		
		KVSetsManager(  );
		
		std::string showSets();
		
		KVSet* addKVSet( KVSetBufferBase *buffer ,size_t task_id  );												// New KVSet for normal operation
		KVSet* addKVSet( size_t id ,size_t task_id , KVFormat format , size_t num_kvs , size_t size );		// Recover a KVSet that is suppoused to be on disk
		
		KVSet * getKVSet( size_t id );
		
		std::string fileNameForKVSet( size_t id );
		
		
		void extract( KVSet *b );
		void insert( KVSet *b );
		
		void sort( KVSet *b)
		{
			extract(b);
			insert(b);
		}
		
		void insert_on_top( KVSet *b , KVSet *ref );
		void insert_bellow( KVSet *b , KVSet *ref );


		size_t freeUnusedMemory();
		KVSet * findNextToBeReadFromDisk( );
		KVSet * findNextToBeFlushedToDisk( );

		size_t freeMemory( size_t necessary_size ,  KVSet *higherPriorityKVSet );
		
		bool all_on_disk();
		
		
		void linkKVSetVectorToTask(Task *task , std::vector<KVSet*>*sets );
		void unlinkKVSetVectorToTask(Task *task , std::vector<KVSet*>*sets );
		
		bool areResourcesAvailable( Task * task );
		void retainResources(  Task* task );
		void releaseResources(  Task* task );
		
		
	private:
		size_t remove(KVSet *b);	// Internal function to remove a KVSet while iterating the list

	};
}

#endif
