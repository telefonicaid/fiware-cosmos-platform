#ifndef SAMSON_KV_QUEUE_H
#define SAMSON_KV_QUEUE_H

#include <string>
#include <vector>

#include "samson.h"
#include "KVFormat.h"
#include "ParameterMonitor.h"
#include "Format.h"
#include "KvInfo.h"              /* KVInfo                                   */



/**
 Class containing a list of tasks for KVSet.
 It also contains a lock mechanish since multiple elements can tasks over the same KVSet
 Example of this concurrency would be a task that is generating a subtask
 */

namespace ss {

	class Operation;
	class KVQueue;	
	class KVSet;
	class KVQueueMonitor;
	
	/**
	 Queue process to automatically process content of a queue
	 */
	
	class KVQueueProcess
	{
		
	public:
		
		KVQueueProcess( std::string _input_queue , std::string _operation_name , std::vector<std::string> _outputs_queues );
		
		std::string operation_name;					//!< Operation to run		
		std::vector<std::string> input_queues;		//!< Input queues ( queue.process )
		std::vector<std::string> output_queues;		//!< Output queues ( queues )
		
		
		std::string str()
		{
			std::ostringstream o;
			o << "Operation: " << operation_name << " ";
			
			o << "Inputs: ";
			for ( size_t i = 0 ; i < input_queues.size() ; i++)
				o << input_queues[i] << " ";
			
			o << "Outputs: ";
			for ( size_t i = 0 ; i < output_queues.size() ; i++)
				o << output_queues[i] << " ";
			
			return o.str();
		}
	};
	

	/**
	 A vector of KVSets
	 Set of KVSets that can add and remove easyly
	 */
	
	class KVSetVector : public std::vector<KVSet*>
	{
	public:
				
		KVFormat format;
		
		KVSetVector( KVFormat _format )
		{
			format = _format;
		}
	private:
		friend class KVQueue;
		
		KVSetVector(  )
		{
		}
		
		void setFormat( KVFormat _format)
		{
			format = _format;
		}
		
		
	public:		
		KVSetVector* duplicate( )
		{
			KVSetVector* tmp = new KVSetVector( format );
			tmp->insert( tmp->end() , begin() , end() ); 
			
			return tmp;
		}
		
		size_t getNumKVs();
	};			
	


	
	/*
		KVQueue:
	 
			It is basically a "pointer" to a KVStorage.
			It is use by all elements in the SAMSON Platform
	 
			The idea is that you can use "nextStorage" only if you retain the queue
			You can only run "update_to_next_version" if you retain the quue
	 
	 */
	
	class KVQueue : public KVInfo
	{
		
	public:
		
		KVFormat format;
		std::string name;
		
		KVSetVector hashSets[ NUM_SETS_PER_STORAGE ];
		
		std::vector<std::string> distributionQueues;	// Queues to distribute data when received
		
		KVQueue( std::string _name , KVFormat _format ); 
		
		
		// Add and Remove a KVSet to this queue
		void addKVSet( KVSet *set , int hash );

		
		KVSetVector * getKVSets( int hash , bool remove_originals );

		void clear();

		void clearHash( int hash );
		
		
		KVSet *getAnyKVSet();
		
		std::string str();
		
	};	
	
}

#endif
