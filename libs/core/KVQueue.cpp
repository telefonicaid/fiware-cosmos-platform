#include "samsonTuning.h"        /* NUM_SETS_PER_STORAGE                     */
#include <samson/KVFormat.h>
#include "KVSet.h"               /* KVSet                                    */
#include "KvInfo.h"              /* KVInfo                                   */
#include "KVQueueProcess.h"      /* KVQueueProcess                           */
#include "KVQueue.h"             /* Own interface                            */



namespace ss {
#pragma mark KVQueueProcess
	
	KVQueueProcess::KVQueueProcess( std::string _input_queue , std::string _operation_name , std::vector<std::string> _output_queues )
	{
		operation_name = _operation_name;
		input_queues.push_back( _input_queue );
		output_queues = _output_queues;
	}

#pragma mark KVQueue
	
	
	KVQueue::KVQueue( std::string _name , KVFormat _format )
	{
		name = _name;
		format = _format;
		
		for (int i = 0 ; i < NUM_SETS_PER_STORAGE ; i++)
			hashSets[i].setFormat(format);

	}
	
	void KVQueue::addKVSet( KVSet *set , int hash )
	{
		hashSets[hash].push_back( set );
		set->addQueue( this );
	}
			
	std::string KVQueue::str()
	{
		std::ostringstream	o;
		o << "KVQUEUE: " << name;
		
		size_t total_num_sets = 0 ;
		for (int i = 0 ; i < NUM_SETS_PER_STORAGE ;i++)
			total_num_sets += hashSets[i].size();
		o << "(" << total_num_sets << " sets) ";
		
		o << "(" << format.str() << ") " << strKVInfo();
		return o.str();
	}
	
	KVSetVector * KVQueue::getKVSets( int hash , bool remove_originals )
	{
		KVSetVector*tmp = hashSets[hash].duplicate();
		
		if( remove_originals )
			clearHash(hash);
		
		return tmp;
	}
	
	void KVQueue::clear()
	{
		for (int hash = 0 ; hash < NUM_SETS_PER_STORAGE ; hash++)
			clearHash( hash );
	}
	
	void KVQueue::clearHash( int hash )
	{
		
		// Remove from the KVSets
		for (size_t i = 0 ; i < hashSets[hash].size() ; i++)
			hashSets[hash][i]->removeQueue( this );

		hashSets[hash].erase( hashSets[hash].begin() , hashSets[hash].end() );
		assert( hashSets[hash].size() == 0);
	}
	
	
	KVSet *KVQueue::getAnyKVSet()
	{
		for (int hash = 0 ; hash < NUM_SETS_PER_STORAGE ; hash++)
			if( hashSets[hash].size() > 0)
				return hashSets[hash][0];
		return NULL;
	}
}
