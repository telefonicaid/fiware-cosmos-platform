#include <string>

#include "samson.h"
#include "KVManager.h"
#include "MemoryController.h"
#include "KVQueue.h"
#include "KVSet.h"
#include "KVSetsManager.h"
#include "KVFormat.h"
#include "Data.h"
#include "Module.h"
#include "KVFormat.h"
#include "SSLogger.h"
#include "SSMonitor.h"
#include "KVSetBuffer.h"
#include "TaskManager.h"
#include "Task.h"                /* Task                                     */
#include "KVQueuesManager.h"     /* Own interface                            */



namespace ss
{	
	void KVQueuesManager::addKVSetToQueue( KVSet *set , KVQueue *queue , int hash)
	{
		if( !queue )
			return;
		queue->addKVSet( set , hash );		// Add to the correct queue
	}
	
	void KVQueuesManager::addKVSetToKVQueue( KVSet *set , std::string queue_name , int hash )
	{
		
		KVQueue *queue = findQueue( queue_name );
		if( queue )
		{
			
			addKVSetToQueue( set , queue , hash );
			
			// Distribiton of KVSets to the distribution queue mechanism
			for ( size_t i = 0 ; i < queue->distributionQueues.size() ; i++)
			{
				KVQueue *queue_distribution =  findQueue( queue->distributionQueues[i] );
				
				
				addKVSetToQueue( set , queue_distribution , hash );
			}
		}
		else
			SSLogger::log("KV Manager" , SSLogger::warning , "KVSet not added since queue unknown" );
		
	}	
	
	KVQueue * KVQueuesManager::findQueue( std::string& queue_name  )
	{
		std::map<std::string , KVQueue*>::iterator i = queues.find( queue_name );
		if( i == queues.end() )
			return NULL;
		else
			return i->second;
		
	}
	
	
	void KVQueuesManager::newKVQueue( std::string queue_name , KVFormat format )
	{
		KVQueue *_queue = new KVQueue( queue_name , format );	
		queues.insert( std::pair<std::string , KVQueue*>( queue_name , _queue) );
		
		SSMonitor::shared()->addQueueToMonitor( _queue );			// Add to the monitoring system
		
	}
	
	void KVQueuesManager::removeKVQueue( std::string queue_name )
	{
		std::map<std::string , KVQueue*>::iterator i = queues.find( queue_name );
		if( i != queues.end() )
		{
			KVQueue *queue = i->second;
			
			assert( queue );
			
			queue->clear();
			
			SSMonitor::shared()->removeQueueToMonitor( queue );		
			
			queues.erase(i);		// Remove from the queues map
			delete queue;			// Remove the allocated object
		}
		
	}		
	
	std::string KVQueuesManager::str()
	{
		std::stringstream o;
		
		std::map<std::string , KVQueue*>::iterator i;
		for ( i = queues.begin() ; i!= queues.end() ; i++ )
			o << i->second->str() << std::endl ;
		
		return o.str();
	}
	
	KVManagerLog * KVQueuesManager::getFullLog()
	{
		KVManagerLog *log = new KVManagerLog( 0 , TASK_CODE_ACTION );
		std::map<std::string , KVQueue*>::iterator i;
		for ( i = queues.begin() ; i!= queues.end() ; i++ )
		{
			KVQueue * _queue = i->second;
			log->addQueue( _queue );
		}
		return log;
	}
}
