#ifndef SAMSON_KV_QUEUE_H
#define SAMSON_KV_QUEUE_H

/* ****************************************************************************
*
* FILE                 KVQueue.h - list of tasks for KVSet
*
*
* NOTES
*   KVQueue contains a lock mechanism since multiple elements can access tasks
*   over the same KVSet.
*   Example of this concurrency would be a task that is generating a subtask.
*
*   KVQueue:
*     It is basically a "pointer" to a KVStorage.
*     It is use by all elements in the SAMSON Platform
*    
*     The idea is that you can use "nextStorage" only if you retain the queue
*     You can only run "update_to_next_version" if you retain the quue
*/
#include <string>
#include <vector>

#include "samsonTuning.h"        /* NUM_SETS_PER_STORAGE                     */
#include <samson/KVFormat.h>
#include "ParameterMonitor.h"
#include "Format.h"
#include "KVSetVector.h"         /* KVSetVector                              */
#include "KvInfo.h"              /* KVInfo                                   */



namespace ss
{
	class KVSet;
	
	class KVQueue : public KVInfo
	{
	public:
		KVFormat format;
		std::string name;
		
		KVSetVector hashSets[ NUM_SETS_PER_STORAGE ];
		
		std::vector<std::string> distributionQueues;	// Queues to distribute data when received
		
		KVQueue( std::string _name , KVFormat _format ); 

		// Add and Remove a KVSet to this queue
		void addKVSet(KVSet* set, int hash );
		
		KVSetVector * getKVSets( int hash , bool remove_originals );

		void clear();
		void clearHash( int hash );
		
		KVSet *getAnyKVSet();
		
		std::string str();
	};	
}

#endif
