

#include "ControllerDataManager.h"			// Own interface
#include "ControllerTask.h"					// ControllerTask
#include "CommandLine.h"					// au::CommandLine
#include "samson/KVFormat.h"				// ss::KVFormat
#include "ControllerQueue.h"				// ss::ControllerQueue
#include "au_map.h"							// au::insertInMap
#include "DataManager.h"					// ss::DataManagerCommandResponse
#include "SamsonController.h"				// ss::SamsonController

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
			if( controller->data.existQueue( name ) )
			{
				std::ostringstream output;
				output << "Queue " + name + " already exist\n";
				response.output = output.str();
				response.error = true;
				return response;
			}			
			
			KVFormat format = KVFormat::format( keyFormat , valueFormat );
			ControllerQueue *tmp = new ControllerQueue(name , format);
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
			if( !controller->data.existQueue( name ) )
			{
				std::ostringstream output;
				output << "Queue " + name + " does not exist\n";
				response.output = output.str();
				response.error = true;
				return response;
			}			
			
			ControllerQueue *tmp =  queues.extractFromMap(name);
			delete tmp;
			
			response.output = "OK";
			return response;
		}		
		
		response.error = true;
		response.output = "Unknown command";
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
	
	std::string ControllerDataManager::status()
	{
		std::ostringstream o;
		o << "Data Manager:" << std::endl;
		lock.lock();
		for ( au::map< std::string , ControllerQueue>::iterator q = queues.begin() ; q != queues.end() ; q++)
			o << q->first << " " << q->second->str() << std::endl;
		lock.unlock();
		
		return o.str();
		
	}		
	
	std::string ControllerDataManager::getQueuesStr( std::string format )
	{
		std::ostringstream o;
		lock.lock();
		
		o << "Queues:\n";
		std::map< std::string , ControllerQueue*>::iterator i;
		for (i = queues.begin() ; i!= queues.end() ;i++)
			o << i->first << i->second->str() << "\n";
		
		
		lock.unlock();
		return o.str();
	}
	
	
	

}
