

#include "Monitor.h"			// Own interface
#include "SamsonController.h"	// ss::SamsonController
#include "Queue.h"				// ss::Queue

namespace ss {
	
	
#pragma mark SSMonitor
	

	void *runMonitoring( void*p)
	{
		((Monitor*)p)->run();
		return NULL;
	}
	
	
	Monitor::Monitor( SamsonController * _controller)
	{
		// Keep a pointer to the controller
		controller = _controller;
		
		// Create the monitoring thread
		pthread_create(&t, NULL, runMonitoring, this);
	}
	
	void Monitor::run()
	{		
		while( true )
		{			
			// Full all fields
			system.push( "random" , random()%10 );
			//system.push( "random2" , random()%10 );

			lock.lock();
			
			// Take a sample for all the queues
			for (std::set<Queue*>::iterator iter = queues.begin() ; iter != queues.end() ; iter++)
				(*iter)->takeMonitorSamples();

			lock.unlock();
			
			
			sleep(1);				// Sleep for the next snapshot
		}
	}
	
	std::string Monitor::getJSONString( std::string command )
	{
		lock.lock();

		std::stringstream o;
	
		// ------------------------------------------------------------------------------------------------
		o << "{";
		
		o << "system:" << system.getJSONString() << ",";
		o << "queues:";
		o << "[";

		std::set<Queue*>::iterator queues_iterator;				
		for ( queues_iterator = queues.begin() ; queues_iterator != queues.end() ; )
		{
			Queue *q = *queues_iterator;
			o << q->monitor.getJSONString();
			
			queues_iterator++;
			if( queues_iterator != queues.end() )
				o << ",";
			
		}

		o << "]";
		
		o << ",";
		o << "}";
		// ------------------------------------------------------------------------------------------------
		
				
		lock.unlock();

		return o.str();
		
	}

		
	void Monitor::addQueueToMonitor( Queue *queue )
	{
		lock.lock();
		queues.insert( queue );
		lock.unlock();
	}	
	
	void Monitor::removeQueueToMonitor( Queue *queue )
	{
		lock.lock();
		queues.erase( queue );
		lock.unlock();
	}
	
	
	
	
	
}

