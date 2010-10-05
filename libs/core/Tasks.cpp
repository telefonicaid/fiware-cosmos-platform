#include "Tasks.h"
#include "NetworkManager.h"
#include "SSMonitor.h"



namespace ss
{
   	Task::Task( std::string command , size_t id, Task * parentTask ) : TaskBase(id, parentTask)
	{
		AUCommandLine _cmdLine;						// Parser used to analyse the command sentence
		_command = command;
		
		// Common parse options
		_cmdLine.set_flag_boolean("remove");		// Flag to indicate that original sets should be removed from the input queue
		_cmdLine.set_flag_boolean("sub_operation");	// Flag to indicate that it is a suboperation ( for every hash )
		_cmdLine.set_flag_int("hash", -1);			// In suboperations, it is used to indicate the hash to process
		_cmdLine.set_flag_int("n", 1);
		_cmdLine.set_flag_boolean("no-state");
		_cmdLine.set_flag_int("scale", 0);

		_cmdLine.parse( command );
		
		// Capture the flag of "remove original source"
		remove_originals = _cmdLine.get_flag_bool("remove");
		
		// Let's identify the task
		
		taskType = TASK_TYPE_UNKNOWN;
		error = false;
		
		if( _cmdLine.get_argument(0) == "show" )
		{
			taskType = TASK_TYPE_SHOW; 
			if( _cmdLine.get_num_arguments() < 2)
			{
				errOutput << "Wrong number of parameters. Use show queue" << std::endl;
				error = true;
				return;
			}
			
			// Only show the first queue ( not possible to show more than one )
			input_queues.push_back( _cmdLine.get_argument(1) );
			
			// Get any valid set to show
			KVSetVector *sets = KVManager::shared()->getAnyKVSetForTask( this, _cmdLine.get_argument(1) );
			groups.push_back( sets );
			
			return;
		}

		if( _cmdLine.get_argument(0) == "spread" )
		{
			taskType = TASK_TYPE_SPREAD; 
			if( _cmdLine.get_num_arguments() != 3)
			{
				errOutput << "Wrong number of parameters. Use spread input_queue output_queue" << std::endl;
				error = true;
				return;
			}
			
			input_queues.push_back( _cmdLine.get_argument(1) );
			output_queues.push_back( _cmdLine.get_argument(2) );
			hash = 0;					// Spread is only a hash "0" operation
			remove_originals = true;	// It is always removing the original sets

			// Get the formats
			input_queues_format.push_back( KVManager::shared()->getKVQueueFormat( input_queues[0] ));
			output_queues_format.push_back( KVManager::shared()->getKVQueueFormat( output_queues[0] ));

			
			// Check both formats are the same
			if( !input_queues_format[0].isEqual( output_queues_format[0] ) )
			{
				error = true;
				errOutput << "Spread operation only works with queues with the same format";
				return;
			}
			
			return;
		}
		
		
		if ( _cmdLine.get_flag_bool("sub_operation") )
		{
			operation_name = _cmdLine.get_argument(0);
			operation = ModulesManager::shared()->getOperation( operation_name );
			taskType = TASK_TYPE_SUB_MAP_REDUCE;
			hash = _cmdLine.get_flag_int("hash");
			
			if( !setupInputOutput(_cmdLine) )
			{
				error = true;
				return;
			}
			
			return;
		}
		
		// Operation or script
		operation_name = _cmdLine.get_argument(0);
		operation = ModulesManager::shared()->getOperation( operation_name );
		if( ! operation )
		{
			errOutput << "Unkown operation (" << operation_name << ")" << std::endl;
			error = true;
			return;
		}
		
		// Let's do something with it
		if (operation->getType() == Operation::script)
		{
			taskType = TASK_TYPE_SCRIPT; 
			
			if( !setupInputOutput(_cmdLine) )
			{
				error = true;
				return;
			}
			
			
			scriptLineCounter = 0;
			
			// Prepare replacement with arguments
			int position = 1;
			for (size_t i = 0 ; i < input_queues.size() ; i++)
			{
				std::ostringstream mark;
				mark << "$" << position++;
				replacements.insert( std::pair<std::string,std::string>( mark.str() , input_queues[i] ) );
			}
			for (size_t i = 0 ; i < output_queues.size() ; i++)
			{
				std::ostringstream mark;
				mark << "$" << position++;
				replacements.insert( std::pair<std::string,std::string>( mark.str() , output_queues[i] ) );
			}
			
		}
		else if (operation->getType() == Operation::generator)
		{
			taskType = TASK_TYPE_GENERATOR; 
			if( !setupInputOutput(_cmdLine) )
			{
				error = true;
				return;
			}
			
		}
		else
			taskType = TASK_TYPE_MAP_REDUCE; 

		
		
		// Mark as error if unknown operation
		if( taskType == TASK_TYPE_UNKNOWN)
		{
			errOutput << "Unknown operation ";
			error = true;
		}
		
	}
	
	void Task::prepareKVSets()
	{
		switch (taskType) {
			case TASK_TYPE_GENERATOR:
			case TASK_TYPE_SUB_MAP_REDUCE:
			case TASK_TYPE_SPREAD:
				startInputOtput();
				break;
			default:
				break;
		}
	}

	
	
	bool Task::runShow( )
	{
		KVSetVector*sets = groups[0];
		
		if( !sets )
			return true;	// Nothing to do
		
		for (size_t i = 0 ;  i < sets->size() ; i++)
		{
			KVSet *set = sets->at(i);
			
			DataInstance* keyData = ModulesManager::shared()->newDataInstance( set->getFormat().keyFormat );
			if( !keyData ){
				SSLogger::shared()->log( this , SSLogger::error , "Data type not supported " +  set->getFormat().keyFormat );
				return true;
			}
			
			DataInstance* valueData = ModulesManager::shared()->newDataInstance( set->getFormat().valueFormat );
			if( !valueData ){
				SSLogger::shared()->log( this , SSLogger::error , "Data type not supported " +  set->getFormat().valueFormat );
				return true;
			}

			KVFormat format = set->getFormat();
			KVSetVector sets( format );
			sets.push_back(set);
			KVReader *reader = new KVReader( &sets );
			
			for (size_t i = 0 ; i < std::min ( (size_t)100 , (size_t)reader->num_kvs ) ; i++)
			{
				keyData->parse(reader->kvs[i]->key );
				valueData->parse(reader->kvs[i]->value );
				
				std::cout << keyData->str() << " " << valueData->str() << std::endl;
				
			}
			
			delete reader;
		}
		
		
		return true;
	}
	
	bool Task::runGenerator()
	{
		assert( operation->getType() == Operation::generator );
		
		// Run the map operation
		Generator *generator = (Generator*) operation->getInstance();
		generator->run( writers );
		delete generator;
		
		finishInputOtput();
		
		return true;
	}
	
	
	bool Task::runMapReduce()
	{
		
		for (int i = 0 ; i < NUM_SETS_PER_STORAGE ; i++)
		{
			std::ostringstream task_command;
			task_command << _command << " -hash " << i << " -sub_operation";
			runCommand(task_command.str());
			
			
		}
		
		return true;	
		
	}
	
	bool Task::runSubMapReduce()
	{
		switch (operation->getType()) {
				
			case Operation::map:
			{					
				// Generate input superVector
				KVMultipleReader* superVector = new KVMultipleReader( groups );
				
				// Run the map operation
				Map *mapper = (Map*) operation->getInstance();
				
				mapper->run( superVector->getFullSets() , writers );
				
				
				delete mapper;
				delete superVector;
				
			}
				break;
				
			case Operation::reduce:
			{
				// Generate input superVector
				KVMultipleReader* superVector = new KVMultipleReader( groups );
				
				// Run the map operation
				Reduce *reducer = (Reduce*) operation->getInstance();
				
				// Sort and group inputs
				superVector->initReduceSets();
				
				KVSET * sets = superVector->getNextReduceSets();
				while( sets )
				{
					reducer->run(sets , writers );
					sets = superVector->getNextReduceSets();
				}
				
				delete reducer;
				delete superVector;
				
			}
				break;
				
			default:
				assert( false );	//Not implemented...?
				break;
		}
		
		finishInputOtput();
		return true;
	}	
	
	bool Task::runScript()
	{
		assert( operation->getType()== Operation::script );

		
		while( scriptLineCounter < (int)operation->code.size() )
		{
			std::string command = operation->code[scriptLineCounter++];
			AUCommandLine cmdLine; 
			cmdLine.parse(command );
			
			if ( cmdLine.get_num_arguments() >= 0)
			{
				if ( cmdLine.get_argument(0) == "add_queue" )
				{
					if( cmdLine.get_num_arguments() < 4 )
					{
						SSLogger::log( this , SSLogger::error, "Usage: add_queue name <keyFormat> <valueFormat>\n");
						return quitScript();
					}
					
					std::string name = cmdLine.get_argument( 1 );
					std::string keyFormat= cmdLine.get_argument( 2 );
					std::string valueFormat = cmdLine.get_argument( 3 );
					std::ostringstream queue_name;
					queue_name << "q_task_" << getId() << "_" << name;
					
					if( !ModulesManager::shared()->checkData( keyFormat ) )
					{
						std::stringstream message;
						message << "Unsupported data format " << keyFormat << " (line " << (scriptLineCounter-1) ;
						SSLogger::log( this , SSLogger::error, message.str()  );
						return quitScript();
					}
					if( !ModulesManager::shared()->checkData( valueFormat ) )
					{
						std::stringstream message;
						message << "Unsupported data format " << valueFormat << " (line " << (scriptLineCounter-1) ;
						return quitScript();
					}
					
					KVManager::shared()->newKVQueue( getId() ,   queue_name.str(), KVFormat::format( keyFormat , valueFormat ) );
					tmp_queues.push_back( queue_name.str() );
					replacements.insert( std::pair<std::string,std::string>( name , queue_name.str() ) );
				}
				else {
					
					std::string new_command = command;
					
					// Replace string process
					for (std::map <std::string,std::string>::iterator i = replacements.begin() ; i != replacements.end() ; i++)
						replace_string( new_command , i->first , i->second );
					
					runCommand( new_command );
					return false; // this will ensure that the script is no longer executed until this task is finished
				}
			}			 
		}
		
		// Finish!
		return quitScript();
		
		
	}
	
	
	bool Task::runSpread()
	{
		// Generate input superVector
		KVMultipleReader* superVector = new KVMultipleReader( groups );
		KVSET *sets = superVector->getFullSets();

		// Take the common format to identify key-value
		KVFormat format = input_queues_format[0];

		DataInstance* keyData = ModulesManager::shared()->newDataInstance( format.keyFormat);
		if( !keyData ){
			SSLogger::shared()->log( this , SSLogger::error , "Data type not supported " +  format.keyFormat );
			return true;
		}
		
		DataInstance* valueData = ModulesManager::shared()->newDataInstance( input_queues_format[0].valueFormat );
		if( !valueData ){
			SSLogger::shared()->log( this , SSLogger::error , "Data type not supported " +  format.valueFormat );
			return true;
		}
		
		
		for ( size_t i = 0 ; i < sets[0].num_kvs ; i++ )
		{
			keyData->parse(sets[0].kvs[i]->key );
			valueData->parse(sets[0].kvs[i]->value );

			writers[0]->emit( keyData , valueData );
		}
		

		finishInputOtput();

		
		return true;

	}
	
	
	void Task::runCommand( std::string command )
	{
		TaskManager::shared()->runCommand( this , command ,  true );
	}
	
	
	
	std::string Task::str()
	{
		std::ostringstream o;
		
		o << "[" << au::Format::int_string( getId() , 6 ) << "]";
		Task *parentTask = getParentTask();
		if( parentTask )
			o << "[" << parentTask->getId() << "]";
		else
			o << "[------]";
		
		o << "[";
		switch (status) {
			case definition:	o << "D"; break;
			case in_queue:		o << "Q"; break;
			case running:		o << "R"; break;
			case waiting:		o << "W"; break;
			case finished:		o << "F"; break;
		}
		
		o << "] ";
		o << "[ Children: " << numberChildren << " ] ";
/*
		o << "Sets: ";
		for (size_t i = 0 ; i < sets.size() ; i++)
			o << sets[i]->id << " ";
*/		
 
		o << " Command:" << _command;
		return o.str();
	}	
	
}
