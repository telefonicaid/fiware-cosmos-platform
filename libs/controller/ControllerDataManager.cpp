

#include "ControllerDataManager.h"			// Own interface
#include "ControllerTask.h"					// ControllerTask
#include "CommandLine.h"					// au::CommandLine
#include "samson/KVFormat.h"				// ss::KVFormat
#include "Queue.h"				// ss::Queue
#include "au_map.h"							// au::insertInMap
#include "DataManager.h"					// ss::DataManagerCommandResponse
#include "SamsonController.h"				// ss::SamsonController
#include "coding.h"						// ss::KVInfo
#include "ModulesManager.h"					// Utility functions
#include "samson/Operation.h"				// ss::Operation
#include "QueueFile.h"						// ss::QueueFile
#include "samsonDirectories.h"				// SAMSON_CONTROLLER_DIRECTORY
#include "MessagesOperations.h"				// evalHelpFilter(.)
#include "AutomaticOperation.h"				// ss::AutomaticOperation
#include "SamsonSetup.h"					// ss::SamsonSetup

namespace ss {
	
	// Handy function

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
		automatic_operations_manager.clear();
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

		
		if( commandLine.get_argument(0) == "remove_operation" )
		{
			if( commandLine.get_flag_string("tag") != "" )
			{
				automatic_operations_manager.removeAllWithTag( commandLine.get_flag_string("tag") );
				
				response.output = "OK";
				return response;
			}
			
			if( commandLine.get_num_arguments() < 2 )
			{
				response.output = "Usage: remove_operation id";
				response.error = true;
				return response;
			}
			
			size_t id = strtoll( commandLine.get_argument(1).c_str()  , (char **)NULL, 10);
			automatic_operations_manager.remove(id);
			
			response.output = "OK";
			return response;
			
		}
			
		
		if( commandLine.get_argument(0) == "set_operation" )
		{
			if( commandLine.get_num_arguments() < 2 )
			{
				response.output = "Usage: set_operation command [ \"queue queue_name [min_size] [min_kvs]\" ] ... [-timeout t]";
				response.error = true;
				return response;
			}
			
			// Create the automatic operation with the command given at the command line
			std::string command	= commandLine.get_argument(1);
			AutomaticOperation *automatic_operation = new AutomaticOperation( command );
			
			for (int i = 2 ; i < commandLine.get_num_arguments() ; i++)
			{
				au::CommandLine _cmdLine;
				_cmdLine.set_flag_uint64("size",0);
				_cmdLine.set_flag_uint64("kvs",0);
				_cmdLine.parse( commandLine.get_argument(i) ); 
				
				if( ( _cmdLine.get_num_arguments() < 2) || (_cmdLine.get_argument(0) != "queue" ) )
				{
					response.output = "Usage: set_operation command [ \"thrigger queue1 [min_size] [min_kvs]\" ] ... [-timeout t]";
					response.error = true;
					return response;
				}
				
				std::string queue_name = _cmdLine.get_argument(1);
				Queue *queue =  queues.findInMap( queue_name );
				
				if( !queue )
				{
					std::ostringstream output;
					output << "Queue " + queue_name + " does not exist";
					response.output = output.str();
					response.error = true;
					return response;				
				}
				
				size_t min_size = _cmdLine.get_flag_uint64("size");
				size_t min_kvs	= _cmdLine.get_flag_uint64("kvs");
				
				AOQueueThrigger *queue_thrigger = new AOQueueThrigger( queue , min_size , min_kvs );
				automatic_operation->add( (AOThrigger*)queue_thrigger );
			}

			// Set tag
			std::string tag =  commandLine.get_flag_string("tag");
			if( tag != "")
				automatic_operation->addTag( tag );
			
			// Add the automatic operation to the manager
			automatic_operations_manager.add( automatic_operation );
			
			response.output = "OK";
			return response;
		}		
		

		if( commandLine.isArgumentValue(0, "remove_all" , "remove_all" ) )
		{
			// Not controlled files to not be removed
			
			queues.clear();
			
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
				assert( false );
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

	void ControllerDataManager::fill( network::AutomaticOperationList *aol , std::string command)
	{
		lock.lock();
		automatic_operations_manager.fill( aol, command);
		lock.unlock();
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
		
		std::map< std::string , Queue*>::iterator i;
		for (i = queues.begin() ; i!= queues.end() ;i++)
		{
			Queue *queue = i->second;
			
			if( filterName( i->first , begin, end) )
			{
				network::FullQueue *fq = ql->add_queue();
				
				network::Queue *q = fq->mutable_queue();
				
				q->set_name( i->first );
				
				// Format
				fillKVFormat( q->mutable_format() , queue->format() );
				
				//Info
				fillFullKVInfo( q->mutable_info(), queue->info() );
				
				
				// Add file information
				au::map< std::string , QueueFile >::iterator iter;
				
				for ( iter = queue->files.begin() ; iter != queue->files.end() ; iter++)
				{
					network::File *file = fq->add_file();
					iter->second->fill( file );
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
			//assert( !ans.error );	// Internal command ( no error possible )
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
				network::FullQueue *fq = q->getFullQueue();
				info->input_queues.push_back( fq ); 
				
				
			}
			else
			{
				error_message << "Unknown queue " << info->inputs[i];
				info->setError( error_message.str() );
				return; 
			}
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
				network::FullQueue *fq = q->getFullQueue();
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
	
	// Automatic operation API to get list of pending jobs and set as finished
	
	std::vector<AOInfo> ControllerDataManager::getNextAutomaticOperations()
	{
		lock.lock();
		std::vector<AOInfo> tmp = automatic_operations_manager.getNextAutomaticOperations();
		lock.unlock();
		
		return tmp;
	}
	
	void ControllerDataManager::finishAutomaticOperation( size_t id ,bool error , std::string error_message )
	{
		lock.lock();
		automatic_operations_manager.finishAutomaticOperation( id , error , error_message );
		lock.unlock();
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
