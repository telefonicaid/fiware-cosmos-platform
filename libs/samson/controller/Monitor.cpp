

#include "Monitor.h"			// Own interface
#include "samson/controller/SamsonController.h"	// samson::SamsonController
#include "Queue.h"				// samson::Queue

namespace samson {
	
	
#pragma mark SSMonitor
	

	
	Monitor::Monitor( SamsonController * _controller )
	{
		// Keep a pointer to the controller
		controller = _controller;
		
	}
    
    Monitor::~Monitor()
    {
        // Remove all the objects allocated
        queues.clearMap();
    }
	
	void Monitor::takeSamples()
	{		

	   controller->pushSystemMonitor( &system ); 


		// For each queue, take sample
		std::vector<QueueMonitorInfo> queuesMonitorInfo;
		controller->data.getQueuesMonitorInfo( queuesMonitorInfo ); // Get the names for all the queues
		
		au::map<std::string,MonitorBlock> tmp_queues;			// Temporal vector to hold all the queues
		
		for ( size_t i = 0 ; i < queuesMonitorInfo.size() ; i++ )
		{
			MonitorBlock* mb = queues.extractFromMap( queuesMonitorInfo[i].queue );
			if( !mb )
			{
				mb = new MonitorBlock();
				mb->addMainParameter( "name" , queuesMonitorInfo[i].queue );
				mb->addMainParameter( "format" , queuesMonitorInfo[i].format );
				
			}
			
			mb->push("size", queuesMonitorInfo[i].info.size );
			mb->push("kvs", queuesMonitorInfo[i].info.kvs );
			mb->push("num_files", queuesMonitorInfo[i].num_files );

			tmp_queues.insertInMap(queuesMonitorInfo[i].queue, mb);
		}

        // Remove the rest of unused queue information
        queues.clearMap();
        
		// Remove the previous queues not used any more and copy the prepared samples
        queues.insert( tmp_queues.begin() , tmp_queues.end() );
        tmp_queues.clear(); // Note here we are not deleting internal elements
		
	}
	
	std::string Monitor::getJSONString( std::string command )
	{

		std::stringstream o;
	
		// ------------------------------------------------------------------------------------------------
		o << "{";
		
		o << "system:" << system.getJSONString() << ",";
		o << "queues:";
		o << "[";

		au::map<std::string,MonitorBlock>::iterator queues_iterator;				
		for ( queues_iterator = queues.begin() ; queues_iterator != queues.end() ; )
		{
			MonitorBlock *mb = queues_iterator->second;
			o << mb->getJSONString();
			
			queues_iterator++;
			if( queues_iterator != queues.end() )
				o << ",";
			
		}

		o << "]";
		
		o << ",";
		o << "}";
		// ------------------------------------------------------------------------------------------------

		return o.str();
		
	}


	
	
	
	
}
