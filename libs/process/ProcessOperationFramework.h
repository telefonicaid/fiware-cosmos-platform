#ifndef _H_PROCESS_OPERATION_FRAMEWORK
#define _H_PROCESS_OPERATION_FRAMEWORK

#include "OperationFramework.h"				// Own interface
#include "ProcessWriter.h"					// ss::ProcessWriter
#include "samson.pb.h"						// ss::network::,,,
#include "coding.h"							// FileHeader, 

namespace ss {

	
	
	/* Function to work with KVStruct */
	
	class KVInput
	{
		
	public:
		
		KV ** _kv;						// Vector with pointers to kv
		KV *kv;							// Vector of KV elements
		
		size_t max_num_kvs;				// Allocation size
		size_t num_kvs;					// Real number of kvs in the vectors
		
		size_t pos;						// Position where this input is processing
		size_t num_kvs_with_same_key;	// Number of kvs with the same key
		bool participating;				// Flag to indicate if this imput participate in this round
		bool finished;					// Flag to indicate that this input is finised ( no more key-values)
		
		// This functions are set manually
		DataSizeFunction keySize;
		DataSizeFunction valueSize;
		
		// Compare functions
		DataCompareFunction compareFunction;
		
		
		KVInput( )
		{
			_kv = NULL;
			kv  = NULL;
			max_num_kvs = 0;
			num_kvs = 0;
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
			pos = 0;
		}
		
		void addKVs( char * data , FileKVInfo info)
		{
			size_t offset = 0;
			
			// Process a set of key values
			for (size_t i = 0 ; i < info.kvs ; i++)
			{
				kv[ num_kvs ].key = data + offset;
				
				offset += keySize( data + offset );
				
				kv[ num_kvs ].value = data + offset;
				
				offset += valueSize( data + offset );
				
				num_kvs++;
			}

			// Make sure the parsing is OK!
			assert( offset == info.size );
		}
		
		void sort()
		{
			std::sort( _kv , _kv + num_kvs  , compareFunction );
		}
		
		
		void compute_num_kvs_with_same_key()
		{
			num_kvs_with_same_key = 1;
		}
		
		
		
		
	};

	class KVInputs
	{
		
	public:
		
		int num_inputs;			
		KVInput *_input;
		//KVSetStruct *input;

		// Global key compare function
		DataCompareFunction keyCompareFunction;
		
		
		KVInputs( int _num_inputs )
		{
			num_inputs= _num_inputs;
			_input = new KVInput[ num_inputs ];
		}

		~KVInputs()
		{
			delete[] _input;
		}
		
		void sort()
		{
			for (int i = 0 ; i < num_inputs ; i++)
				_input[i].sort();
		}
		
		void processBegin()
		{
			for (int i = 0 ; i < num_inputs ; i++)
				_input[i].compute_num_kvs_with_same_key();
		}
		
		bool processNextIteration()
		{
			/*
			int min_key_input = -1;
			
			for (int i = 1 ; i < num_inputs ; i++)
			{
				if( ! _input[i].finished )
				{
					if ( min_key_input == -1)
					{
						_input[i].participating = true;
						min_key_input = i;
					}
					else
					{
						int res = keyCompareFunction( _input[i]._kvs[ _input[i].pos ] , _input[min_key_input]._kvs[ _input[min_key_input].pos ] );
						if( res == 0)
							_input[i].participating = true;
						else if( res < 0)
						{
							
							for (int j = 0 ; j < i ; j++)
								_input[j].participating = false;	// Deactivate participation of the rest of elements
							
						}
							
					}
					
				}
				
				
				
			}
			*/
			
			return true;
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
			int num_inputs = m.worker_task().input_size();

			assert( num_inputs == (int) inputFormats.size() );
			
			// Complete structure to prepare inputs
			KVInputs inputs( num_inputs );
			
			for (int i = 0 ; i < (int)inputFormats.size() ;i++)
			{
				Data *keyData = process->modulesManager.getData( inputFormats[i].keyFormat );
				Data *valueData = process->modulesManager.getData( inputFormats[i].valueFormat );
				
				assert( keyData );
				assert( valueData );
					
				inputs._input[i].keySize = keyData->getSizeFunction();
				inputs._input[i].valueSize = valueData->getSizeFunction();
				inputs._input[i].compareFunction = keyData->getCompareFunction();	// to be changed by a combination of both functions
			}			
						
			
			// Shared memory area used as input for this operation
			SharedMemoryItem* item =  MemoryManager::shared()->getSharedMemory( m.input_shm() );
			char *data = item->data;
			
			
			int num_hash_groups = m.num_hash_groups();
			int *num_input_files = (int*) malloc( sizeof(int) * num_inputs );
			int total_num_files = 0;
			assert( num_inputs == (int)inputFormats.size() );
			
			for(int i = 0 ; i < num_inputs ; i++)
			{
				num_input_files[i] = m.worker_task().input(i).file_size();
				total_num_files += num_input_files[i];
			}
			
			FileKVInfo **info = (FileKVInfo **) malloc( sizeof(FileKVInfo *) * total_num_files );
			for (int i = 0 ; i < total_num_files ; i++)
				info[i] = (FileKVInfo*) data + i*( sizeof(FileKVInfo)*num_hash_groups);

			// Pointer to current data
			char **file_data = (char**) malloc( sizeof(char*) * total_num_files );

			// get the rigth pointer to where files begins
			char* tmp_file_data = data + ( total_num_files * num_hash_groups * sizeof( FileKVInfo ) );
			for (int i = 0 ; i < total_num_files ; i++)
			{
				size_t _size = *((size_t*)tmp_file_data);
				tmp_file_data += sizeof( size_t);
				
				file_data[i] = tmp_file_data;
				
				tmp_file_data += _size;
			}
			
			


			reduce->init();

			for (int hg = 0 ; hg < num_hash_groups ; hg++)
			{
				// Get the total number of kvs for each input to prepare everything				
				// ---------------------------------------------------------------------
				int file_counter = 0;
				for (int i = 0 ; i < num_inputs ; i++)
				{
					
					size_t total_kvs = 0;
					for (int f = 0 ; f < num_input_files[i] ; f++)
					{
						total_kvs += info[file_counter][hg].kvs;
						file_counter++;
					}
					inputs._input[i].prepareInput( total_kvs );
				}
				
				// Getting real key-values
				// ---------------------------------------------------------------------
				file_counter=0;
				for (int i = 0 ; i < num_inputs ; i++)
					for (int f = 0 ; f < num_input_files[i] ; f++)
					{
						inputs._input[i].addKVs( file_data[file_counter] , info[file_counter][hg] );
						
						file_data[file_counter] += info[file_counter][hg].size;	// Move the pointer for the next hash-group process
						file_counter++;
					}
						
				// Sorting data
				inputs.sort();
				
				
				// Fill poiting to real data
				if( inputs._input[0].num_kvs > 0)
				{
					std::ostringstream o;
					o << "Reduce HG with " << inputs._input[0].num_kvs << "kvs\n";
					std::cout << o.str();
				}
			}
			
			reduce->finish();
			
			free( info );
			free( num_input_files);
			
		}
	};	
}

#endif
