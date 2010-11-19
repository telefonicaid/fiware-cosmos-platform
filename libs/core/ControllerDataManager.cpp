

#include "ControllerDataManager.h"			// Own interface
#include "ControllerTask.h"					// ControllerTask
#include "CommandLine.h"					// au::CommandLine
#include "samson/KVFormat.h"				// ss::KVFormat
#include "Queue.h"				// ss::Queue
#include "au_map.h"							// au::insertInMap
#include "DataManager.h"					// ss::DataManagerCommandResponse
#include "SamsonController.h"				// ss::SamsonController
#include "KVInfo.h"							// ss::KVInfo
#include "ModulesManager.h"					// Utility functions
#include "ObjectWithStatus.h"				// getStatusFromArray(.)
#include "samson/Operation.h"				// ss::Operation

namespace ss {
	
	DataManagerCommandResponse ControllerDataManager::_run( std::string command )
	{
		DataManagerCommandResponse response;
		
		au::CommandLine  commandLine;
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
			
			if( !controller->modulesManager.checkData( keyFormat ) )
			{
				response.output = "Unsupported data format " + keyFormat + "\n";
				response.error = true;
				return response;
			}
			
			if( !controller->modulesManager.checkData( valueFormat ) )
			{
				std::ostringstream output;
				output << "Unsupported data format " + keyFormat + "\n";
				response.output = output.str();
				response.error = true;
				return response;
			}
			
			// Check if queue exist
			if( queues.findInMap( name ) != NULL )
			{
				std::ostringstream output;
				output << "Queue " + name + " already exist\n";
				response.output = output.str();
				response.error = true;
				return response;
			}			
			
			KVFormat format = KVFormat::format( keyFormat , valueFormat );
			Queue *tmp = new Queue(name , format);
			queues.insertInMap( name , tmp );
			
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
				delete tmp;
			
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
			if( data_queues.findInMap( name ) != NULL )
			{
				std::ostringstream output;
				output << "Queue " + name + " already exist\n";
				response.output = output.str();
				response.error = true;
				return response;
			}			
			
			Queue *tmp = new Queue( name , KVFormat("txt","txt") );
			data_queues.insertInMap( name , tmp );
			
			response.output = "OK";
			return response;
		}
		
		if( commandLine.get_argument(0) == "remove_data_queue" )
		{
			// Add queue command
			if( commandLine.get_num_arguments() < 2 )
			{
				response.output = "Usage: remove_data_queue name";
				response.error = true;
				return response;
			}
			
			std::string name = commandLine.get_argument( 1 );
			
			// Check if queue exist
			Queue *tmp =  data_queues.extractFromMap(name);
			if( !tmp )
			{
				std::ostringstream output;
				output << "Queue " + name + " does not exist\n";
				response.output = output.str();
				response.error = true;
				return response;
			}
			else
				delete tmp;
			
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
			Queue *q =  data_queues.findInMap(queue);
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
		fileName << "/var/samson/log/log_controller";
		return fileName.str();
	}
	
	std::string ControllerDataManager::getStatus()
	{
		std::ostringstream o;
		
		lock.lock();
		o << getStatusFromArray( queues );
		lock.unlock();
		
		return o.str();
		
	}		
	
	void ControllerDataManager::helpQueues( network::HelpResponse *response , network::Help help )
	{
		lock.lock();
		
		if( help.queues() )
		{
			std::map< std::string , Queue*>::iterator i;
			for (i = queues.begin() ; i!= queues.end() ;i++)
			{
				if( !help.has_name() || i->first == help.name() )
				{
					
					Queue *queue = i->second;
					network::Queue *q = response->add_queue();
					q->set_name( i->first );
					
					// Format
					fillKVFormat( q->mutable_format() , queue->format() );
					
					//Info
					fillKVInfo( q->mutable_info(), queue->info() );
					
				}
				
				
			}
		}
	
		
		if( help.data_queues() )
		{
			std::map< std::string , Queue*>::iterator i;
			for (i = data_queues.begin() ; i!= data_queues.end() ;i++)
			{
				if( !help.has_name() || i->first == help.name() )
				{
					
					Queue *queue = i->second;
					network::Queue *q = response->add_data_queue();
					q->set_name( i->first );
					
					// Format
					fillKVFormat( q->mutable_format() , queue->format() );
					
					//Info
					fillKVInfo( q->mutable_info(), queue->info() );
					
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
				KVFormat queue_format = q->format();
				KVFormat parameter_format = info->operation->getInputFormat(i);
				
				if( !queue_format.isEqual( parameter_format ) )
				{
					error_message << "Wrong format for queue " << queue_name << " (" << queue_format.str() << " vs " << parameter_format.str() << ")";
					info->setError( error_message.str() );
					return; 
				}
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
