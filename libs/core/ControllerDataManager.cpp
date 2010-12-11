

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
#include "Status.h"				// getStatusFromArray(.)
#include "samson/Operation.h"				// ss::Operation
#include "QueueFile.h"						// ss::QueueFile
#include "samsonDirectories.h"				// SAMSON_CONTROLLER_DIRECTORY
#include "MessagesOperations.h"				// evalHelpFilter(.)

namespace ss {
	
	DataManagerCommandResponse ControllerDataManager::_run( std::string command )
	{
		DataManagerCommandResponse response;
		
		au::CommandLine  commandLine;
		commandLine.set_flag_boolean("f");	// Force boolean flag
		
		commandLine.parse( command );
		
		if( commandLine.get_num_arguments() == 0)
		{
			response.output = "No command found";
			response.error = true;
			return response;
		}

		if( commandLine.get_argument(0) == "add_queue" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 4 )
			{
				response.output = "Usage: add_queue name <keyFormat> <valueFormat>";
				response.error = true;
				return response;
			}
			
			std::string name = commandLine.get_argument( 1 );
			std::string keyFormat= commandLine.get_argument( 2 );
			std::string	valueFormat = commandLine.get_argument( 3 );
			
			bool forceFlag = commandLine.get_flag_bool("f");
			
			if( !controller->modulesManager.checkData( keyFormat ) )
			{
				response.output = "Unsupported data format " + keyFormat + "\n";
				response.error = true;
				return response;
			}
			
			if( !controller->modulesManager.checkData( valueFormat ) )
			{
				std::ostringstream output;
				output << "Unsupported data format " + valueFormat + "\n";
				response.output = output.str();
				response.error = true;
				return response;
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
						response.output = "OK";
						return response;
					}
					else
					{
						std::ostringstream output;
						output << "Queue " + name + " already exist with another formats: (" << _queue->_format.str() << "). Option -f is not enougth.\n";
						response.output = output.str();
						response.error = true;
						return response;
						
					}
				}
				else
				{
					std::ostringstream output;
					output << "Queue " + name + " already exist\n";
					response.output = output.str();
					response.error = true;
					return response;
				}
			}			
			
			KVFormat format = KVFormat::format( keyFormat , valueFormat );
			Queue *tmp = new Queue(name , format);
			queues.insertInMap( name , tmp );

			
			// Notify the monitor
			controller->monitor.addQueueToMonitor(tmp);
			
			response.output = "OK";
			return response;
		}

		if( commandLine.get_argument(0) == "remove_queue" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 2 )
			{
				response.output = "Usage: remove_queue name";
				response.error = true;
				return response;
			}
			
			std::string name = commandLine.get_argument( 1 );
			
			// Check if queue exist
			Queue *tmp =  queues.extractFromMap(name);
			if( !tmp )
			{
				std::ostringstream output;
				output << "Queue " + name + " does not exist\n";
				response.output = output.str();
				response.error = true;
				return response;
			}
			else
			{
				controller->monitor.removeQueueToMonitor(tmp);
				
				delete tmp;
			}
			
			response.output = "OK";
			return response;
		}
		
		if( commandLine.get_argument(0) == "clear_queue" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 2 )
			{
				response.output = "Usage: clear_queue name1 name2 ...";
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
					output << "Queue " + name + " does not exist\n";
					response.output = output.str();
					response.error = true;
					return response;
				}
				else
					tmp->clear();
			}
			
			response.output = "OK";
			return response;
		}

#pragma mark Change the name of a queue
		
		if( commandLine.get_argument(0) == "mv_queue" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 3 )
			{
				response.output = "Usage: mv_queue name name2";
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
				output << "Queue " + name + " does not exist\n";
				response.output = output.str();
				response.error = true;
				return response;
			} else if( tmp2 )
			{
				std::ostringstream output;
				output << "Queue " + name + " exist. Please, remove it first with remove_queue command\n";
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
		
		if( commandLine.get_argument(0) == "duplicate_queue" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 3 )
			{
				response.output = "Usage: duplicate_queue name name2";
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
				output << "Queue " + name + " does not exist\n";
				response.output = output.str();
				response.error = true;
				return response;
			}
			else if( tmp2 )
			{
				std::ostringstream output;
				output << "Queue " + name + " exist. Please, remove it first with remove_queue command\n";
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
		
		if( commandLine.get_argument(0) == "cp_queue" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 3 )
			{
				response.output = "Usage: cp_queue queue_from name_to";
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
				output << "Queue " << name << " does not exist\n";
				response.output = output.str();
				response.error = true;
				return response;
			} 

			if( !tmp2 )
			{
				std::ostringstream output;
				output << "Queue " << name << " does not exist\n";
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
				output << "Queues " << name << " and " << name2 << " does not have the same format (key-values).\n";
				response.output = output.str();
				response.error = true;
				return response;
			}
			
			
			// copy content
			tmp2->copyFileFrom( tmp );
			
			response.output = "OK";
			return response;
		}				
		
// DATA QUEUES
		
		if( commandLine.get_argument(0) == "add_data_queue" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 2 )
			{
				response.output = "Usage: add_data_queue name";
				response.error = true;
				return response;
			}
			
			std::string name = commandLine.get_argument( 1 );

			
			// Check if queue exist
			if( queues.findInMap( name ) != NULL )
			{
				std::ostringstream output;
				output << "Queue " + name + " already exist\n";
				response.output = output.str();
				response.error = true;
				return response;
			}			
			
			Queue *tmp = new Queue( name , KVFormat("txt","txt") );
			queues.insertInMap( name , tmp );
			
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
			size_t size = strtoll(commandLine.get_argument( 3 ).c_str(), (char **)NULL, 10);
			
			std::string queue		= commandLine.get_argument( 4 );
			
			// Check valid queue
			Queue *q =  queues.findInMap(queue);
			if( !q )
			{
				std::ostringstream output;
				output << "Data Queue " << queue << " does not exist\n";
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
				output << "Queue " << queue << " does not exist\n";
				response.output = output.str();
				response.error = true;
				return response;
			}
			
			q->addFile( worker , fileName , KVInfo( size , kvs)  );
			response.output = "OK";
			return response;
		}				
		
		
		
		response.error = true;
		response.output = std::string("Unknown command: ") +  command;
		return response;
	}
	
	void ControllerDataManager::_un_run( std::string command )
	{
		// Undo a particular action
	}
	
	
	std::string ControllerDataManager::getLogFileName(   )
	{
		std::ostringstream fileName;
		fileName << SAMSON_CONTROLLER_DIRECTORY << "log_controller";
		return fileName.str();
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
				fillKVInfo( q->mutable_info(), queue->info() );
				
				
				// Add file information
				std::list< QueueFile* >::iterator iter;
				
				for ( iter = queue->files.begin() ; iter != queue->files.end() ; iter++)
				{
					network::File *file = fq->add_file();
					(*iter)->fill( file );
				}
			}
			
			
		}
		
		
		lock.unlock();		
	}
	
	
	void ControllerDataManager::helpQueues( network::HelpResponse *response , network::Help help )
	{
		lock.lock();
		
		if( help.queues() )
		{
			std::map< std::string , Queue*>::iterator i;
			for (i = queues.begin() ; i!= queues.end() ;i++)
			{
				if(  evalHelpFilter( &help , i->first ) )
				{
					
					Queue *queue = i->second;
					network::FullQueue *fq = response->add_queue();
					
					network::Queue *q = fq->mutable_queue();
					q->set_name( i->first );
					
					// Format
					fillKVFormat( q->mutable_format() , queue->format() );
					
					//Info
					fillKVInfo( q->mutable_info(), queue->info() );

					
					// Add file information
					std::list< QueueFile* >::iterator iter;

					for ( iter = queue->files.begin() ; iter != queue->files.end() ; iter++)
					{
						network::File *file = fq->add_file();
						(*iter)->fill( file );
					}
				}
				
				
			}
		}
	
		
		lock.unlock();
	}
	
	void ControllerDataManager::retreveInfoForTask( ControllerTaskInfo *info )		
	{
		lock.lock();
		_retreveInfoForTask( info );
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
				
				// Add the input files for this input
				
				network::FileList fileList;
				q->insertFilesIn( fileList );
				info->input_files.push_back( fileList ); 

				
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
				
				network::Queue qq;
				qq.set_name( q->getName() );
				network::KVFormat *f = qq.mutable_format();
				f->set_keyformat( q->format().keyFormat );
				f->set_valueformat( q->format().valueFormat );
				info->output_queues.push_back( qq ); 
				
			}
			else
			{
				error_message << "Unknown queue " << info->outputs[i];
				info->setError( error_message.str() );
				return; 
			}
		}
		
		
	}
	
	
}
