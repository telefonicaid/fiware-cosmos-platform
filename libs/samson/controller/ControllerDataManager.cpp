

#include "ControllerDataManager.h"			// Own interface
#include "ControllerTask.h"					// ControllerTask
#include "au/CommandLine.h"					// au::CommandLine
#include "samson/module/KVFormat.h"				// samson::KVFormat
#include "Queue.h"				// samson::Queue
#include "au/map.h"							// au::insertInMap
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
	

    
	ControllerDataManager::ControllerDataManager( ) : DataManager( getLogFileName() )
	{
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

#pragma mark ----
	
	void ActiveTask::addFiles( Queue *q )
	{
		// Add the files from this queue
		for ( au::map< std::string , QueueFile >::iterator i = q->files.begin() ; i != q->files.end() ; i++ )
			files.insert( i->first );
	}

#pragma mark ----

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
		DataManagerCommandResponse response;
		
		au::CommandLine  commandLine;
		commandLine.set_flag_boolean("f");		// Force boolean flag to avoid error when creating queue
		commandLine.set_flag_boolean("txt");	// when adding txt data sets
		commandLine.set_flag_string("tag","");	// when removing automatic operation
		
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

        if( commandLine.get_argument(0) == "add_queue" )
		{
			
			
			if( commandLine.get_num_arguments() < 4 )
			{
				response.output = "Usage: add_queue name format operation outputs1 outputs2 output3. Where format = { txt , key_format-value_format } and outputs1 = { stream_queue_name , q1,q2,q3 } ";
				response.error = true;
				return response;
			}
            
			std::string name            = commandLine.get_argument( 1 );
            std::string format_string   = commandLine.get_argument( 2 );
			std::string operation       = commandLine.get_argument( 3 );

            KVFormat format = KVFormat::format( format_string );
			
            
            if( !ModulesManager::shared()->checkData( format.keyFormat ) )
            {
                response.output = "Unsupported data format " + format.keyFormat;
                response.error = true;
                return response;
            }
            
            if( !ModulesManager::shared()->checkData( format.valueFormat ) )
            {
                std::ostringstream output;
                output << "Unsupported data format " + format.valueFormat;
                response.output = output.str();
                response.error = true;
                return response;
            }

            // Check it the queue already exists
            network::StreamQueue *_queue = stream_queues.findInMap( name );
			
			// Check if queue exist
			if( _queue != NULL )
			{
                std::ostringstream output;
                output << "Queue " + name + " already exist";
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
                    if( commandLine.get_num_arguments() < ( 4 + op->getNumOutputs() ) )
                    {
                        std::ostringstream output;
                        output << "Not enought outputs for operation " + operation + ". It has " << op->getNumOutputs() << " outputs";
                        response.output = output.str();
                        response.error = true;
                        return response;
                        
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
                    output << "Operation type is currently not supported... comming soon!";
                    response.output = output.str();
                    response.error = true;
                    return response;
                    
                }
                    break;
            }
            
            
            // Create the new queue
			
            network::StreamQueue *tmp = new network::StreamQueue();
            tmp->set_name(name);
            copy( &format , tmp->mutable_format() );
            tmp->set_operation( operation );
            
            for (int i = 0 ; i < op->getNumOutputs() ; i++ )
            {
                network::StreamQueueOutput * output = tmp->add_output();

                // Get comma separated elements
                std::vector<std::string> queues = au::split( commandLine.get_argument( 4 + i) , ',' );

                for ( size_t j = 0 ; j < queues.size() ; j++ )
                    output->add_queue( queues[j] );
                
            }
            
            // Insert the queue in the global list of stream-queues
			stream_queues.insertInMap( name , tmp );
            
            
			response.output = "OK";
			return response;
		}

		


		if( commandLine.isArgumentValue(0, "remove_all" , "remove_all" ) )
		{
			// remove completelly queues
			queues.clearMap();
            
            // remove completelly stream_queues
            stream_queues.clearMap();

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
					ActiveTask *at = tasks.findInMap( task_id );
					if(at)
						at->addFiles( tmp );
					
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
					ActiveTask *at = tasks.findInMap( task_id );
					if(at)
						at->addFiles( tmp );
					
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
				output << "Queue " + name + " exist. Please, remove it first with remove_queue command";
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
				output << "Queue " + name + " exist. Please, remove it first with remove_queue command";
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
		return SamsonSetup::shared()->controllerDirectory + "/log_controller";
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
        {
            std::map< std::string , network::StreamQueue*>::iterator i;
            for (i = stream_queues.begin() ; i!= stream_queues.end() ;i++)
            {
                network::StreamQueue *queue = i->second;
                
                if( filterName( i->first , begin, end) )
                {                
                    network::StreamQueue *fq = ql->add_stream_queue();
                    fq->CopyFrom(*queue);
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
	
	void ControllerDataManager::retreveInfoForTask( size_t job_id , ControllerTaskInfo *info , bool clear_inputs )		
	{
		lock.lock();
		_retreveInfoForTask( info );

		if( clear_inputs )
		{
			std::ostringstream command;
			command << "clear ";
			for (size_t i = 0 ; i < info->inputs.size() ; i++ )
				command << info->inputs[i] << " ";

			DataManagerCommandResponse ans =  _runOperation( job_id , command.str() );
		}
		
		lock.unlock();
		
	}
	
	void ControllerDataManager::_retreveInfoForTask( ControllerTaskInfo *info )		
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
					info->setError( error_message.str() );
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
				info->setError( error_message.str() );
				return; 
			}
		}

		// Check too many outputs error
		if( info->operation->getType() != Operation::script )
			if( (int)info->outputs.size() > SamsonSetup::shared()->num_paralell_outputs )
			{
				error_message << "Operation with too many outputs ( " << info->outputs.size() << " ). The limit in this SAMSON cluster is " << SamsonSetup::shared()->num_paralell_outputs;
				info->setError( error_message.str() );
				return; 
			}
		
		// Check output	
		for (int i = 0 ; i < (int)info->outputs.size() ; i++)
		{
			std::string queue_name = info->outputs[i];
			
			Queue *q = queues.findInMap( queue_name );
			if( q )
			{
				KVFormat queue_format = q->format();
				KVFormat parameter_format = info->operation->getOutputFormat(i);
				
				if( !queue_format.isEqual( parameter_format ) )
				{
					error_message << "Wrong format for queue " << queue_name << " (" << queue_format.str() << " vs " << parameter_format.str() << ")";
					info->setError( error_message.str() );
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
				info->setError( error_message.str() );
				return; 
			}
		}
		
		
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
