#include <string>

#include "KVFormat.h"            /* KVFormat                                 */
#include "KVQueueProcess.h"      /* KVQueueProcess                           */
#include "KVQueue.h"             /* KVQueue                                  */
#include "Tasks.h"               /* Task                                     */
#include "KVSet.h"               /* KvSet                                    */
#include "KVManager.h"           /* Own interface                            */



namespace ss {
#pragma mark KVMANAGER Singleton
	
	static KVManager* kvManager=NULL;			// Singlelton pointer

	KVManager* KVManager::shared()
	{
		if(!kvManager)
			kvManager = new KVManager();
		return kvManager;
	}

#pragma mark KVMANAGER
	
	KVManager::KVManager() : kvMantainerStopLock( &lock )
	{
		lock.description = "KVManager lock";

		// Recreate the log file compressing current state
		setupFromLog(this);
		
		// A single log with all information on the platform
		KVManagerLog * tmp = queues.getFullLog();
		_restoreWithUniqueLog( tmp );
		delete tmp;
		
		
		// Start the log file
		_startLog();
		
	}


	KVFormat getFormat( protocol::Format _format  )
	{
		return KVFormat::format( _format.key() , _format.value() );
	}
	
	
	
	void KVManager::runOperation( KVManagerLog *log )
	{
		
		lock.lock();

		
		// Add queues
		for ( int q = 0 ; q < log->add_queue_size() ; q++)
		{
			protocol::Queue queue = log->add_queue(q);
			
			std::string queue_name = queue.name();
			KVFormat format = getFormat( queue.format());
			queues.newKVQueue( queue_name , format );
		}
			
		// Remove queues
		for ( int q = 0 ; q < log->remove_queue_size() ; q++)
		{
			protocol::Queue queue = log->remove_queue(q);
			
			std::string queue_name = queue.name();
			queues.removeKVQueue( queue_name );
		}
		
		// Add KVSets to a particular queue
		for ( int ss = 0 ; ss < log->add_sets_size() ; ss++)
		{
			protocol::SetVector setVector = log->add_sets(ss); 

			int hash = setVector.hash();
			std::string queue_name = setVector.queue_name();
			KVQueue *queue = queues.findQueue(queue_name);
			
			for (int s = 0 ; s < setVector.set_size() ; s++)
			{
				protocol::Set set = setVector.set(s);
				
				size_t id = set.id();
				size_t task_id = set.task_id();
				size_t num_kvs = set.num_kvs();
				size_t size = set.size();
				KVFormat format = getFormat( set.format() );
				
				// Recovery of a set with this id
				KVSet *_set = sets.getKVSet( id );	// Maybe we already recovered from file
				if( !_set )
					_set = sets.addKVSet(id, task_id, format, num_kvs, size);
				
				if ( _set )
				{
					queues.addKVSetToQueue(_set, queue, hash);
				}
				
			}					
		}

		// Remove KVSets to a particular queue ( we are removing all of them )
		for ( int ss = 0 ; ss < log->remove_sets_size() ; ss++)
		{
			protocol::SetVector setVector = log->remove_sets(ss); 
			
			int hash = setVector.hash();
			std::string queue_name = setVector.queue_name();
			KVQueue *queue = queues.findQueue(queue_name);

			KVSetVector *sets =  queue->getKVSets( hash , true );	// Removing originals
			delete sets;

			// TODO: We could check sets match with the indicated in the operation
			
		}

		
		lock.unlock();

		
	}
	
	
	
	
	
#pragma mark TASKS Interface
	
	void KVManager::notifyNewTask( Task* task )
	{
		lock.lock();
		
		// Log activity
		KVManagerLog op( task->getId() , TASK_CODE_BEGIN );
		Task *parentTask = task->getParentTask();
		op.setParentTaskId( parentTask?parentTask->getId():0 );
		_logOperation( &op );
		
		lock.unlock();
	}
	
	void KVManager::notifyFinishedTask( Task* task )
	{
		lock.lock();
		sets.unlinkKVSetVectorToTask( task , &task->sets  );
		
		// Log activity
		KVManagerLog op( task->getId() , TASK_CODE_END );
		Task *parentTask = task->getParentTask();
		op.setParentTaskId( parentTask?parentTask->getId():0 );
		_logOperation( &op );
		
		lock.unlock();
	}
	
	KVSetVector * KVManager::getKVSetsForTask( Task *task, std::string queue_name , int hash , bool remove_originals )
	{
		assert( task->status == definition );
		
		KVSetVector * _sets = NULL;
		
		lock.lock();
		
		KVQueue *queue = queues.findQueue( queue_name );
		if( queue )
		{
			_sets =  queue->getKVSets( hash , remove_originals );

			// Add this sets to the list inside the task for retain / release sets for this task
			sets.linkKVSetVectorToTask( task , _sets );
			
			// Add to the vetor in task to "unlink" these sets
			task->addKVSets(_sets);
		}

		if( remove_originals )
		{
			// Log activity
			KVManagerLog op( task->getId() , TASK_CODE_ACTION );
			op.removeKVSetVectorToKVQueue(_sets, queue_name , hash );
			_logOperation( &op );
		}
		
		lock.unlock();
		
		// Wake up the save/load process
		lock.wakeUpStopLock( &kvMantainerStopLock );
		
		return _sets;
	}
		
	KVSetVector * KVManager::getAnyKVSetForTask( Task *task , std::string queue_name )		
	{
		assert( task->status == definition );
		
		KVSetVector * _sets = NULL;
		KVSet *set = NULL;
		
		lock.lock();
		
		KVQueue *queue = queues.findQueue( queue_name );
		if( queue )
		{
			_sets = new KVSetVector(queue->format);
			set = queue->getAnyKVSet(  );
			if( set )
				_sets->push_back(set);
		
			sets.linkKVSetVectorToTask( task , _sets );
		}
		
		// Add this sets to the list inside the task for retain / release sets for this task
		task->addKVSets(_sets);
		
		lock.unlock();
		
		// Wake up the save/load process
		lock.wakeUpStopLock( &kvMantainerStopLock );
		
		return _sets;
	
	}	
	

	KVQueueWriter* KVManager::getKVQueueWriter( Task* task ,  std::string queue_name )
	{
		KVQueueWriter* queueWriter = NULL;
		
		lock.lock();
		
		KVQueue *queue = queues.findQueue( queue_name );
		if ( queue )
			queueWriter = new KVQueueWriter( task , queue_name , queue->format );
		lock.unlock();

		return queueWriter;
	}
	
	

#pragma mark KVSets for flushing and loading
	
	KVSet * KVManager::getNextKVSetDiskOperation()
	{
		
		KVSet *set;
		
		while( true )
		{
		
			lock.lock();
			
			
			memoryController.dealloc( sets.freeUnusedMemory() );
			
			if( memoryController.isMemoryUsageHigh() )
			{
				set = sets.findNextToBeFlushedToDisk();
				if( !set )
				{
					set = sets.findNextToBeReadFromDisk();
					
					if( set ) // Try to get memory for it
						if( !memoryController.alloc(set->getSize()) )
						{
							memoryController.dealloc( sets.freeMemory( set->getSize() , set ) );
							if( !memoryController.alloc(set->getSize()) )
								set = NULL;
						}
					
				}
			}
			else
			{
				set = sets.findNextToBeReadFromDisk();
				if( set ) // Try to get memory for it
					if( !memoryController.alloc(set->getSize()) )
					{
						memoryController.dealloc( sets.freeMemory( set->getSize() , set ) );
						if( !memoryController.alloc(set->getSize()) )
							set = NULL;
					}
				
				if( !set )
					set = sets.findNextToBeFlushedToDisk();
			}
			
			
			if ( set )
			{
				lock.unlock();
				return set;
			}
			else
			{
				lock.unlock_waiting_in_stopLock( &kvMantainerStopLock );
			}
			
		}
	}
	
	void KVManager::notifyFinishFlushToDisk( KVSet *set )
	{
		lock.lock();
		
		set->setOnDisk();
		set->notifyFlushKVSetToDisk();
		
		
		// Log activity
		KVManagerLog op( set->task_id , TASK_CODE_ACTION );
		op.addSavedToDisk( set->getId() );
		_logOperation( &op );
		
		
		lock.unlock();
	}
	
	void KVManager::notifyFinishLoadFromDisk( KVSet *set )
	{
		lock.lock();
		set->setOnMemory();
		set->notifyReadKVSetFromDisk();
		lock.unlock();
	}
	
	

	
#pragma mark Retain and Release resources
	
	bool KVManager::retainResources(  Task* task )
	{
		lock.lock();
		
		bool resources_avaliable = sets.areResourcesAvailable( task );
		
		if( resources_avaliable )
			sets.retainResources( task );
		
		lock.unlock();
		
		return resources_avaliable;
	}		
	
	void KVManager::releaseResources(  Task* task )
	{
		lock.lock();
		sets.releaseResources( task );
		lock.unlock();

	}	
	
#pragma mark ADD KVSETs to the MAnager

	
	KVSet* KVManager::addKVSet( size_t task_id , KVSetBufferBase *buffer )
	{
		lock.lock();
		KVSet * set = sets.addKVSet( buffer , task_id );
		
		// Log activity
		KVManagerLog op( task_id , TASK_CODE_ACTION );
		op.addKVSet( set->getId() );
		_logOperation( &op  );
		
		
		lock.unlock();
		
		return set;
	}

	
	
	void KVManager::addKVSetToKVQueue(size_t task_id ,  KVSet *set , std::string queue_name , int hash )
	{
		lock.lock();
		
		queues.addKVSetToKVQueue( set , queue_name , hash );

		
		// Log activity
		KVManagerLog op( task_id , TASK_CODE_ACTION );
		op.addKVSetToKVQueue(set, queue_name , hash );
		_logOperation( &op );
		
		lock.unlock();
		
		lock.wakeUpStopLock( &kvMantainerStopLock );
	}
	
	
#pragma mark SHOW sets
	
	std::string KVManager::showSets()
	{
		lock.lock();
		std::string tmp = sets.showSets();
		lock.unlock();
		
		return tmp;
	}
	
	KVFormat KVManager::getKVQueueFormat( std::string name )
	{
		KVFormat format;

		lock.lock();
		KVQueue *queue = queues.findQueue( name );
		if( queue )
			format = queue->format;
		lock.unlock();
		
		return format;
			
	}

	
	void KVManager::newKVQueue( size_t task_id, std::string queue_name , KVFormat format )
	{
		lock.lock();
		
		// Log activity
		KVManagerLog *op = new KVManagerLog( task_id , TASK_CODE_ACTION );
		op->addQueue(queue_name , format );
		_logOperation( op  );
		delete op;
		
		
		queues.newKVQueue( queue_name , format );
		
		
		lock.unlock();
		
	}

	void KVManager::removeKVQueue(size_t task_id,  std::string queue_name )
	{

		lock.lock();
		
		// Log activity
		KVManagerLog *op = new KVManagerLog( task_id , TASK_CODE_ACTION );
		op->removeQueue( queue_name );
		_logOperation( op  );
		delete op;
		
		
		queues.removeKVQueue(queue_name );
		lock.unlock();
		
		
	}
	
	void KVManager::connectQueues( std::string queue_name, std::vector<std::string> connected_queue_names  )
	{
		lock.lock();
		
		KVQueue *queue = queues.findQueue( queue_name );
		if( queue )
			for (size_t i = 0 ; i < connected_queue_names.size() ; i++)
				queue->distributionQueues.push_back( connected_queue_names[i] );
		
		lock.unlock();
		
		lock.wakeUpStopLock( &kvMantainerStopLock );
	}
	
	std::string KVManager::showQueues()
	{
		lock.lock();
		
		std::string txt = queues.str();
		lock.unlock();
		
		return txt;
	}
	
	
#pragma mark Operations PROCESS
	
	
	void KVManager::addProcess( std::string queue_name , std::string operation_name , std::vector<std::string> output_queues )
	{
		lock.lock();
		
		KVQueueProcess *p = new KVQueueProcess(  queue_name , operation_name , output_queues);
		process.push_back( p );
		
		lock.unlock();
		
		
	}
	
	std::string KVManager::showProcess()
	{
		lock.lock();
		
		std::ostringstream o;
		for ( size_t i = 0 ; i < process.size() ; i++)
			o << process[i]->str() << std::endl;
		
		lock.unlock();
		
		return o.str();
	}
	
	void KVManager::scheduleAutomaticTasks()
	{
		
		std::vector<std::string> commands;

		lock.lock();
		
		for (size_t  i = 0 ; i < process.size() ; i++)
		{
			KVQueueProcess *p = process[i];
			
			KVQueue *queue = queues.findQueue( p->input_queues[0] );
			
			if( queue->data.num_kvs > 100 )
			{
				std::stringstream command;
				
				command << p->operation_name << " ";
				for (size_t i = 0 ; i < p->input_queues.size() ; i++)
					command << p->input_queues[i] << " ";
				for (size_t i = 0 ; i < p->output_queues.size() ; i++)
					command << p->output_queues[i] << " ";
				
				command << " -remove";
				// Create the tasks to process this queue
				commands.push_back( command.str() );
			}
			
		}
		
		lock.unlock();
		
		
		for (size_t i = 0 ; i < commands.size() ; i++)
		{
			TaskManager::shared()->runCommand( NULL , commands[i] , false );
		}
		
	}
	
	
	
#pragma mark OPERATIONS WITH MEMORY
	
	void KVManager::addAvailableMemory( size_t memory )
	{
		lock.lock();
		memoryController.addAvailableMemory( memory);
		lock.unlock();
	}
	
	
	void KVManager::alloc( size_t size  )
	{
		while( true )
		{
			lock.lock();

			if( memoryController.alloc( size ) ) 
			{
				lock.unlock();
				return;
			}
			
			// Try to free as much memory as possible to alloc new space
			memoryController.dealloc(  sets.freeMemory(  size - memoryController.availableMemory() , NULL ) );
			
			
			if( memoryController.alloc(size) )
			{
				lock.unlock();
				return;
			}

			lock.unlock();
			sleep(1);
			
		}
	}
	
	void KVManager::dealloc( size_t size  )
	{
		lock.lock();
		memoryController.dealloc(size);
		lock.unlock();
	}
	
	
	std::string KVManager::strMemory()
	{
		lock.lock();
		std::string tmp = memoryController.str();
		lock.unlock();
		return tmp;
	}
	

	
	
	
	
}



