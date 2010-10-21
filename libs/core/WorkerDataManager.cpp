
#include "WorkerDataManager.h"		// Own interface
#include "CommandLine.h"			// au::CommandLine
#include "SamsonWorker.h"			// ss::SamsonWorker
#include "au_map.h"					// findInMap
#include "WorkerQueue.h"				// ss::ControllerQueue

namespace ss {

	
	bool WorkerDataManager::_run( size_t task_id , std::string command )
	{
		// we must verify the command
		
		au::CommandLine commandLine;
		commandLine.parse(command);
		
		if( commandLine.get_num_arguments() == 0)
		{
			return true;
		}
		
		std::string mainCommand = commandLine.get_argument(0);
		
		if( mainCommand == "add_queue" )
		{
			std::string name = commandLine.get_argument(1);
			
			if( au::findInMap( queues, name)!= NULL)
				return false;
			
			KVFormat format = KVFormat::format( commandLine.get_argument(2) , commandLine.get_argument(3) );
			WorkerQueue *tmp = new WorkerQueue(name , format);
			au::insertInMap( queues, name , tmp);
			
			return true;
		}

		if( mainCommand == "remove_queue" )
		{
			std::string name = commandLine.get_argument(1);
			
			WorkerQueue *queue = au::extractFromMap( queues, name );
			if( queue )
			{
				delete queue;
				return true;
			}
			else
				return false;
		}

		if( mainCommand == "move_queue" )
		{
			std::string name = commandLine.get_argument(1);
			std::string name2 = commandLine.get_argument(2);
			
			
			WorkerQueue * queue = au::extractFromMap( queues, name );
			queue->rename( name2 );
			au::insertInMap(queues, name2, queue);
			
			return true;
		}
		
		if( mainCommand == "clear_queue" )
		{
			std::string name = commandLine.get_argument(1);
			
			WorkerQueue *queue = au::findInMap( queues, name );

			if( !queue )
				return false;	// Not found queue
			
			queue->clear();	// Remove all data
		}
		
		
		if( mainCommand == "add_file" )
		{
			std::string name = commandLine.get_argument(1);
			WorkerQueue *queue = au::findInMap( queues, name );
			
			if( !queue )
				return false;
			
			std::string fileName = commandLine.get_argument(2);
			
			queue->addFile( fileName );
			return true;	
		}
		
		// Unknwn command
		return false;
	}
	
	void WorkerDataManager::fillWorkerStatus( network::WorkerStatus* status )
	{
		lock.lock();
		
		
		for( std::map< std::string , WorkerQueue*>::iterator q = queues.begin() ; q != queues.end() ; q++)
		{
			
			WorkerQueue * _q = q->second;
			network::Queue* report_queue = status->add_queue();
			
			KVInfo info = _q->getInfo();
			
			report_queue->set_size(info.size);
			report_queue->set_kvs(info.kvs);
		}
		
		
		lock.unlock();
	}
	
	std::string WorkerDataManager::getLogFileName(   )
	{
		std::ostringstream fileName;
		fileName << "/var/samson/log/log_worker_";
		fileName << worker->network->getMyidentifier();
		return fileName.str();
	}
	
	
	
}