
#include "WorkerDataManager.h"		// Own interface
#include "CommandLine.h"			// au::CommandLine
#include "SamsonWorker.h"			// ss::SamsonWorker

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
			
			if( findQueue(name)!= NULL)
				return false;
			
			KVFormat format = KVFormat::format( commandLine.get_argument(2) , commandLine.get_argument(3) );
			WorkerQueue *tmp = new WorkerQueue(name , format);
			queue.insert( std::pair<std::string,WorkerQueue*>( name, tmp ) );
			
			return true;
		}

		if( mainCommand == "remove_queue" )
		{
			std::string name = commandLine.get_argument(1);
			
			std::map< std::string , WorkerQueue*>::iterator q = queue.find( name );
			if( q == queue.end() )
				return false;	// Not found queue

			delete q->second;	// Remove que queue object from memory
			queue.erase( q );
			return true;
		}

		if( mainCommand == "move_queue" )
		{
			std::string name = commandLine.get_argument(1);
			std::string name2 = commandLine.get_argument(2);
			
			std::map< std::string , WorkerQueue*>::iterator q = queue.find( name );
			if( q == queue.end() )
				return false;	// Not found queue
			
			WorkerQueue *_queue = q->second;
			
			queue.erase( q );
			
			_queue->rename( name2 );
			queue.insert( std::pair<std::string,WorkerQueue*>( name2 , _queue ) );
			
			return true;
		}
		
		if( mainCommand == "clear_queue" )
		{
			std::string name = commandLine.get_argument(1);
			
			std::map< std::string , WorkerQueue*>::iterator q = queue.find( name );
			if( q == queue.end() )
				return false;	// Not found queue
			
			q->second->clear();	// Remove all data
		}
		
		
		if( mainCommand == "add_file" )
		{
			std::string name = commandLine.get_argument(1);
			WorkerQueue *queue = findQueue(name);
			
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
		
		
		for( std::map< std::string , WorkerQueue*>::iterator q = queue.begin() ; q != queue.end() ; q++)
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