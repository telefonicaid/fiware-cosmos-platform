#ifndef SAMSON_KV_QUEUE_H
#define SAMSON_KV_QUEUE_H

#include <string>
#include <vector>

#include "samson.h"
#include "KVFormat.h"
#include "ParameterMonitor.h"
#include "Format.h"



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
			KVSetVector*tmp = new KVSetVector( format );
			tmp->insert( tmp->end() , begin() , end() ); 
			
			return tmp;
		}
		
		size_t getNumKVs();
		
	};			
	


	/**
	 Basic information to add of remove counters
	 */
	
	class KVInfoData
	{
	public:
		
		size_t num_kvs;
		size_t size;
		size_t size_on_memory;
		size_t size_on_disk;
		
		KVInfoData()
		{
			num_kvs = 0;
			size = 0;
			size_on_memory = 0;
			size_on_disk = 0;
		}
		
		KVInfoData( size_t _num_kvs , size_t _size , size_t _size_on_memory , size_t _size_on_disk )
		{
			num_kvs = _num_kvs;
			size = _size;
			size_on_memory = _size_on_memory;
			size_on_disk = _size_on_disk;
		}
		
		void add( KVInfoData *data )
		{
			num_kvs += data->num_kvs;
			size += data->size;
			size_on_memory += data->size_on_memory;
			size_on_disk += data->size_on_disk;
		}
		
		void remove( KVInfoData *data )
		{
			num_kvs -= data->num_kvs;
			size -= data->size;
			size_on_memory -= data->size_on_memory;
			size_on_disk -= data->size_on_disk;
		}
		
	};
	
	/**
	 Information about the content of a Queue
	 It receives the rigth notification to keep information about this queue
	 */
	
	class KVInfo
	{
	public:
		
		KVInfoData data;

		// Monitor to hold historics
		ParameterMonitor p_num_kvs;
		ParameterMonitor p_size;
		ParameterMonitor p_size_on_memory;
		ParameterMonitor p_size_on_disk;
		
		
		void sample()
		{
			p_num_kvs.push( (double)data.num_kvs / 1000000.0 );
			p_size.push( (double)data.size  / 1000000.0 );
			p_size_on_disk.push((double)data.size_on_disk  / ( (double)data.size + 1.0)  );
			p_size_on_memory.push( (double)data.size_on_memory  / ((double)data.size + 1.0) );
		}
		
		std::string strKVInfo()
		{
			std::ostringstream	o;
			o << " [ KVS: " << au::Format::string( data.num_kvs );
			o << " S: " << au::Format::string( data.size  );
			o << " MU: " << au::Format::string( data.size_on_memory );
			o << " DU: " << au::Format::string( data.size_on_disk );
			o << " ]" ;
			return o.str();
		}
		
		std::string getParameter( std::string name , int scale )
		{
			if( name == "size" )
				return p_size.str( scale );
			if( name == "num_kvs" )
				return p_size.str( scale );
			if( name == "size_on_memory" )
				return p_size_on_memory.str( scale );
			if( name == "size_on_disk" )
				return p_size_on_disk.str( scale );
			
			return "unknown parameter";
		}
		
		void addKVInfoData( KVInfoData *_data );
		void removeKVInfoData( KVInfoData *_data );
		
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
