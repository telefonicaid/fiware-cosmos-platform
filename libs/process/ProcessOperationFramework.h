#ifndef _H_PROCESS_OPERATION_FRAMEWORK
#define _H_PROCESS_OPERATION_FRAMEWORK

#include "OperationFramework.h"				// Own interface
#include "ProcessWriter.h"					// ss::ProcessWriter
#include "samson.pb.h"						// ss::network::,,,
#include "coding.h"							// FileHeader, 

namespace ss {

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

				case Operation::map:
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
					
					break;
				}
					
					
				default:
					assert( false );
					break;
			}
			
		}
	};	
}

#endif
