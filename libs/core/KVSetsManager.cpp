#include <sstream>

#include "KVSet.h"
#include "SSMonitor.h"
#include "KVQueue.h"
#include "SSLogger.h"
#include "Task.h"                /* Task                                     */
#include "KVSetsManager.h"       /* Own interface                            */



namespace ss {

	// Singleton implementation of the KVSetManager
	
	KVSetsManager::KVSetsManager(  )
	{
		current_KVSetId = 0;		// Init the identifier counter for the next KVSet
		
		top = NULL;
		bottom = NULL;
	}
	
	KVSet* KVSetsManager::addKVSet( KVSetBufferBase *buffer , size_t task_id )
	{

		KVSet * set = new KVSet( current_KVSetId, task_id , fileNameForKVSet( current_KVSetId ) ,   buffer );	// Create the KVSet

		current_KVSetId++;					// Prepare the counter for the next KVSet
		insert( set );						// Insert into the double-linked list of sets
		
		return set;
	}	
	
	KVSet* KVSetsManager::addKVSet( size_t id ,size_t task_id  ,  KVFormat format , size_t num_kvs , size_t size )
	{
		KVSet *set = new KVSet( id , task_id , fileNameForKVSet( id ) );
		insert(set);
		
		if( current_KVSetId <= id )
			current_KVSetId = id+1;
		
		return set;
	}
	
	
	KVSet * KVSetsManager::getKVSet( size_t id )
	{
		KVSet *b = top;
		while( b )
		{
			if( b->id == id )
				return b;
			b = b->bottom;
		}
		return NULL;
	}
	
	bool KVSetsManager::all_on_disk()
	{
		KVSet *b = top;
		while( b )
		{
			if( !b->isOnDisk() )
				return false;
			b = b->bottom;
		}
		return true;
	}
	
	std::string KVSetsManager::fileNameForKVSet( size_t id )
	{
		std::ostringstream o;
		o << SAMSON_DATA_DIRECTORY << "/data_" << id;
		return o.str();
	}
	
	std::string KVSetsManager::showSets()
	{
		std::ostringstream o;
		for (KVSet *b = top ; b ; b = b->bottom )
			o << b->str() << std::endl;
		
		return o.str();
	}		
	
	void KVSetsManager::extract( KVSet *b )
	{
		if( top == b )
			top = b->bottom;
		
		if ( bottom == b)
			bottom = b->top;
		
		if( b->top )
			b->top->bottom = b->bottom;
		
		if (b->bottom )
			b->bottom->top = b->top;
		
		b->top = NULL;
		b->bottom = NULL;
		
	}
	
	void KVSetsManager::insert( KVSet *b )
	{
		
		if( !top )
		{
			assert( !bottom );
			top = b;
			bottom = b;
			return;
		}
		
		KVSet *t = top;
		
		while( t && t->compare(b) > 0)
		{
			
			t = t->bottom;
		}
		
		
		if( t )
		{
			// Insert on top of t
			insert_on_top(b, t);
		}
		else
		{
			// Insert at the bottom of the stack
			bottom->bottom = b;
			b->top = bottom;
			bottom = b;
		}
		
		
	}
	
	
	void KVSetsManager::insert_on_top( KVSet *b , KVSet *ref )
	{
		b->top = ref->top;
		b->bottom = ref;
		
		if ( ref->top )
			ref->top->bottom = b;
		
		ref->top = b;
		
		if ( top == ref )
			top = b;
		
	}
	
	void KVSetsManager::insert_bellow( KVSet *b , KVSet *ref )
	{
		b->top = ref;
		b->bottom = ref->bottom;
		
		
		if ( ref->bottom )
			ref->bottom->top = b;
		
		ref->bottom = b;
		
		if ( bottom == ref )
			bottom = b;
		
	}
	
	size_t KVSetsManager::remove( KVSet *b )
	{
		size_t free_memory = 0 ;
		assert( b->queues.size()==0 );	// This shoudl be since it is not in any queue
		
		// --------------------------------
		if( b->isOnMemory() )
		{
			free_memory += b->freeBuffer( );	
		}
		extract(b);
		delete b;
		// --------------------------------
		
		return free_memory;
	}
	

	size_t KVSetsManager::freeUnusedMemory()
	{
		size_t total = 0 ;
		
		KVSet *b = top;
		while( b )
		{
			
			if( b->shouldBeRemoved() )
			{
				KVSet *next = b->bottom;
				total += remove(b);
				b = next;
			}
			else
				b = b->bottom;
		}
		
		
		return total;
	}
	
	
	
	KVSet * KVSetsManager::findNextToBeReadFromDisk( )
	{
		KVSet *b = top;
		while( b )
		{
			
			if( b->shouldBeRemoved() )
			{
				KVSet *next = b->bottom;
				//remove(b);
				b = next;
			}
			else
			{
				if ( b->shouldBeReadFromDisk() )
					return b;
				b = b->bottom;
			}
		}

		return NULL;
	}
	
	KVSet * KVSetsManager::findNextToBeFlushedToDisk( )
	{
		KVSet *b = bottom;
		while( b )
		{
			if( b->shouldBeRemoved() )
			{
				KVSet *next = b->top;
				//remove(b);
				b = next;
			}
			else
			{
				if( b->shouldBeFlushedToDisk() )
					return b;
				b = b->top;
			}
		}
		
		return NULL;
	}


	
	size_t KVSetsManager::freeMemory( size_t necessary_size ,  KVSet *higherPriorityKVSet )
	{
		size_t total =0;
		
		KVSet *b = bottom;
		while ( b )
		{
			if( b == higherPriorityKVSet )
				return total;					// We can not free more memory
			
			if( b->shouldBeRemoved() )
			{
				KVSet *next = b->top;
				total += remove(b);
				b = next;
			}
			else
			{
				if( b->canBeFree() )					// Free if possible
				{
					total += b->freeBuffer();
					b->notifyFreeKVSet();		// Notify to the queues that contains me
				}
				
				b = b->top;
			}
			
			// Enougth free memory
			if( total >= necessary_size )
				return total;
		}
		
	
		return total;
	}
	
	
	
	
	void KVSetsManager::linkKVSetVectorToTask(Task *task ,  std::vector<KVSet*>*_sets)
	{
		for (  size_t i = 0 ; i < _sets->size() ; i++)
		{
			(*_sets)[i]->add_task( task->getId() );
			sort( (*_sets)[i] );	// Resort element taking into account the task
		}
		
	}
	
	void KVSetsManager::unlinkKVSetVectorToTask(Task *task ,  std::vector<KVSet*> *_sets)
	{
		for (  size_t i = 0 ; i < _sets->size() ; i++)
		{
			(*_sets)[i]->remove_task( task->getId() );
			sort( (*_sets)[i] );	// Resort element taking into account the task
		}
		
	}
	
	bool KVSetsManager::areResourcesAvailable( Task * task )
	{
		// Let's check everything is on memory ( if I can block, nobody can release )
		for (  std::vector<KVSet*>::iterator i = task->sets.begin() ;  i < task->sets.end() ; i++ )
			if( ! (*i)->isOnMemory() )
				return false;
		
		return true;
	}
	
	void KVSetsManager::retainResources(  Task* task )
	{
		
		for (  std::vector<KVSet*>::iterator i = task->sets.begin() ;  i < task->sets.end() ; i++ )
			(*i)->retainToUse( );
	}
	
	void KVSetsManager::releaseResources(  Task* task )
	{
		for (  std::vector<KVSet*>::iterator i = task->sets.begin() ;  i < task->sets.end() ; i++ )
			(*i)->releaseToUse( );
	}		
	
	
	
}

