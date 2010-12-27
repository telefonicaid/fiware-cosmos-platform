

#include "ProcessOperationFramework.h"		// Own interface
#include "WorkerTaskManager.h"				// ss::WorkerTaskItemWithOutput

namespace ss {

	void ProcessOperationFramework::flushOutput( WorkerTaskItem *taskItem)
	{
		if( pw )
			pw->FlushBuffer(taskItem);
		if( ptw )
			ptw->FlushBuffer(taskItem);
	}
	
	ProcessOperationFramework::ProcessOperationFramework( Process *_process , network::ProcessMessage m  ) : OperationFramework( m )
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
		if( pw )
			pw->setProcess( process );	
		if( ptw )
			ptw->setProcess( process );	
		
	}
	
	ProcessOperationFramework::~ProcessOperationFramework()
	{
		delete environment;
	}
	
	void ProcessOperationFramework::run()
	{
		
		// Run the operation
		assert( operation );
		
		switch (operation->getType()) {
			case Operation::generator:
			{
				// Run the generator over the ProcessWriter to emit all key-values
				Generator *generator = (Generator*) operation->getInstance();
				generator->environment = environment;
				pw->clear();
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
				pw->clear();
				parser->run( item->data , m.input_size() ,  pw );
				parser->finish();
				break;
			}
			case Operation::parserOut:
			{
				runParserOut();
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
	
	void ProcessOperationFramework::runParserOut()
	{
		// Run the generator over the ProcessWriter to emit all key-values
		ParserOut *parserOut = (ParserOut*) operation->getInstance();
		parserOut->environment = environment;
		
		std::vector<KVFormat> inputFormats =  operation->getInputFormats();
		
		int num_inputs		= m.worker_task().input_size();
		int num_input_files = m.num_input_files();
		
		assert( num_inputs == (int) inputFormats.size() );
		
		// Complete structure to prepare inputs
		KVInputVector inputs(num_inputs);
		
		// Shared memory area used as input for this operation
		SharedMemoryItem* item =  MemoryManager::shared()->getSharedMemory( m.input_shm() );
		char *data = item->data;
		
		// Process all input files [ Header ] [ Info ] [ Data ]
		ProcessSharedFile *reduce_file = new ProcessSharedFile[ num_input_files ];
		size_t offset = 0;
		for (int i = 0 ; i < num_input_files ; i++ )
			offset += reduce_file[i].set( data + offset );
		

		// Get the number of hash groups and make sure all the files have the same number
		int num_hash_groups = reduce_file[0].header->num_hash_groups();
		for (int i = 0 ; i < num_input_files ; i++ )
		{
			assert( reduce_file[i].header->num_hash_groups() == num_hash_groups);
		}
		
		
		// Get the rigth functions to process input key-values
		for (int i = 0 ; i < (int)inputFormats.size() ;i++)
		{
			Data *keyData	= process->modulesManager.getData( inputFormats[i].keyFormat );
			Data *valueData	= process->modulesManager.getData( inputFormats[i].valueFormat );
			
			assert( keyData );
			assert( valueData );
			
			inputs.keySize = keyData->getSizeFunction();		// Common to all inputs
			inputs.valueSize[i] = valueData->getSizeFunction();	
		}			
		
		KVSetStruct* inputStructs = (KVSetStruct*) malloc( sizeof(KVSetStruct) * num_inputs );
		
		parserOut->init();
		
		
		for (int hg = 0 ; hg < num_hash_groups ; hg++)
		{
			
			// Counte the number of key-values I will have in this round
			size_t num_kvs = 0;
			for (int f = 0 ; f < num_input_files ; f++)
				num_kvs += reduce_file[f].getNumKVsInThisHashGroup();
			
			// Prepare the input with the rigth number of key-values
			inputs.prepareInput(num_kvs);
			
			// Add the key-values from all the inputs
			for (int f = 0 ; f < num_input_files ; f++)
			{
				ProcessSharedFile &_file = reduce_file[f];
				inputs.addKVs( _file );
			}
			
			// Make sure we have read all the key-values
			assert( inputs.num_kvs == num_kvs );
			
			
			// Prepare inputStucts
			assert( num_inputs == 1);
			
			inputStructs[0].kvs = &inputs._kv[0];
			inputStructs[0].num_kvs = inputs.num_kvs;
			
			parserOut->run(inputStructs, ptw);
			
			
		}
		
		free(inputStructs);
		parserOut->finish();
		delete[] reduce_file;
		
	}		
	
	void ProcessOperationFramework::runMap()
	{
		// Run the generator over the ProcessWriter to emit all key-values
		Map *map = (Map*) operation->getInstance();
		map->environment = environment;
		
		std::vector<KVFormat> inputFormats =  operation->getInputFormats();
		
		int num_inputs		= m.worker_task().input_size();
		int num_input_files = m.num_input_files();
		
		assert( num_inputs == (int) inputFormats.size() );
		
		// If no input files, no operation is needed
		if( num_input_files == 0)
			return;
		
		// Complete structure to prepare inputs
		KVInputVector inputs(num_inputs);
		
		// Shared memory area used as input for this operation
		SharedMemoryItem* item =  MemoryManager::shared()->getSharedMemory( m.input_shm() );
		char *data = item->data;
		
		// Process all input files [ Header ] [ Info ] [ Data ]
		ProcessSharedFile *reduce_file = new ProcessSharedFile[ num_input_files ];
		size_t offset = 0;
		for (int i = 0 ; i < num_input_files ; i++ )
			offset += reduce_file[i].set( data + offset );
		
		// Get the number of hash groups and make sure all the files have the same number
		int num_hash_groups = reduce_file[0].header->num_hash_groups();
		for (int i = 0 ; i < num_input_files ; i++ )
		{
			assert( reduce_file[i].header->num_hash_groups() == num_hash_groups);
		}
		
		
		// Get the rigth functions to process input key-values
		for (int i = 0 ; i < (int)inputFormats.size() ;i++)
		{
			Data *keyData	= process->modulesManager.getData( inputFormats[i].keyFormat );
			Data *valueData	= process->modulesManager.getData( inputFormats[i].valueFormat );
			
			assert( keyData );
			assert( valueData );
			
			inputs.keySize = keyData->getSizeFunction();		// Common to all inputs
			inputs.valueSize[i] = valueData->getSizeFunction();	
		}			
		
		KVSetStruct* inputStructs = (KVSetStruct*) malloc( sizeof(KVSetStruct) * num_inputs );
		
		map->init();
		
		// Init the PW
		pw->clear();
		
		for (int hg = 0 ; hg < num_hash_groups ; hg++)
		{
			
			// Counte the number of key-values I will have in this round
			size_t num_kvs = 0;
			for (int f = 0 ; f < num_input_files ; f++)
				num_kvs += reduce_file[f].getNumKVsInThisHashGroup();
			
			// Prepare the input with the rigth number of key-values
			inputs.prepareInput(num_kvs);
			
			// Add the key-values from all the inputs
			for (int f = 0 ; f < num_input_files ; f++)
			{
				ProcessSharedFile &_file = reduce_file[f];
				inputs.addKVs( _file );
			}
			
			// Make sure we have read all the key-values
			assert( inputs.num_kvs == num_kvs );
			
			
			// Prepare inputStucts
			assert( num_inputs == 1);
			
			inputStructs[0].kvs = &inputs._kv[0];
			inputStructs[0].num_kvs = inputs.num_kvs;
			
			map->run(inputStructs, pw);
			
			
		}
		
		free(inputStructs);
		map->finish();
		delete[] reduce_file;
		
	}
	
	void ProcessOperationFramework::runReduce()
	{
		
		// Run the generator over the ProcessWriter to emit all key-values
		Reduce *reduce = (Reduce*) operation->getInstance();
		reduce->environment = environment;
		
		std::vector<KVFormat> inputFormats =  operation->getInputFormats();
		
		int num_inputs		= m.worker_task().input_size();
		int num_input_files = m.num_input_files();
		
		if( num_input_files == 0)
			return;	// If no input file, no process required
		
		assert( num_inputs == (int) inputFormats.size() );
		
		// Complete structure to prepare inputs
		KVInputVector inputs(num_inputs);
		
		// Shared memory area used as input for this operation
		SharedMemoryItem* item =  MemoryManager::shared()->getSharedMemory( m.input_shm() );
		char *data = item->data;
		
		// Process all input files [ Header ] [ Info ] [ Data ]
		ProcessSharedFile *reduce_file = new ProcessSharedFile[ num_input_files ];
		size_t offset = 0;
		for (int i = 0 ; i < num_input_files ; i++ )
			offset += reduce_file[i].set( data + offset );
		
		// Get the number of hash groups and make sure all the files have the same number
		int num_hash_groups = reduce_file[0].header->num_hash_groups();
		for (int i = 0 ; i < num_input_files ; i++ )
		{
			assert( reduce_file[i].header->num_hash_groups() == num_hash_groups);
		}
		
		
		// Get the rigth functions to process input key-values
		for (int i = 0 ; i < (int)inputFormats.size() ;i++)
		{
			Data *keyData	= process->modulesManager.getData( inputFormats[i].keyFormat );
			Data *valueData	= process->modulesManager.getData( inputFormats[i].valueFormat );
			
			assert( keyData );
			assert( valueData );
			
			inputs.keySize = keyData->getSizeFunction();		// Common to all inputs
			inputs.valueSize[i] = valueData->getSizeFunction();	
		}			
		
		inputs.compare = operation->getInputCompareFunction();
		OperationInputCompareFunction compareKey = operation->getInputCompareByKeyFunction();
		
		
		KVSetStruct* inputStructs = (KVSetStruct*) malloc( sizeof(KVSetStruct) * num_inputs );
		
		
		//std::cout << "Reduce everything ready...\n";
				
		reduce->init();
		
		pw->clear();
		
		for (int hg = 0 ; hg < num_hash_groups ; hg++)
		{
			
			// Counte the number of key-values I will have in this round
			size_t num_kvs = 0;
			for (int f = 0 ; f < num_input_files ; f++)
				num_kvs += reduce_file[f].getNumKVsInThisHashGroup();
			
			// Prepare the input with the rigth number of key-values
			inputs.prepareInput(num_kvs);
			
			// Add the key-values from all the inputs
			for (int f = 0 ; f < num_input_files ; f++)
			{
				ProcessSharedFile &_file = reduce_file[f];
				inputs.addKVs( _file );
			}
			
			// Make sure we have read all the key-values
			assert( inputs.num_kvs == num_kvs );
			
			//std::cout << "Hash group with " << num_kvs << " kvs inputs ready\n";
			
			// Sort all input key-values by key - input - value
			inputs.sort();
			
			//std::cout << "Hash group with " << num_kvs << " kvs inputs sorted\n";
			
			// Process all the key-values in order
			size_t pos_begin = 0;	// Position where the next group of key-values begin
			size_t pos_end	 = 1;	// Position where the next group of key-values finish
			
			//std::cout << "Hash group with " << num_kvs << " kvs processing\n";

			/*
			 if( num_kvs > 0 )
				 std::cout << "Reduce Hash group with " << num_kvs << " kvs\n";
			*/
			
			while( pos_begin < num_kvs )
			{
				//std::cout << "PB: " << pos_begin << " PE: " << pos_end << "\n";
				// Identify the number of key-values with the same key
				while( ( pos_end < num_kvs ) && ( compareKey( inputs._kv[pos_begin] , inputs._kv[pos_end] ) == 0) )
					pos_end++;
				
				size_t pos_pointer = pos_begin;
				for (int i = 0 ; i < num_inputs ;i++)
				{
					if( (pos_pointer == pos_end) || ( inputs._kv[pos_pointer]->input != i) )
						inputStructs[i].num_kvs = 0;
					else
					{
						inputStructs[i].kvs = &inputs._kv[pos_pointer];
						inputStructs[i].num_kvs = 0;
						while( ( pos_pointer < pos_end ) && ( inputs._kv[pos_pointer]->input == i) )
						{
							inputStructs[i].num_kvs++;
							pos_pointer++;
						}
					}
				}
				
				reduce->run(inputStructs, pw);
				
				// Go to the next position
				pos_begin = pos_end;
				pos_end = pos_begin + 1;
				
				
			}
			
			
		}
		
		free(inputStructs);
		reduce->finish();
		delete[] reduce_file;
		
	}
	
}


