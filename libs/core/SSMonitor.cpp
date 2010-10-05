#include "SSMonitor.h"
#include "KVManager.h"
#include "MemoryController.h"
#include "TaskManager.h"
#include "KVSet.h"
#include "KVQueue.h"

using namespace ss;



namespace ss {
	
	
#pragma mark SSMonitor
	
	SSMonitor* ssMonitor = NULL;
	
	SSMonitor* SSMonitor::shared()
	{
		if (!ssMonitor)
			ssMonitor = new SSMonitor();
		
		return ssMonitor;
	}
	
	SSMonitor::SSMonitor()
	{
		
		cores =  new ParameterMonitor();
		random =  new ParameterMonitor();
		parameters.insert( std::pair<std::string,ParameterMonitor*>( "system.cores" , cores ) );
		parameters.insert( std::pair<std::string,ParameterMonitor*>( "system.random" , random ) );
		
		
	}
	
	void SSMonitor::run()
	{
		TaskManager *taskManager = TaskManager::shared();
		
		while( true )
		{
			
			// Full all fields
			cores->push( taskManager->num_working_cores );
			random->push( rand()%10 );
			
			// Step monitoring queues
			lock.lock();
			for (std::map<std::string,KVQueue*>::iterator i = queues.begin() ; i!= queues.end() ; i++)
				i->second->sample();
			lock.unlock();
			
			
			// Sleep for the next snapshot
			sleep(1);
		}
	}
	
	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) 
	{
		std::stringstream ss(s);
		std::string item;
		while(std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}	
	std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		return split(s, delim, elems);
	}
	
	
	KVQueue * SSMonitor::findKVQueue( std::string queue_name )
	{
		std::map<std::string,KVQueue*>::iterator i = queues.find( queue_name );
		if( i == queues.end() )
			return NULL;
		else
			return i->second;
	}

	
	
	
	std::string SSMonitor::_getSystemPlot( )
	{
		std::stringstream o;

		o << "[";
		for (int scale = 0 ; scale < 3 ; scale++)
		{
			o << "[";
			o << _getData( "#cores" , "system.cores" , scale );
			o << "]";
			o << ",";
		}
		o << "]";

		return o.str();
	}
	
	
	
	std::string SSMonitor::_getQueuePlot(std::string parameter)
	{
		std::stringstream o;
		
		o << "[";
		for (int scale = 0 ; scale < 3 ; scale++)
		{
			o << "[";

			// Iterate all the queues
			std::map<std::string,KVQueue*>::iterator q;
			for (q = queues.begin() ; q != queues.end() ; q++ )
			{
				std::string full_parameter = std::string("queue.") + q->first + "." +  parameter;
				
				// Size
				o << _getData( q->first , full_parameter , scale ) << ",";
				
			}
			
			o << "]";
			o << ",";
		}
		o << "]";
	
		return o.str();
	}
	
	
	std::string SSMonitor::get( )
	{
		lock.lock();
		
		std::stringstream o;
		std::vector<std::string> queue_parameters;
		queue_parameters.push_back( "size" );
		queue_parameters.push_back( "size_on_memory" );
		queue_parameters.push_back( "size_on_disk" );
		
		// ------------------------------------------------------------------------------------------------
		o << "{";

		o << "system: " << _getSystemPlot() << ",\n";
		o << "queues_size: " << _getQueuePlot( "size" ) << ",\n";
		o << "queues_num_kvs: " << _getQueuePlot( "num_kvs" ) << ",\n";
		o << "queues_size_on_memory: " << _getQueuePlot( "size_on_memory" ) << ",\n";
		o << "queues_size_on_disk: " << _getQueuePlot( "size_on_disk" ) << ",\n";
		
		o << "}";
		// ------------------------------------------------------------------------------------------------
		
				
		lock.unlock();

		return o.str();
		
	}
	
	std::string SSMonitor::_getData( std::string label , std::string parameter , int scale )
	{
		std::ostringstream o;
		o << "{";
		o << "label: \"" << label  + "\"";
		o << ",";
		o << "data: " << _getValues(parameter , scale) ;
		/*
		o << ",";
		o << "lines: { show: true , steps: true }";
		o << ",";
		o << "points: { show: false }";		
		o << ",";
		o << "color: 1 ";		
		 */
		o  <<  "}";
		
		
		return o.str();
		
		
	}
	
	
	
	std::string SSMonitor::_getValues( std::string name , int scale )
	{
		
		std::vector<std::string> tockens = split( name , '.' );
		
		if( tockens.size() == 0)
			return "unknown parameter";
		
		// Let's see if it is a queue
		if( tockens[0] == "queue" )
		{
			if ( tockens.size() != 3)
				return "wrong format : queue.name_queue.paramer (" + name +  ")";
			
			std::string answer;
			KVQueue *queue = findKVQueue( tockens[1] );

			if (!queue)
				answer = "unknown queue ";
			else
				answer = queue->getParameter( tockens[2] , scale );
			
			return answer;
			
		}
		
		if( tockens[0] == "system" )
		{
			if ( tockens.size() != 2)
				return "wrong format : system.paramer (memory, cores, queues, etc)\n";
			
			if( tockens[1] == "queues")
			{
				std::ostringstream out;
				for (std::map<std::string, KVQueue*>::iterator queue = queues.begin() ;  queue != queues.end() ; )
				{
					out << queue->first;
					queue++;
					if( queue != queues.end() )
						out << " ";
				}
				
				return out.str();
			}
			
			if( tockens[1] == "cores" )
				return cores->str(scale);
			
			if( tockens[1] == "random" )
				return random->str(scale);
			   
			
		}
		
		return "Unknown parameter " + name + "\n";
	}
	
	void SSMonitor::addQueueToMonitor( KVQueue *queue )
	{
		lock.lock();
		queues.insert( std::pair<std::string,KVQueue*>( queue->name , queue ) );
		lock.unlock();
	}
	
	void SSMonitor::removeQueueToMonitor( KVQueue *queue )
	{
		lock.lock();
		std::map<std::string,KVQueue*>::iterator i = queues.find( queue->name );
		if( i != queues.end() )
			queues.erase( i );
		lock.unlock();
	}
	
	
}

