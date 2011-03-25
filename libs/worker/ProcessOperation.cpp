
#include "ProcessOperation.h"			// Own interface
#include "ModulesManager.h"				// ss::ModulesManager
#include "WorkerTask.h"					// ss::WorkerTask
#include "WorkerSubTask.h"				// ss::OrganizerSubTask
#include "Packet.h"						// ss::Packet
#include <algorithm>					// std::sort
#include "WorkerTaskManager.h"			// ss::TaskManager
#include "SamsonWorker.h"				// ss::SamsonWorker
#include "KVInputVector.h"				// ss::KVInputVector	

namespace ss {
	

	
#pragma mark ----
	
	ProcessOperation::ProcessOperation(OperationSubTask *_operationSubTask ) : ProcessBase( _operationSubTask->task , ProcessBase::key_value )
	{
		operationSubTask = _operationSubTask;
		
		// Name of the generator
		operation_name = workerTask->operation();	
				
		// Set the buffer size the max size
		operationSubTask->buffer->setSize( operationSubTask->buffer->getMaxSize() );
		
		// Geting the operation and the data base address
		ModulesManager *modulesManager = ModulesManager::shared();
		operation = modulesManager->getOperation( operation_name );
		data = operationSubTask->buffer->getData();
		

		// Set the output mode to txt in this two operations
		if ( operation->getType() == Operation::parserOut )
			setProcessBaseMode( ProcessBase::txt );
		if ( operation->getType() == Operation::parserOutReduce )
			setProcessBaseMode( ProcessBase::txt );

	}
	
	void ProcessOperation::generateTXT( TXTWriter *writer )
	{
		// Run the operation
		if( !operation )
			LM_X(1,("Internal error: No operation in ProcessOperation"));
		
		switch (operation->getType()) {
				
			case Operation::parserOut:
			{
				runParserOut( writer );
				break;
			}
			case Operation::parserOutReduce:
			{
				runParserOutReduce( writer );
				break;
			}
				
			default:
				LM_X(1,("Internal error: Non valid process operation"));
				break;
		}		
		
	}
	
	
	void ProcessOperation::generateKeyValues( KVWriter *writer )
	{
		
		// Run the operation
		if( !operation )
			LM_X(1,("Internal error"));

		
		switch (operation->getType()) {

			case Operation::reduce:
				runReduce( writer );
				break;
				
			case Operation::map:
			{
				runMap( writer );
				break;
			}
				
			default:
				LM_X(1,("Internal error"));
				break;
		}		
		
	}
	
	OperationInstance* ProcessOperation::getOperationInstance()
	{
		// Run the generator over the ProcessWriter to emit all key-values
		OperationInstance *operationInstance = (OperationInstance*) operation->getInstance();
		operationInstance->environment = &environment;
		operationInstance->tracer = this;
		operationInstance->operationController = this;
		
		return operationInstance;
	}


	void ProcessOperation::runParserOut(TXTWriter *writer )
	{
		// Run the generator over the ProcessWriter to emit all key-values
		ParserOut *parserOut = (ParserOut*) getOperationInstance();
		
		std::vector<KVFormat> inputFormats =  operation->getInputFormats();
		
		// Number of input and input files
		int num_inputs		= operationSubTask->task->workerTask.input_queue_size();
		int num_input_files = 0;
		for (int i = 0 ; i < num_inputs ; i++)
			num_input_files += operationSubTask->task->workerTask.input_queue(i).file_size();
		if( num_inputs != (int) inputFormats.size() )
			LM_X(1,("Internal error"));

		
		// Complete structure to prepare inputs
		KVInputVector inputs(num_inputs);
				
		// Process all input files [ Header ] [ Info ] [ Data ]
		ProcessSharedFile *reduce_file = new ProcessSharedFile[ num_input_files ];
		size_t offset = 0;
		for (int i = 0 ; i < num_input_files ; i++ )
			offset += reduce_file[i].set( data + offset );
		
		// Get the number of hash groups and make sure all the files have the same number
		// Other wise the operation is impossible
		uint32 num_hash_groups = reduce_file[0].header->getNumHashGroups();
		for (int i = 0 ; i < num_input_files ; i++ )
		{
			if( reduce_file[i].header->getNumHashGroups() != num_hash_groups)
				LM_X(1,("Internal error"));

		}
		
		
		// Get the rigth functions to process input key-values
		for (int i = 0 ; i < (int)inputFormats.size() ;i++)
		{
			Data *keyData	= ModulesManager::shared()->getData( inputFormats[i].keyFormat );
			Data *valueData	= ModulesManager::shared()->getData( inputFormats[i].valueFormat );
			
			if( !keyData )
				LM_X(1,("Internal error"));

			if( !valueData )
				LM_X(1,("Internal error"));

			
			inputs.keySize = keyData->getSizeFunction();		// Common to all inputs
			inputs.valueSize[i] = valueData->getSizeFunction();	
		}			
		
		// Structure used to pass information to the operations
		KVSetStruct* inputStructs = (KVSetStruct*) malloc( sizeof(KVSetStruct) * num_inputs );
		
		// Ini the operation
		parserOut->init();
		
		// form operations with the hash-groups
		for (uint32 hg = 0 ; hg < num_hash_groups ; hg++ )
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
			if( inputs.num_kvs != num_kvs )
				LM_X(1,("Internal error"));

			
			
			// Prepare inputStucts
			if( num_inputs != 1)
				LM_X(1,("Internal error"));

			
			inputStructs[0].kvs = &inputs._kv[0];
			inputStructs[0].num_kvs = inputs.num_kvs;
			
			parserOut->run(inputStructs, writer);
			
			
		}

		
		free(inputStructs);
		parserOut->finish();
		delete[] reduce_file;
		
	}		

	void ProcessOperation::runParserOutReduce(TXTWriter *writer )
	{
		
		// Run the generator over the ProcessWriter to emit all key-values
		ParserOutReduce *parserOutReduce = (ParserOutReduce*) getOperationInstance();
        
		// Get the input format of all the inputs
		std::vector<KVFormat> inputFormats =  operation->getInputFormats();
        
		// Get the number of files
		int num_inputs		= operationSubTask->task->workerTask.input_queue_size();
		int num_input_files = 0;
		for (int i = 0 ; i < num_inputs ; i++)
			num_input_files += operationSubTask->task->workerTask.input_queue(i).file_size();
		if( num_input_files == 0)
			return;	// If no input file, no process required
		if( num_inputs != (int) inputFormats.size() )
			LM_X(1,("Internal error:"));
        
		
		// Complete structure to prepare inputs
		KVInputVector inputs(num_inputs);
		
		// Process all input files [ Header ] [ Info ] [ Data ]
		ProcessSharedFile *reduce_file = new ProcessSharedFile[ num_input_files ];
		size_t offset = 0;
		for (int i = 0 ; i < num_input_files ; i++ )
			offset += reduce_file[i].set( data + offset );
		
		// Get the number of hash groups and make sure all the files have the same number
		uint32 num_hash_groups = reduce_file[0].header->getNumHashGroups();
		for (uint32 i = 0 ; i < (uint32)num_input_files ; i++ )
		{
			if( reduce_file[i].header->getNumHashGroups() != num_hash_groups)
				LM_X(1,("Internal error:"));
            
		}
		
		
		// Get the rigth functions to process input key-values
		for (int i = 0 ; i < (int)inputFormats.size() ;i++)
		{
			Data *keyData	= ModulesManager::shared()->getData( inputFormats[i].keyFormat );
			Data *valueData	= ModulesManager::shared()->getData( inputFormats[i].valueFormat );
			
			if( !keyData )
				LM_X(1,("Internal error:"));
            
			if( !valueData )
				LM_X(1,("Internal error:"));
            
			
			inputs.keySize = keyData->getSizeFunction();		// Common to all inputs
			inputs.valueSize[i] = valueData->getSizeFunction();	
		}			
		
		inputs.compare = operation->getInputCompareFunction();
		OperationInputCompareFunction compareKey = operation->getInputCompareByKeyFunction();
		
		
		KVSetStruct* inputStructs = (KVSetStruct*) malloc( sizeof(KVSetStruct) * num_inputs );
		
		
		//std::cout << "Reduce everything ready...\n";
		
		parserOutReduce->init();
		
		
		for (uint32 hg = 0 ; hg < num_hash_groups ; hg++)
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
			if( inputs.num_kvs != num_kvs )
				LM_X(1,("Internal error:"));
            
			
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
				
				parserOutReduce->run(inputStructs, writer);
				
				// Go to the next position
				pos_begin = pos_end;
				pos_end = pos_begin + 1;
				
				
			}
			
			
		}
		
		free(inputStructs);
		parserOutReduce->finish();
		delete[] reduce_file;
		

		
	}	    
	
	void ProcessOperation::runMap( KVWriter *writer  )
	{
		
		// Get an instance of the operation and setup correctly
		Map *map = (Map*) getOperationInstance();
		
		std::vector<KVFormat> inputFormats =  operation->getInputFormats();
		
		int num_inputs		= operationSubTask->task->workerTask.input_queue_size();
		int num_input_files = 0;
		for (int i = 0 ; i < num_inputs ; i++)
			num_input_files += operationSubTask->task->workerTask.input_queue(i).file_size();
		
		if( num_inputs != (int) inputFormats.size() )
			LM_X(1,("Internal error:"));
		
		// If no input files, no operation is needed
		if( num_input_files == 0)
			return;
		
		// Complete structure to prepare inputs
		KVInputVector inputs(num_inputs);
		
		
		// Process all input files [ Header ] [ Info ] [ Data ]
		ProcessSharedFile *reduce_file = new ProcessSharedFile[ num_input_files ];
		size_t offset = 0;
		for (int i = 0 ; i < num_input_files ; i++ )
			offset += reduce_file[i].set( data + offset );
		
		// Get the number of hash groups and make sure all the files have the same number
		uint32 num_hash_groups = reduce_file[0].header->getNumHashGroups();
		for (uint32 i = 0 ; i < (uint32)num_input_files ; i++ )
		{
			if( reduce_file[i].header->getNumHashGroups() != num_hash_groups)
				LM_X(1,("Internal error:"));

		}
		
		
		// Get the rigth functions to process input key-values
		for (int i = 0 ; i < (int)inputFormats.size() ;i++)
		{
			Data *keyData	= ModulesManager::shared()->getData( inputFormats[i].keyFormat );
			Data *valueData	= ModulesManager::shared()->getData( inputFormats[i].valueFormat );
			
			if( !keyData )
				LM_X(1,("Internal error:"));

			if( !valueData )
				LM_X(1,("Internal error:"));

			
			inputs.keySize = keyData->getSizeFunction();		// Common to all inputs
			inputs.valueSize[i] = valueData->getSizeFunction();	
		}			
		
		KVSetStruct* inputStructs = (KVSetStruct*) malloc( sizeof(KVSetStruct) * num_inputs );
		
		map->init();
		
		
		for (uint32 hg = 0 ; hg < num_hash_groups ; hg++)
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
			if( inputs.num_kvs != num_kvs )
				LM_X(1,("Internal error:"));

			
			
			// Prepare inputStucts
			if( num_inputs != 1)
				LM_X(1,("Internal error:"));

			
			inputStructs[0].kvs = &inputs._kv[0];
			inputStructs[0].num_kvs = inputs.num_kvs;
			
			map->run(inputStructs, writer);
			
			
		}
		
		free(inputStructs);
		map->finish();
		delete[] reduce_file;
		
	}
		
	void ProcessOperation::runReduce( KVWriter *writer )
	{
		
		// Run the generator over the ProcessWriter to emit all key-values
		Reduce *reduce = (Reduce*) getOperationInstance();

		// Get the input format of all the inputs
		std::vector<KVFormat> inputFormats =  operation->getInputFormats();

		// Get the number of files
		int num_inputs		= operationSubTask->task->workerTask.input_queue_size();
		int num_input_files = 0;
		for (int i = 0 ; i < num_inputs ; i++)
			num_input_files += operationSubTask->task->workerTask.input_queue(i).file_size();
		if( num_input_files == 0)
			return;	// If no input file, no process required
		if( num_inputs != (int) inputFormats.size() )
			LM_X(1,("Internal error:"));

		
		// Complete structure to prepare inputs
		KVInputVector inputs(num_inputs);
		
		// Process all input files [ Header ] [ Info ] [ Data ]
		ProcessSharedFile *reduce_file = new ProcessSharedFile[ num_input_files ];
		size_t offset = 0;
		for (int i = 0 ; i < num_input_files ; i++ )
			offset += reduce_file[i].set( data + offset );
		
		// Get the number of hash groups and make sure all the files have the same number
		uint32 num_hash_groups = reduce_file[0].header->getNumHashGroups();
		for (uint32 i = 0 ; i < (uint32)num_input_files ; i++ )
		{
			if( reduce_file[i].header->getNumHashGroups() != num_hash_groups)
				LM_X(1,("Internal error:"));

		}
		
		
		// Get the rigth functions to process input key-values
		for (int i = 0 ; i < (int)inputFormats.size() ;i++)
		{
			Data *keyData	= ModulesManager::shared()->getData( inputFormats[i].keyFormat );
			Data *valueData	= ModulesManager::shared()->getData( inputFormats[i].valueFormat );
			
			if( !keyData )
				LM_X(1,("Internal error:"));

			if( !valueData )
				LM_X(1,("Internal error:"));

			
			inputs.keySize = keyData->getSizeFunction();		// Common to all inputs
			inputs.valueSize[i] = valueData->getSizeFunction();	
		}			
		
		inputs.compare = operation->getInputCompareFunction();
		OperationInputCompareFunction compareKey = operation->getInputCompareByKeyFunction();
		
		
		KVSetStruct* inputStructs = (KVSetStruct*) malloc( sizeof(KVSetStruct) * num_inputs );
		
		
		//std::cout << "Reduce everything ready...\n";
		
		reduce->init();
		
		
		for (uint32 hg = 0 ; hg < num_hash_groups ; hg++)
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
			if( inputs.num_kvs != num_kvs )
				LM_X(1,("Internal error:"));

			
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
				
				reduce->run(inputStructs, writer);
				
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
