

#include "ControllerDataManager.h"			// Own interface
#include "ControllerTask.h"					// ControllerTask
#include "au/CommandLine.h"					// au::CommandLine
#include "samson/module/KVFormat.h"				// samson::KVFormat
#include "Queue.h"				// samson::Queue

#include "au/map.h"							// au::insertInMap
#include "au/string.h"                      // au::Format

#include "samson/common/EnvironmentOperations.h"			// Own definition of this methods

#include "samson/data/DataManager.h"					// samson::DataManagerCommandResponse
#include "samson/controller/SamsonController.h"				// samson::SamsonController
#include "samson/common/coding.h"						// samson::KVInfo
#include "samson/module/ModulesManager.h"					// Utility functions
#include "samson/module/Operation.h"				// samson::Operation
#include "QueueFile.h"						// samson::QueueFile
#include "samson/common/samsonDirectories.h"				// SAMSON_CONTROLLER_DIRECTORY
#include "samson/common/MessagesOperations.h"				// evalHelpFilter(.)
#include "samson/common/SamsonSetup.h"					// samson::SamsonSetup

namespace samson {
	
#pragma mark ----
	
	void ActiveTask::addFiles( Queue *q )
	{
		// Add the files from this queue
		for ( au::map< std::string , QueueFile >::iterator i = q->files.begin() ; i != q->files.end() ; i++ )
			files.insert( i->first );
	}
    
#pragma mark ----

    
	ControllerDataManager::ControllerDataManager( SamsonController * _controller ) : DataManager( getLogFileName() )
	{
        // Pointer to the contorller
        controller = _controller;
        
		info_kvs.clear();
		info_txt.clear();
	}
	
	ControllerDataManager::~ControllerDataManager()
	{
		// Clear all queues
		_clear();
	}
	
	void ControllerDataManager::copyQueue( Queue * q_from , network::FullQueue * q_to )
	{
		 network::Queue *q = q_to->mutable_queue();
		 
		 // Set the name
		 q->set_name( q_from->name() );
		 
		 // Format
		 fillKVFormat( q->mutable_format() , q_from->format() );
		 
		 //Info
		 fillFullKVInfo( q->mutable_info(), q_from->info() );
		 
		 // Add file information
		au::map< std::string , QueueFile >::iterator iter;
		 
		 for ( iter = q_from->files.begin() ; iter != q_from->files.end() ; iter++)
		 {
			 network::File *file = q_to->add_file();
			 (iter->second)->fill( file );
		 }		
	}

	void ControllerDataManager::_beginTask( size_t task )
	{
		//Create the structure for this task
		
		ActiveTask* active_task = tasks.extractFromMap( task );
		if( active_task )
			delete active_task;
		
		tasks.insertInMap( task , new ActiveTask() );
	}
	
	void ControllerDataManager::_cancelTask( size_t task )
	{
		// Extract the list of files for this operation
		ActiveTask* active_task = tasks.extractFromMap( task );
		if( active_task )
			delete active_task;
	}
	
	void ControllerDataManager::_finishTask( size_t task )
	{
		// Extract the list of files for this operation
		ActiveTask* active_task = tasks.extractFromMap( task );
		if( active_task )
			delete active_task;
	}
	
	void ControllerDataManager::_clear(  )
	{
		info_kvs.clear();
		info_txt.clear();
		
		queues.clearMap();
		tasks.clearMap();
		
	}
	
	
	DataManagerCommandResponse ControllerDataManager::_run( size_t task_id, std::string command )
	{
        ActiveTask *active_task = tasks.findInMap( task_id );

        if(!active_task)
            LM_X(1,("Internal error. Non active task %lu" , task_id ));
        
        
		DataManagerCommandResponse response;
		
		au::CommandLine  commandLine;
		commandLine.set_flag_boolean("f");          // Force boolean flag to avoid error when creating queue
		commandLine.set_flag_boolean("txt");        // when adding txt data sets
		commandLine.set_flag_string("tag","");      // when removing automatic operation
		
		commandLine.parse( command );
		
		bool txt_queue = commandLine.get_flag_bool("txt");
		bool forceFlag = commandLine.get_flag_bool("f");
		
		
		if( commandLine.get_num_arguments() == 0)
		{
			response.output = "No command found";
			response.error = true;
			return response;
		}

		if( commandLine.get_argument(0) == "add" )
		{
			
			int num_min_parameters = txt_queue?2:4;
			
			if( commandLine.get_num_arguments() < num_min_parameters )
			{
				response.output = "Usage: add name <keyFormat> <valueFormat> or add name -txt for txt sets";
				response.error = true;
				return response;
			}

			std::string name		= commandLine.get_argument( 1 );
			std::string keyFormat;
			std::string	valueFormat;
			
			if( !txt_queue )
			{
				keyFormat	= commandLine.get_argument( 2 );
				valueFormat = commandLine.get_argument( 3 );
				
				if( !ModulesManager::shared()->checkData( keyFormat ) )
				{
					response.output = "Unsupported data format " + keyFormat;
					response.error = true;
					return response;
				}
				
				if( !ModulesManager::shared()->checkData( valueFormat ) )
				{
					std::ostringstream output;
					output << "Unsupported data format " + valueFormat;
					response.output = output.str();
					response.error = true;
					return response;
				}
				
			}
			else
			{
				keyFormat = "txt";
				valueFormat = "txt";
			}
			
			Queue *_queue = queues.findInMap( name );
			
			// Check if queue exist
			if( _queue != NULL )
			{
				if( forceFlag )
				{
					// Check the same format
					if( ( _queue->_format.keyFormat == keyFormat ) && ( _queue->_format.valueFormat == valueFormat ) )
					{
						response.output = "OK, queue already existed, but ok";
						return response;
					}
					else
					{
						std::ostringstream output;
						output << "Queue " + name + " already exist with another formats: (" << _queue->_format.str() << ").( Option -f is not enougth ).";
						response.output = output.str();
						response.error = true;
						return response;
						
					}
				}
				else
				{
					std::ostringstream output;
					output << "Queue " + name + " already exist";
					response.output = output.str();
					response.error = true;
					return response;
				}
			}			
			
			KVFormat format = KVFormat::format( keyFormat , valueFormat );
			Queue *tmp = new Queue(name , format);
			queues.insertInMap( name , tmp );

			response.output = "OK";
			return response;
		}

        
        if( commandLine.get_argument(0) == "rm_stream_operation" )
        {
            // Remove a particular stream operation
            
			
			if( commandLine.get_num_arguments() < 2 )
			{
				response.output = "Usage: rm_stream_operation name ";
				response.error = true;
				return response;
			}
            
			std::string name            = commandLine.get_argument( 1 );
			std::string operation       = commandLine.get_argument( 2 );
            
            
            // Check it the queue already exists
            network::StreamOperation *_operation = streamOperations.extractFromMap( name );
			
			// Check if queue exist
			if( _operation  )
                delete _operation;
            else
            {
				response.output = au::str("StreamOperation %s does not exist" , name.c_str() );
				response.error = true;
				return response;
            }
            
			response.output = "OK";
			return response;            
        }
        
        if( commandLine.get_argument(0) == "add_stream_operation" )
		{
			
			
			if( commandLine.get_num_arguments() < 3 )
			{
				response.output = "Usage: add_stream_operation name operation input1 input2 ... outputs1 outputs2 output3";
				response.error = true;
				return response;
			}
            
			std::string name            = commandLine.get_argument( 1 );
			std::string operation       = commandLine.get_argument( 2 );


            // Check it the queue already exists
            network::StreamOperation *_operation = streamOperations.findInMap( name );
			
			// Check if queue exist
			if( _operation != NULL )
			{
                std::ostringstream output;
                output << "StreamOperation " + name + " already exist";
                response.output = output.str();
                response.error = true;
                return response;
			}			
            
            // Check the operation
            
            Operation* op = ModulesManager::shared()->getOperation(operation);
            
            if( !op )
            {
                std::ostringstream output;
                output << "Unsupported operation " + operation;
                response.output = output.str();
                response.error = true;
                return response;
            }

            
            switch ( op->getType() ) {
                    
                case Operation::parser:
                    
                    // Check with the number of outputs
                    if( commandLine.get_num_arguments() < ( 3 + op->getNumOutputs() ) )
                    {
                        std::ostringstream output;
                        output << "Not enough outputs for operation " + operation + ". It has " << op->getNumOutputs() << " outputs";
                        response.output = output.str();
                        response.error = true;
                        return response;
                        
                    }
                    
                    break;

                case Operation::map:
                    
                    // Check with the number of outputs
                    if( commandLine.get_num_arguments() < ( 3 + op->getNumOutputs() ) )
                    {
                        std::ostringstream output;
                        output << "Not enough outputs for operation " + operation + ". It has " << op->getNumOutputs() << " outputs";
                        response.output = output.str();
                        response.error = true;
                        return response;
                        
                    }
                    
                    break;

                case Operation::parserOut:
                    
                    // Check with the number of outputs
                    if( commandLine.get_num_arguments() < ( 3 + op->getNumOutputs() ) )
                    {
                        std::ostringstream output;
                        output << "Not enought outputs for operation " + operation + ". It has " << op->getNumOutputs() << " outputs";
                        response.output = output.str();
                        response.error = true;
                        return response;
                        
                    }
                    
                    break;
                    
                    
                case Operation::reduce:
                {
                    if( op->getNumInputs() != 2 )
                    {
                        response.output = au::str("Only reduce operations with 2 inputs are supported at the moment. ( In the furure, reducers with 3 or more inputs will be supported.");
                        response.error = true;
                        return response;
                    }
                    
                    KVFormat a = op->getInputFormats()[op->getNumInputs() - 1];
                    KVFormat b = op->getOutputFormats()[ op->getNumOutputs() - 1 ];

                    if( !a.isEqual(b)  )
                    {
                        response.output = au::str("Last input and output should be the same data type to qualify as stream-reduce");
                        response.error = true;
                        return response;                        
                    }
                    
                    
                    // Check with the number of outputs
                    if( commandLine.get_num_arguments() < ( 3 + op->getNumInputs() + op->getNumOutputs() ) )
                    {
                        std::ostringstream output;
                        output << "Not enought outputs specified for operation " << operation << ". It has " << op->getNumOutputs() << " inputs and " << op->getNumOutputs() << " outputs.";
                        response.output = output.str();
                        response.error = true;
                        return response;
                        
                    }
                     
                    // Check that the last input and the last output are indeed the same
                    
                    std::string last_input = commandLine.get_argument( 3 + op->getNumInputs() - 1 );
                    std::string last_output = commandLine.get_argument( 3 + op->getNumInputs() + op->getNumOutputs() -1 );
                    if(  last_input !=  last_output )
                    {
                        std::ostringstream output;
                        output << "Last input and last output should be the same state. ( " << last_input << " != " << last_output << ")";
                        response.output = output.str();
                        response.error = true;
                        return response;
                        
                    }
                    
                }
                    break;
                    
                case Operation::script:
                {
                    std::ostringstream output;
                    output << "Script operations cannot be used to process stream queues. Only parsers, maps and spetial reducers";
                    response.output = output.str();
                    response.error = true;
                    return response;
                    
                }
                    
                    break;
                    
                default:
                {
                    std::ostringstream output;
                    output << "Operation type is currently not supported... coming soon!";
                    response.output = output.str();
                    response.error = true;
                    return response;
                    
                }
                    break;
            }
            
            
            // Create the new queue
			
            network::StreamOperation *tmp = new network::StreamOperation();
            tmp->set_name(name);
            tmp->set_operation( operation );

            tmp->set_num_workers( controller->network->getNumWorkers() );
            
            int num_inputs  = op->getNumInputs();
            int num_outputs = op->getNumOutputs();

            
			if( commandLine.get_num_arguments() < ( 3 + num_inputs + num_outputs ) )
            {
                response.output = 
                    au::str("Not enougth parameters for this operation. Required %d inputs and &d outputs" , 
                                num_inputs , num_outputs );
                response.error = true;
                return response;
            }
                        
            for (int i = 0 ; i < num_inputs ; i++ )
            {
                std::string queue_name = commandLine.get_argument( 3 + i );
                tmp->add_input_queues( queue_name );
            }
            
            for (int i = 0 ; i < num_outputs ; i++ )
            {
                std::string queue_name = commandLine.get_argument( 3 + num_inputs + i );
                tmp->add_output_queues( queue_name );
            }
            
            // Insert the queue in the global list of stream-queues
			streamOperations.insertInMap( name , tmp );
            
            
			response.output = "OK";
			return response;
		}

        if( commandLine.get_argument(0) == "set_stream_operation_property" )
		{
			
			
			if( commandLine.get_num_arguments() < 4 )
			{
				response.output = "Usage: set_stream_operation_property stream_operation_name property value";
				response.error = true;
				return response;
			}
            
			std::string name            = commandLine.get_argument( 1 );
			std::string property        = commandLine.get_argument( 2 );
			std::string value           = commandLine.get_argument( 3 );
            
            
            // Check it the queue already exists
            network::StreamOperation *stream_operation = streamOperations.findInMap( name );
			
			// Check if queue exist
			if( stream_operation == NULL )
			{
                std::ostringstream output;
                output << "Stream Operation " + name + " not found";
                response.output = output.str();
                response.error = true;
                return response;
			}			

            // Add a propery here
            network::EnvironmentVariable* ev = stream_operation->mutable_environment()->add_variable();
            ev->set_name( property );
            ev->set_value( value );
            
			response.output = "OK";
			return response;
		}
		


		if( commandLine.isArgumentValue(0, "remove_all" , "remove_all" ) )
		{
			// remove completelly queues
			queues.clearMap();
            
            // remove completelly stream_queues
            streamOperations.clearMap();

			// Clear the total counter of data
			info_kvs.clear();
			info_txt.clear();
			
			response.output = "OK";
			return response;
			
		}
		
		if( commandLine.isArgumentValue(0, "rm" , "rm" ) )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 2 )
			{
				response.output = "Usage: rm name";
				response.error = true;
				return response;
			}
			
			for (int i = 1 ; i < commandLine.get_num_arguments() ; i++)
			{
				
				std::string name = commandLine.get_argument( i );
				
				// Check if queue exist
				Queue *tmp =  queues.extractFromMap(name);
				if( !tmp )
				{
					if( !forceFlag )
					{
						std::ostringstream output;
						output << "Queue " + name + " does not exist";
						response.output = output.str();
						response.error = true;
						return response;
					}
				}
				else
				{
					// Add the removed files to the associated list to be preserved until the task is not completed
                    active_task->addFiles( tmp );
					delete tmp;
				}
			}
			
			response.output = "OK";
			return response;
		}

		if( commandLine.isArgumentValue(0, "check" , "check" ) )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 2 )
			{
				response.output = "Usage: check name";
				response.error = true;
				return response;
			}
			
			for (int i = 1 ; i < commandLine.get_num_arguments() ; i++)
			{
				
				std::string name = commandLine.get_argument( i );
				
				// Check if queue exist
				Queue *tmp =  queues.findInMap(name);
				if( !tmp )
				{
					if( !forceFlag )
					{
						std::ostringstream output;
						output << "Queue " + name + " does not exist";
						response.output = output.str();
						response.error = true;
						return response;
					}
				}
			}
			
			response.output = "OK";
			return response;
		}
		
		
		if( commandLine.isArgumentValue(0, "clear" , "clear" ) )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 2 )
			{
				response.output = "Usage: clear name1 name2 ...";
				response.error = true;
				return response;
			}

			for (int i  = 1 ; i < commandLine.get_num_arguments() ; i++)
			{
				std::string name = commandLine.get_argument( i );
				
				// Check if queue exist
				Queue *tmp =  queues.findInMap(name);
				if( !tmp )
				{
					std::ostringstream output;
					output << "Queue " + name + " does not exist";
					response.output = output.str();
					response.error = true;
					return response;
				}
				else
				{
					// Protect this files until task is confirmed
                    active_task->addFiles( tmp );
					tmp->clear();
				}
			}
			
			response.output = "OK";
			return response;
		}

#pragma mark Change the name of a queue
		
		if( commandLine.get_argument(0) == "mv" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 3 )
			{
				response.output = "Usage: mv name name2";
				response.error = true;
				return response;
			}
			
			std::string name = commandLine.get_argument( 1 );
			std::string name2 = commandLine.get_argument( 2 );
			
			// Check if queue exist
			Queue *tmp =  queues.findInMap(name);
			Queue *tmp2 =  queues.findInMap(name2);

			if( !tmp )
			{
				std::ostringstream output;
				output << "Queue " + name + " does not exist";
				response.output = output.str();
				response.error = true;
				return response;
			} else if( tmp2 )
			{
				std::ostringstream output;
				output << "Queue " + name2 + " exist. Please, remove it first with remove_queue command";
				response.output = output.str();
				response.error = true;
				return response;
			}
			else
			{
				Queue *original_queue =  queues.extractFromMap(name);
				original_queue->rename( name2 );
				queues.insertInMap( name2 , original_queue );
			}
			
			response.output = "OK";
			return response;
		}		

#pragma mark Duplicate a queue
		
		if( commandLine.get_argument(0) == "dup" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 3 )
			{
				response.output = "Usage: dup name name2";
				response.error = true;
				return response;
			}
			
			std::string name = commandLine.get_argument( 1 );
			std::string name2 = commandLine.get_argument( 2 );
			
			// Check if queue exist
			Queue *tmp =  queues.findInMap(name);
			Queue *tmp2 =  queues.findInMap(name2);
			
			if( !tmp )
			{
				std::ostringstream output;
				output << "Queue " + name + " does not exist";
				response.output = output.str();
				response.error = true;
				return response;
			}
			else if( tmp2 )
			{
				std::ostringstream output;
				output << "Queue " + name2 + " exist. Please, remove it first with remove_queue command";
				response.output = output.str();
				response.error = true;
				return response;
			}
			else
			{
				KVFormat format = tmp->format();
				tmp2 = new Queue(name2 , format);
				tmp2->copyFileFrom(tmp);
				queues.insertInMap( name2 , tmp2 );
				
			}
			
			response.output = "OK";
			return response;
		}	
		
#pragma mark Copy content from one queue to another
		
		if( commandLine.get_argument(0) == "cp" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 3 )
			{
				response.output = "Usage: cp queue_from name_to";
				response.error = true;
				return response;
			}
			
			std::string name = commandLine.get_argument( 1 );
			std::string name2 = commandLine.get_argument( 2 );
			
			// Check if queue exist
			Queue *tmp =  queues.findInMap(name);
			Queue *tmp2 =  queues.findInMap(name2);
			
			if( !tmp )
			{
				std::ostringstream output;
				output << "Queue " << name << " does not exist";
				response.output = output.str();
				response.error = true;
				return response;
			} 

			if( !tmp2 )
			{
				std::ostringstream output;
				output << "Queue " << name << " does not exist";
				response.output = output.str();
				response.error = true;
				return response;
			} 
			

			// check formats
			KVFormat f1 = tmp->format();
			KVFormat f2 = tmp->format();
			
			if( ! f1.isEqual( f2 ) )
			{
				std::ostringstream output;
				output << "Queues " << name << " and " << name2 << " does not have the same format (key-values).";
				response.output = output.str();
				response.error = true;
				return response;
			}
			
			
			// copy content
			tmp2->copyFileFrom( tmp );
			
			response.output = "OK";
			return response;
		}				
		
		
#pragma mark Add data		
		
		if( commandLine.get_argument(0) == "add_data_file" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 5 )
			{
				response.output = "Usage: add_data_file worker fileName size queue";
				response.error = true;
				return response;
			}
			
			int worker = (int)strtol(commandLine.get_argument( 1 ).c_str(), (char **)NULL, 10);
			
			std::string fileName	= commandLine.get_argument( 2 );
			size_t size				= strtoll(commandLine.get_argument( 3 ).c_str(), (char **)NULL, 10);
				
			info_txt.append( size , 1 );			
			
			std::string queue		= commandLine.get_argument( 4 );
			
			// Check valid queue
			Queue *q =  queues.findInMap(queue);
			if( !q )
			{
				std::ostringstream output;
				output << "Data Queue " << queue << " does not exist";
				response.output = output.str();
				response.error = true;
				return response;
			}

			q->addFile( worker , fileName , KVInfo(size,1) );
			response.output = "OK";
			return response;
		}			

#pragma mark add files to queues
		
		if( commandLine.get_argument(0) == "add_file" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 6 )
			{
				response.output = "Usage: add_file worker fileName size kvs queue";
				response.error = true;
				return response;
			}
			
			int worker = (int)strtol(commandLine.get_argument( 1 ).c_str(), (char **)NULL, 10);
			
			std::string fileName	= commandLine.get_argument( 2 );

			size_t size = strtoll(commandLine.get_argument( 3 ).c_str(), (char **)NULL, 10);
			size_t kvs = strtoll(commandLine.get_argument( 4 ).c_str(), (char **)NULL, 10);
			std::string queue		= commandLine.get_argument( 5 );
			
			// Check valid queue
			Queue *q =  queues.findInMap(queue);
			if( !q )
			{
				std::ostringstream output;
				output << "Queue " << queue << " does not exist";
				response.output = output.str();
				response.error = true;
				return response;
			}
			
			info_kvs.append( size , kvs );			
			q->addFile( worker , fileName , KVInfo( size , kvs)  );
			response.output = "OK";
			return response;
		}				
		
		
		if( commandLine.get_argument(0) == "remove_file" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 6 )
			{
				response.output = "Usage: remove_file worker fileName size kvs queue";
				response.error = true;
				return response;
			}
			
			int worker = (int)strtol(commandLine.get_argument( 1 ).c_str(), (char **)NULL, 10);
			
			std::string fileName	= commandLine.get_argument( 2 );
			
			size_t size = strtoll(commandLine.get_argument( 3 ).c_str(), (char **)NULL, 10);
			size_t kvs = strtoll(commandLine.get_argument( 4 ).c_str(), (char **)NULL, 10);
			std::string queue		= commandLine.get_argument( 5 );
			
			// Check valid queue
			Queue *q =  queues.findInMap(queue);
			if( !q )
			{
				std::ostringstream output;
				output << "Queue " << queue << " does not exist";
				response.output = output.str();
				response.error = true;
				return response;
			}
			
			if( q->removeFile( worker , fileName , KVInfo( size , kvs)  ) )
			{
				info_kvs.remove( size , kvs );			
				response.output = "OK";
				return response;
			}
			else
			{
				response.output = "File not found in this queue";
				response.error = true;
				return response;
			}
		}				
		
		
		response.error = true;
		response.output = std::string("Unknown command: ") +  command;
		return response;
	}
	
	
	
	std::string ControllerDataManager::getLogFileName(   )
	{
		return SamsonSetup::shared()->logDirectory + "/log_controller";
	}
	
	std::string ControllerDataManager::getStatus()
	{
		/*
		std::ostringstream o;
		lock.lock();
		o << getStatusFromArray( queues );
		lock.unlock();
		return o.str();
		 */
		
		return "";
		
	}		

	
	void ControllerDataManager::fill( network::QueueList *ql , std::string command)
	{
		au::CommandLine cmdLine;
		cmdLine.set_flag_string("begin" , "");
		cmdLine.set_flag_string("end" , "");
		cmdLine.parse(command);
		
		std::string begin = cmdLine.get_flag_string("begin");
		std::string end = cmdLine.get_flag_string("end");
		
		lock.lock();

		// List of data-sets
        {
            std::map< std::string , Queue*>::iterator i;
            for (i = queues.begin() ; i!= queues.end() ;i++)
            {
                Queue *queue = i->second;
                
                if( filterName( i->first , begin, end) )
                {                
                    network::FullQueue *fq = ql->add_queue();
                    queue->fill( fq );
                }
            }
        }

		// List of active tasks ( list of files that should not be removed )
		std::map< size_t , ActiveTask*>::iterator t;
		for (t = tasks.begin() ; t!= tasks.end() ;t++)
		{
			network::ActiveTask* at = ql->add_tasks();
			for ( std::set<std::string>::iterator f = (t->second)->files.begin() ; f != (t->second)->files.begin() ; f++)
				at->add_filename( *f );
		}
		
		// List of stream queues
        network::StreamOperationList *ol = ql->mutable_stream_operation_list();
        {
            std::map< std::string , network::StreamOperation*>::iterator i;
            for (i = streamOperations.begin() ; i!= streamOperations.end() ;i++)
            {
                network::StreamOperation * streamOperation = i->second;
                
                if( filterName( i->first , begin, end) )
                {                
                    network::StreamOperation *so = ol->add_operation();
                    so->CopyFrom(*streamOperation);
                }
            }
        }
		
		lock.unlock();		
	}
    
    
	
	void ControllerDataManager::fill( network::DownloadDataInitResponse* response , std::string queue )
	{
		lock.lock();
		
		Queue * q = queues.findInMap( queue );
		if( q )
			copyQueue( q , response->mutable_queue() );
		
		lock.unlock();
	}
    
    
#pragma mark
    
    // Take a select stament and complete with -key_format XXX -value_format XXX acording to the input queue 
    
    void ControllerDataManager::completeSelect( std::string& command , au::ErrorManager& error )
    {
        // this retains the locker
        au::Locker locker( &lock );
        
        // Fill inputs and outputs of a select operation
        // Format of the command select path_to_key path_to_value from to
        au::CommandLine cmdLine;
        cmdLine.set_flag_boolean("create");
        cmdLine.parse( command );
        
        if ( cmdLine.get_num_arguments() < 4 )
        {
            error.set("Not enougth parameters for the select sentence. Usage: select fromQueue toQueue key_path value_path");
            return;
        }

        std::string path_key    = cmdLine.get_argument(3);
        std::string path_value  = cmdLine.get_argument(4);
        
        std::string fromQueue   = cmdLine.get_argument(1);
        std::string toQueue   = cmdLine.get_argument(1);

        Queue *q = queues.findInMap( fromQueue );
        
        if( !q )
        {
            error.set( au::str("Select: Unkwown queue %s" , fromQueue.c_str() ) );
            return;
        }
        

        // Take the format of the input queue
        KVFormat format = q->format();

        command.append(au::str(" -input_key_format %s " , format.keyFormat.c_str() ) );
        command.append(au::str(" -input_value_format %s " , format.valueFormat.c_str() ) );
        

        Data *key_data = ModulesManager::shared()->getData( format.keyFormat );
        Data *value_data = ModulesManager::shared()->getData( format.keyFormat );
       
        if( !key_data || ! value_data )
        {
            error.set( "Internal error related with current key-value of existing queue" );
            return;
        }
            
        DataInstance *key_data_instance = (DataInstance *)key_data->getInstance();
        DataInstance *value_data_instance = (DataInstance *)value_data->getInstance();

        
        if( !key_data_instance || ! value_data_instance )
        {
            error.set( "Internal error related with current key-value of existing queue" );
            return;
        }

        LM_M(("Data instance for key: %s" , key_data_instance->getTypeFromPath("").c_str() ));
        LM_M(("Data instance for value: %s" , value_data_instance->getTypeFromPath("").c_str() ));
        
        std::string output_key_format; 
        std::string output_value_format; 


        // Get the data for the output key
        if( au::getRoot( path_key ) == "key" )
            output_key_format = key_data_instance->getTypeFromPath( au::getRest( path_key ) );
        else if( au::getRoot( path_key ) == "value" )
            output_key_format = value_data_instance->getTypeFromPath( au::getRest( path_key ) );
        else
        {
            delete key_data_instance;
            delete value_data_instance;
            error.set( au::str( "Path %s not valid. Should start with 'value' or 'key' " , path_key.c_str() ) );
            return;
        }
        
        // Get the data type for the output value
        if( au::getRoot( path_value ) == "key" )
            output_value_format = key_data_instance->getTypeFromPath( au::getRest( path_value ) );
        else if( au::getRoot( path_value ) == "value" )
            output_value_format = value_data_instance->getTypeFromPath( au::getRest( path_value ) );
        else
        {
            delete key_data_instance;
            delete value_data_instance;
            error.set( au::str( "Path %s not valid. Should start with 'value' or 'key' " , path_key.c_str() ) );
            return;
        }
        
        
        // This is the input format
        LM_M(("Output formats: %s %s" , output_key_format.c_str() , output_value_format.c_str() ));
        
        command.append(au::str(" -output_key_format %s " , output_key_format.c_str() ) );
        command.append(au::str(" -output_value_format %s " , output_value_format.c_str() ) );
        
        command.append( " -select_complete "  );
        
        delete key_data_instance;
        delete value_data_instance;

        
        //selectOperation->error.set("No continue at the moment...");
        
        
    }

    
#pragma mark
	
	void ControllerDataManager::retreveInfoForTask( size_t job_id , ControllerTaskInfo *info  )		
	{
		lock.lock();
		_retreveInfoForTask( job_id, info );
		
		lock.unlock();
		
	}
	
	void ControllerDataManager::_retreveInfoForTask( size_t job_id , ControllerTaskInfo *info )		
	{
		std::ostringstream error_message;
		
		// Check inputs	
		for (int i = 0 ; i < (int)info->inputs.size() ; i++)
		{
			std::string queue_name =  info->inputs[i] ;
			Queue *q = queues.findInMap( queue_name );
			
			if( q )
			{
				KVFormat queue_format		= q->format();
				KVFormat parameter_format	= info->operation->getInputFormat(i);
				
				if( !queue_format.isEqual( parameter_format ) )
				{
					error_message << "Wrong format for queue " << queue_name << " (" << queue_format.str() << " vs " << parameter_format.str() << ")";
					info->error.set( error_message.str() );
					return; 
				}
				
				// add queu to be emitted in the WorkerTask packet
                network::FullQueue *fq = new network::FullQueue();
				q->fill( fq );
                
				info->input_queues.push_back( fq ); 
				
				
			}
			else
			{
				error_message << "Unknown queue " << info->inputs[i];
				info->error.set( error_message.str() );
				return; 
			}
		}

		// Check too many outputs error
        int max_num_outputs = SamsonSetup::getInt("general.max_parallel_outputs"); 
        
		if( info->operation->getType() != Operation::script )
			if( (int)info->outputs.size() > max_num_outputs )
			{
				error_message << "Operation with too many outputs ( " << info->outputs.size() << " ). The limit in this SAMSON cluster is " << max_num_outputs;
				info->error.set( error_message.str() );
				return; 
			}
		
		// Check output	
		for (int i = 0 ; i < (int)info->outputs.size() ; i++)
		{
			std::string queue_name = info->outputs[i];
			
			Queue *q = queues.findInMap( queue_name );
            KVFormat parameter_format = info->operation->getOutputFormat(i);
            
            
            if( !q && info->flag_create )
            {
                // Create the queue on demand
                std::ostringstream command;
                command << "add " << queue_name << " " << parameter_format.keyFormat << " " << parameter_format.valueFormat;
                DataManagerCommandResponse ans =  _runOperation( job_id , command.str() );
                
                // Find again
                q = queues.findInMap( queue_name );

                if( !q )
                {
                    info->error.set( au::str("Queue %s not found even after trying to add for the -create flag" , queue_name.c_str() ) );
                    return; 
                }
            }
            
			if( q )
			{
				KVFormat queue_format = q->format();
				
				if( !queue_format.isEqual( parameter_format ) )
				{
					error_message << "Wrong format for queue " << queue_name << " (" << queue_format.str() << " vs " << parameter_format.str() << ")";
					info->error.set( error_message.str() );
					return; 
				}

				// add queu to be emitted in the WorkerTask packet
				network::FullQueue *fq = new network::FullQueue();
                q->fill(fq);
				info->output_queues.push_back( fq ); 
				
			}
			else
			{
				error_message << "Unknown queue " << info->outputs[i];
				info->error.set( error_message.str() );
				return; 
			}
		}
        
        // Crear outputs if necessary
        if( info->flag_clear )
		{
			std::ostringstream command;
			command << "clear ";
			for (size_t i = 0 ; i < info->outputs.size() ; i++ )
				command << info->outputs[i] << " ";
            
			DataManagerCommandResponse ans =  _runOperation( job_id , command.str() );
		}

        
		
	}
	
    void ControllerDataManager::getInfo( std::ostringstream& output)
    {
        au::xml_open(output,"data_manager");
        
        output << "<queues>\n";

		au::map< std::string , Queue>::iterator queue;
        for ( queue = queues.begin() ; queue != queues.end() ; queue++ )
            queue->second->getInfo( output );
        output << "</queues>\n";
        
        output << "<stream_operations>\n";
        
        au::map< std::string , network::StreamOperation >::iterator so;
        for (so = streamOperations.begin() ; so != streamOperations.end() ; so++)
        {
            output << "<stream_operation>\n";
            
            output << "<name>" << so->second->name() << "</name>";
            const network::StreamOperation& stream_operation = *so->second; 
            output << "<description>" << samson::getInfo( stream_operation ) << "</description>";
            
            output << "</stream_operation>\n";
        }
        
        output << "</stream_operations>\n";
        

        au::xml_close(output,"data_manager");
        
        
    }
	
#pragma mark Monitorization
	
	void ControllerDataManager::getQueuesMonitorInfo( std::vector<QueueMonitorInfo> &output_queues )
	{
		lock.lock();
		
		au::map< std::string , Queue>::iterator iter;
		for ( iter = queues.begin() ; iter != queues.end() ; iter++)
			output_queues.push_back( iter->second->getQueueMonitorInfo() );
		
		lock.unlock();
		
	}	
	
	
	
}
