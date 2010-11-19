

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
	
	
	Monitor::	Monitor( SamsonController *controller)
	{
		// Create the monitoring thread
		pthread_create(&t, NULL, runMonitoring, this);
	}
	
	void Monitor::run()
	{		
		while( true )
		{			
			// Full all fields
			system.push( "random" , random()%10 );
			
			// Step monitoring queues
			/*
			lock.lock();
			for (std::map<std::string,KVQueue*>::iterator i = queues.begin() ; i!= queues.end() ; i++)
				i->second->sample();
			lock.unlock();
			 */
			
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
		for ( queues_iterator = queues.begin() ; queues_iterator != queues.end() ; queues_iterator++)
		{
			Queue *q = *queues_iterator;
			o << q->monitor.getJSONString();
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

