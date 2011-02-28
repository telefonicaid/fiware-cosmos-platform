
#include "ProcessOperation.h"		// Own interface
#include "ModulesManager.h"			// ss::ModulesManager
#include "WorkerTask.h"				// ss::WorkerTask
#include "WorkerSubTask.h"		// ss::OrganizerSubTask
#include "Packet.h"					
#include <algorithm>    // std::sorr

namespace ss {
	
	
	
	/* Vector key-values to sort and process all input channels */
	
	class KVInputVector
	{
		
	public:
		
		KV *kv;							// Dynamic Vector of KV elements
		KV ** _kv;						// Dynamic Vector with pointers to kv
		
		size_t max_num_kvs;				// Allocation size
		size_t num_kvs;					// Real number of kvs in the vectors
		
		DataSizeFunction keySize;		// Function to get the size of a particular value of the key ( common to all inputs )
		DataSizeFunction *valueSize;	// Function to get the size of a partiuclar value of the value ( different for each input )
		
		OperationInputCompareFunction compare;
		
		int num_inputs;
		
		KVInputVector( int _num_inputs )
		{
			num_inputs = _num_inputs;
			
			_kv = NULL;
			kv  = NULL;
			max_num_kvs = 0;
			num_kvs = 0;
			
			valueSize = (DataSizeFunction *) malloc( sizeof( DataSizeFunction) * num_inputs );
		}
		
		~KVInputVector()
		{
			if( _kv )
				free( _kv );
			if( kv ) 
				free ( kv );
		}
		
		// For each hash group, prepare the vector
		
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
			for (size_t i = 0 ; i < _max_num_kvs ; i++)
				_kv[i] = &kv[i];
			
			num_kvs = 0;
		}
		
		void addKVs( ProcessSharedFile& file )
		{
			int input = file.header->input;
			
			// Get the rigth size function
			DataSizeFunction _valueSize = valueSize[input];
			
			// Get the pointer to data
			char *data = file.data + file.offset;
			
			// Get the info we should read
			FileKVInfo info = file.info[ file.hg ];
			
			
			// Local offset
			size_t offset = 0;
			
			// Process a set of key values
			for (size_t i = 0 ; i < info.kvs ; i++)
			{
				kv[ num_kvs ].key = data + offset;
				
				offset += keySize( data + offset );
				
				kv[ num_kvs ].value = data + offset;
				
				offset += _valueSize( data + offset );
				
				kv[num_kvs].input = input;
				
				num_kvs++;
			}
			
			// Make sure the parsing is OK!
			assert( offset == info.size );
			
			// update the  file for the next round
			file.hg++;
			file.offset += info.size;
			
		}
		
		
		// global sort function key - input - value
		void sort()
		{
			if( num_kvs > 0 )
				std::sort( _kv , _kv + num_kvs , compare );
		}
		
	};
	
	
#pragma mark ----
	
	ProcessOperation::ProcessOperation(OperationSubTask *_operationSubTask ) : ProcessBase( _operationSubTask->task , ProcessBase::key_value )
	{
		operationSubTask = _operationSubTask;
		
		// Name of the generator
		operation_name = workerTask->operation();	
		
		// Description with the name of the generator
		setStatus( operation_name );
		
		// Set the buffer size the max size
		operationSubTask->buffer->setSize( operationSubTask->buffer->getMaxSize() );
		
		// Geting the operation and the data base address
		ModulesManager *modulesManager = ModulesManager::shared();
		operation = modulesManager->getOperation( operation_name );
		data = operationSubTask->buffer->getData();
		
		
		if ( operation->getType() == Operation::parserOut )
			setProcessBaseMode( ProcessBase::txt );

		
	}
	
	void ProcessOperation::generateTXT( TXTWriter *writer )
	{
		// Run the operation
		assert( operation );
		
		switch (operation->getType()) {
			case Operation::generator:
				assert( false );	// Not considered here any more
				break;
			case Operation::parser:
				assert( false );	// Not considered here any more
				break;
			case Operation::parserOut:
			{
				runParserOut( writer );
				break;
			}
			case Operation::reduce:
				assert( false );	// Not considered here any more
				break;
				
			case Operation::map:
			{
				assert( false );	// Not considered here any more
				break;
			}
				
			default:
				assert( false );
				break;
		}		
		
	}
	
	
	void ProcessOperation::generateKeyValues( KVWriter *writer )
	{
		
		// Run the operation
		assert( operation );
		
		switch (operation->getType()) {
			case Operation::generator:
				assert( false );	// Not considered here any more
				break;
			case Operation::parser:
				assert( false );	// Not considered here any more
				break;
			case Operation::parserOut:
			{
				assert(false);
				break;
			}
			case Operation::reduce:
				runReduce( writer );
				break;
				
			case Operation::map:
			{
				runMap( writer );
				break;
			}
				
			default:
				assert( false );
				break;
		}		
		
	}

	void ProcessOperation::runParserOut(TXTWriter *writer )
	{
		// Run the generator over the ProcessWriter to emit all key-values
		ParserOut *parserOut = (ParserOut*) operation->getInstance();
		parserOut->environment = &environment;
		parserOut->tracer = this;
		parserOut->operationController = this;
		
		std::vector<KVFormat> inputFormats =  operation->getInputFormats();
		
		int num_inputs		= operationSubTask->task->workerTask.input_size();
		int num_input_files = 0;
		for (int i = 0 ; i < num_inputs ; i++)
			num_input_files += operationSubTask->task->workerTask.input(i).file_size();
		
		assert( num_inputs == (int) inputFormats.size() );
		
		// Complete structure to prepare inputs
		KVInputVector inputs(num_inputs);
				
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
			Data *keyData	= ModulesManager::shared()->getData( inputFormats[i].keyFormat );
			Data *valueData	= ModulesManager::shared()->getData( inputFormats[i].valueFormat );
			
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
			
			parserOut->run(inputStructs, writer);
			
			
		}

		
		free(inputStructs);
		parserOut->finish();
		delete[] reduce_file;
		
	}		

	
	void ProcessOperation::runMap( KVWriter *writer  )
	{
		
		
		// Run the generator over the ProcessWriter to emit all key-values
		Map *map = (Map*) operation->getInstance();
		map->environment = &environment;
		map->tracer = this;						// To be able to send traces
		map->operationController = this;

		
		std::vector<KVFormat> inputFormats =  operation->getInputFormats();
		
		int num_inputs		= operationSubTask->task->workerTask.input_size();
		int num_input_files = 0;
		for (int i = 0 ; i < num_inputs ; i++)
			num_input_files += operationSubTask->task->workerTask.input(i).file_size();
		
		
		assert( num_inputs == (int) inputFormats.size() );
		
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
		int num_hash_groups = reduce_file[0].header->num_hash_groups();
		for (int i = 0 ; i < num_input_files ; i++ )
		{
			assert( reduce_file[i].header->num_hash_groups() == num_hash_groups);
		}
		
		
		// Get the rigth functions to process input key-values
		for (int i = 0 ; i < (int)inputFormats.size() ;i++)
		{
			Data *keyData	= ModulesManager::shared()->getData( inputFormats[i].keyFormat );
			Data *valueData	= ModulesManager::shared()->getData( inputFormats[i].valueFormat );
			
			assert( keyData );
			assert( valueData );
			
			inputs.keySize = keyData->getSizeFunction();		// Common to all inputs
			inputs.valueSize[i] = valueData->getSizeFunction();	
		}			
		
		KVSetStruct* inputStructs = (KVSetStruct*) malloc( sizeof(KVSetStruct) * num_inputs );
		
		map->init();
		
		
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
			
			map->run(inputStructs, writer);
			
			
		}
		
		free(inputStructs);
		map->finish();
		delete[] reduce_file;
		
	}
		
	void ProcessOperation::runReduce( KVWriter *writer )
	{
		
		// Run the generator over the ProcessWriter to emit all key-values
		Reduce *reduce = (Reduce*) operation->getInstance();
		reduce->environment = &environment;
		reduce->tracer = this;						// To be able to send traces
		reduce->operationController = this;

		std::vector<KVFormat> inputFormats =  operation->getInputFormats();
		
		int num_inputs		= operationSubTask->task->workerTask.input_size();
		int num_input_files = 0;
		for (int i = 0 ; i < num_inputs ; i++)
			num_input_files += operationSubTask->task->workerTask.input(i).file_size();
		
		if( num_input_files == 0)
			return;	// If no input file, no process required
		
		assert( num_inputs == (int) inputFormats.size() );
		
		// Complete structure to prepare inputs
		KVInputVector inputs(num_inputs);
		
		
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
			Data *keyData	= ModulesManager::shared()->getData( inputFormats[i].keyFormat );
			Data *valueData	= ModulesManager::shared()->getData( inputFormats[i].valueFormat );
			
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
		
		/*
		 
		
		// Get a generator instance
		
		Generator * generator  = (Generator*) op->getInstance();
		generator->environment = &environment;
		
		generator->run( writer );
		
		delete generator;
		 */
		
	
}
