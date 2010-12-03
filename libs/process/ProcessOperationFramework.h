#ifndef _H_PROCESS_OPERATION_FRAMEWORK
#define _H_PROCESS_OPERATION_FRAMEWORK

#include "OperationFramework.h"				// Own interface
#include "ProcessWriter.h"					// ss::ProcessWriter
#include "samson.pb.h"						// ss::network::,,,
#include "coding.h"							// FileHeader, 

namespace ss {
	
	/* Vector key-values to sort and process all input channels */
	
	class KVInputVector
	{
		
	public:
		
		KV *kv;							// Dynamic Vector of KV elements
		KV ** _kv;						// Dynamic Vector with pointers to kv
		
		size_t max_num_kvs;				// Allocation size
		size_t num_kvs;					// Real number of kvs in the vectors
		
		DataSizeFunction *keySize;
		DataSizeFunction *valueSize;
		
		int num_inputs;
		
		KVInputVector( int _num_inputs )
		{
			num_inputs = _num_inputs;
			
			_kv = NULL;
			kv  = NULL;
			max_num_kvs = 0;
			num_kvs = 0;
			
			
			keySize = (DataSizeFunction *) malloc( sizeof( DataSizeFunction) * num_inputs );
			valueSize = (DataSizeFunction *) malloc( sizeof( DataSizeFunction) * num_inputs );
		}
		
		~KVInputVector()
		{
			if( _kv )
				free( _kv );
			if( kv ) 
				free ( kv );
		}
		
		void prepareInput( size_t _max_num_kvs )
		{
			if( _max_num_kvs > max_num_kvs )
			{
				if( _kv )
					free( _kv );
				if( kv ) 
					free ( kv );

				// Set a new maximum number of kvs
				max_num_kvs = _max_num_kvs;
				
				_kv = (KV**) malloc( sizeof(KV*) * _max_num_kvs );
				kv  = (KV*) malloc( sizeof(KV) * max_num_kvs );
				
			}
			
			// Set the pointers to internal structure ( to sort again )
			for (size_t i = 0 ; i < max_num_kvs ; i++)
				_kv[i] = &kv[i];
			
			num_kvs = 0;
		}
		
		void addKVs( int input , char * data , FileKVInfo info)
		{
			DataSizeFunction _keySize	= keySize[input];
			DataSizeFunction _valueSize = valueSize[input];

			size_t offset = 0;
			
			// Process a set of key values
			for (size_t i = 0 ; i < info.kvs ; i++)
			{
				kv[ num_kvs ].key = data + offset;
				
				offset += _keySize( data + offset );
				
				kv[ num_kvs ].value = data + offset;
				
				offset += _valueSize( data + offset );

				kv[num_kvs].input = input;
				
				num_kvs++;
			}

			// Make sure the parsing is OK!
			assert( offset == info.size );
		}
		
	};

	
	class ReduceFile
	{
	public:
		
		// Pointer to the header
		ReduceFileHeader *header;

		// Pointer to the info for each hash-group
		FileKVInfo *info;

		// Data pointer to data
		char *data;
		size_t offset;

		
		// Set the real pointer to the current data and returns the toal size of this file in the shared memory area
		size_t set( char *_data , int num_hash_groups )
		{
			header = (ReduceFileHeader*) _data;
			
			offset = 0;
			
			info = (FileKVInfo*) _data + sizeof(ReduceFileHeader);
			
			data = _data + sizeof(ReduceFileHeader) + sizeof(FileKVInfo)*num_hash_groups;
			
			return sizeof(ReduceFile) + sizeof(FileKVInfo)*num_hash_groups + header->size;
			
		}
		
		char* currentData()
		{
			return data + offset;
		}
		
		void increaseOffset( size_t _offset )
		{
			offset += _offset;
		}
		
		
		
	};
	
	
	class Process;
	class WorkerTaskItemWithOutput;
	
	class ProcessOperationFramework : public OperationFramework
	{
		
		Process *process;
		Operation *operation;
		
		Environment * environment;
		
	public:
		
		ProcessOperationFramework( Process *_process , network::ProcessMessage m  ) : OperationFramework( m )
		{
			
			process = _process;
			assert( process );
			
			// Create the environment for any possible operation
			environment = new Environment();
			environment->copyFrom(&process->environment);
			
			// We still have no idea about the operation to run
			operation = process->modulesManager.getOperation( m.operation() );
			assert( operation );
			
			// Set the process to report "full buffer messages"
			pw->setProcess( process );	

		}
		
		~ProcessOperationFramework()
		{
			delete environment;
		}
		
		void flushOutput( WorkerTaskItemWithOutput *taskItem );
		
		
		void run()
		{
			// Run the operation
			assert( operation );
			
			switch (operation->getType()) {
				case Operation::generator:
				{
					// Run the generator over the ProcessWriter to emit all key-values
					Generator *generator = (Generator*) operation->getInstance();
					generator->environment = environment;
					generator->run( pw );
					break;
				}
				case Operation::parser:
				{
					// Run the generator over the ProcessWriter to emit all key-values
					Parser *parser = (Parser*) operation->getInstance();
					parser->environment = environment;
					
					SharedMemoryItem*item =  MemoryManager::shared()->getSharedMemory( m.input_shm() );

					parser->init();
					parser->run( item->data , m.input_size() ,  pw );
					parser->finish();
					break;
				}

				case Operation::reduce:
					runReduce();
					break;
					
					
				case Operation::map:
				{
					runMap();
					break;
				}
					
					
				default:
					assert( false );
					break;
			}
			
		}
		
		void runMap()
		{
			// Run the generator over the ProcessWriter to emit all key-values
			Map *map = (Map*) operation->getInstance();
			map->environment = environment;
			
			std::vector<KVFormat> inputFormats =  operation->getInputFormats();
			assert( inputFormats.size() == 1);	// In map operations we only have one input
			
			Data *keyData = process->modulesManager.getData( inputFormats[0].keyFormat );
			Data *valueData = process->modulesManager.getData( inputFormats[0].valueFormat );
			
			assert( keyData );
			assert( valueData );
			
			// Functions to get the size of the key-values
			DataSizeFunction keySize = keyData->getSizeFunction();
			DataSizeFunction valueSize = valueData->getSizeFunction();
			
			
			// Shared memory area used as input for this operation
			SharedMemoryItem* item =  MemoryManager::shared()->getSharedMemory( m.input_shm() );
			
			// This item is suppoused to be a full KV file
			FileHeader *header = (FileHeader*)item->data;
			
			assert( header->check() );	// Assert magic number
			
			// Header of the KV file
			FileKVInfo* info = (FileKVInfo*) ( item->data + sizeof(FileHeader) ); 
			
			
			// Data pointer ( initial position of data )
			char *data = item->data + sizeof(FileHeader) + sizeof(FileKVInfo)*( KV_NUM_HASHGROUPS + 1);
			
			// Collect the maximum number of key-values per hash-group
			size_t max_num_kvs = 0;
			for (int i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
				if( info[i+1].kvs > max_num_kvs )
					max_num_kvs = info[i+1].kvs;
			
			// Input structure used to be the argument to map operation
			KVSetStruct input;
			
			// Alloc space for the KV objects and for the KV pointers
			input.kvs = (KV**) malloc( sizeof(KV*) * max_num_kvs );
			KV* kv = (KV*) malloc( sizeof(KV) * max_num_kvs );
			
			for (size_t i = 0 ; i < max_num_kvs ; i++)
				input.kvs[i] = &kv[i];
			
			
			map->init();
			
			// Fill for each hash-group
			for (int i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
			{
				// Prepare input for this hash-group
				size_t local_size	= info[i+1].size;
				size_t local_kvs	= info[i+1].kvs;
				size_t pos_kv = 0;
				
				// Last position of the pointer
				char *last_data = data + local_size;
				
				while( data < last_data )
				{
					
					kv[pos_kv].key = data;
					size_t key_size = keySize(  data );
					data += key_size;
					
					kv[pos_kv].value = data;
					size_t value_size = valueSize(  data );
					data += value_size;
					
					pos_kv++;
				}
				
				assert( pos_kv == local_kvs);
				input.num_kvs = local_kvs;
				
				// run the map for this hash-group
				map->run( &input ,  pw );
			}
			
			map->finish();
			
			free( input.kvs );
			free( kv );
			
		}
		
		void runReduce()
		{
			
			// Run the generator over the ProcessWriter to emit all key-values
			Reduce *reduce = (Reduce*) operation->getInstance();
			reduce->environment = environment;
			
			std::vector<KVFormat> inputFormats =  operation->getInputFormats();

			int num_inputs		= m.worker_task().input_size();
			int num_hash_groups = m.num_hash_groups();
			int num_input_files = m.num_input_files();

			assert( num_inputs == (int) inputFormats.size() );
			
			// Complete structure to prepare inputs
			KVInputVector inputs(num_inputs);
			
			// Shared memory area used as input for this operation
			SharedMemoryItem* item =  MemoryManager::shared()->getSharedMemory( m.input_shm() );
			char *data = item->data;
			
			
			// Process all input files [ Header ] [ Info ] [ Data ]
			ReduceFile *file = new ReduceFile[ num_input_files ];
			size_t offset = 0;
			for (int i = 0 ; i < num_input_files ; i++ )
				offset += file[i].set( data + offset , num_hash_groups );
			
			
			for (int i = 0 ; i < (int)inputFormats.size() ;i++)
			{
				Data *keyData	= process->modulesManager.getData( inputFormats[i].keyFormat );
				Data *valueData	= process->modulesManager.getData( inputFormats[i].valueFormat );
				
				assert( keyData );
				assert( valueData );
					
				inputs.keySize[i] = keyData->getSizeFunction();
				inputs.valueSize[i] = valueData->getSizeFunction();
			}			

			reduce->init();
			
			
			for (int hg = 0 ; hg < num_hash_groups ; hg++)
			{
				
				// Counte the number of key-values I will have in this round
				size_t num_kvs = 0;
				for (int f = 0 ; f < num_input_files ; f++)
					num_kvs += file[f].info[hg].kvs;
				inputs.prepareInput(num_kvs);
				
				// Add the key-values from all the inputs
				for (int f = 0 ; f < num_input_files ; f++)
				{
					inputs.addKVs(file[f].header->input , file[f].currentData() , file[f].info[hg]);
					file[f].increaseOffset( file[f].info[hg].size );
				}
				
				// Make sure we have read all the key-values
				assert( inputs.num_kvs == num_kvs );
				
			}
			

			reduce->finish();
			
			
		}
	};	
}

#endif
