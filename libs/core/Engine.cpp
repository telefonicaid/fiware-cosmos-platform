

#include "logMsg.h"				// LM_X
#include <time.h>
#include <sys/time.h>

#include "Engine.h"							// Own interface
#include "EngineElement.h"					// ss::EngineElement
#include "SamsonSetup.h"					// ss::SamsonSetup
#include "ProcessItem.h"					// ss::ProcessItem
#include "EngineDelegates.h"		
#include "DiskOperation.h"					// ss::DiskOperation

namespace ss
{
	
	Engine *e=NULL;		// Static singlelton for thir worker
	
	void*runEngineBakground(void*e)
	{
		((Engine*)e)->run();
		return NULL;
	}
	
	
	Engine::Engine()
	{
		_quit = false;									// By default, this flag is false
		_running =  false;								// Not running until "run" method is called
		
		pthread_mutex_init(&elements_mutex, 0);			// Mutex to protect elements
		pthread_cond_init(&elements_cond, 0) ;			// Conditional to block the thread while waiting the next event
		
		// Take the num of process from setup and init the current number of concurrent process
		num_processes = SamsonSetup::shared()->num_processes;
		
		
		// Number of parallel disk operations
		num_disk_operations = 1;
		
	}
	
	Engine::~Engine()
	{
		pthread_mutex_destroy(&elements_mutex);			// Mutex to protect elements
		pthread_cond_destroy(&elements_cond) ;			// Conditional to block the thread while waiting the next event
	}
	
	void Engine::init()
	{
		if( e )
			LM_X(1,("Error initializing twice the Samson Engine"));
		
		LM_T( LmtEngine , ("Engine init"));

		e = new Engine();
		
	}

	void Engine::destroy()
	{
		if( !e )
			LM_X(1,("Error destroying a non initialized Samson Engine"));
		
		LM_T( LmtEngine , ("Engine destroying"));
		
		delete e;
		e = NULL;
	}
	

	Engine *Engine::shared()
	{
		if( !e )
			LM_X(1,("SamsonEngine not initialized"));
		return e;
	}

	
	void Engine::runInBackground()
	{
		pthread_create(&t, 0, runEngineBakground, e);	
	}

	void Engine::run()
	{
		// Keep the thread for not calling quit from the same thread
		t = pthread_self();	
		
		// Add the Nothing Engine Element to loop continuously
		add( new ss::NothingEngineElement() );
		
		LM_T( LmtEngine , ("Engine run"));
		_running =  true;
		
		while( true )
		{
			
			pthread_mutex_lock(&elements_mutex);

			time_t now = time(NULL);
			//LM_M(("Loop of the engine time: %u elements: %d", now , elements.size() ));
			
			if( elements.size() == 0)
			{
				// No more things to do
				LM_T( LmtEngine, ("SamsonEngine: No more elements to process in the engine. Quiting ..."));
				pthread_mutex_unlock(&elements_mutex);

				_running = false;	// Flag to indicate that the engine is not running any more
				
				return;
			}

			if( now >= elements.front()->getThriggerTime() )
			{
				//LM_M(("Something to execute...."));
				
				EngineElement *element = elements.front();
				elements.pop_front();

				// Unlock the thread
				pthread_mutex_unlock(&elements_mutex);
				
				// Execute the item

				LM_T( LmtEngine, ("[START] Engine executing %s" , element->getDescription().c_str()));

				element->run();

				LM_T( LmtEngine, ("[DONE] Engine executing %s" , element->getDescription().c_str()));
				
				
				if( element->isRepeated() )
				{
					// Insert again
					element->Reschedule();
					add( element );
				}
				else
					delete element;
				
			}
			else
			{
				int seconds = elements.front()->getThriggerTime() - now;
				
				struct timeval tv;
				struct timespec ts;
				gettimeofday(&tv, NULL);
				ts.tv_sec = tv.tv_sec + seconds;
				ts.tv_nsec = 0;
				
				//LM_M(("Sleeping %d seconds",  seconds ));
				
				pthread_cond_timedwait(&elements_cond, &elements_mutex , &ts );
				pthread_mutex_unlock(&elements_mutex);
				
				//LM_M(("Waking up after sleeping %d seconds",  seconds ));
				
			}
		}
		
	}
	
	void Engine::quit()
	{
		
		
		pthread_mutex_lock(&elements_mutex);

		// Flag to avoid more "adds"
		_quit = true;	

		// Remove All elements
		elements.clearList();		
		
		// Wake up the main thread to process this element if necessary	
		pthread_cond_signal(&elements_cond);
		
		pthread_mutex_unlock(&elements_mutex);
			
		// If we are calling quit from another thread, we will wait until the main thead is finished
		if( pthread_self() != t )
		{
			
			LM_M(("Waiting samson engine to finish...."));
			
			while( _running )
				sleep(1);
			
			LM_M(("Finish waiting samson engine to finish...."));
		}
		
	}
			
	void Engine::add( EngineElement *element )
	{
		pthread_mutex_lock(&elements_mutex);
		
		if( _quit )
		{
			// Not adding this element
			delete element;
		}
		else
		{
			elements.insert( _find_pos( element ) ,  element );

			// Wake up the main thread to process this element if necessary	
			pthread_cond_signal(&elements_cond);
		}
		
		pthread_mutex_unlock(&elements_mutex);
		
		
	}

	void Engine::addProcessItem( ProcessItem *item )
	{
		LM_T( LmtEngine , ("Adding ProcessItem") );
		
		pthread_mutex_lock(&elements_mutex);
		
		if( _quit )
		{
			// Not adding this element
			delete item;
		}
		else
		{
			items.insert( item );
		}
		
		LM_T( LmtEngine , ("Engine state for background process: Pending %u Running %u Halted %u", items.size() , running_items.size() , halted_items.size()  ) );
		
		pthread_mutex_unlock(&elements_mutex);

		
		// Check background processes to see if it is necessary to run new stuff
		checkBackgroundProcesses();
		
	}
	
	
	std::list<EngineElement*>::iterator Engine::_find_pos( EngineElement *e)
	{
		for (std::list<EngineElement*>::iterator i = elements.begin() ; i != elements.end() ; i++)
		{
			if( (*i)->getThriggerTime() > e->getThriggerTime() )
				return i;
		}
		
		return elements.end();
	}
	
	ProcessItem* Engine::_getNextItemToRun()
	{
		
		ProcessItem* item = NULL;
		
		// Halted process comes first 
		
		for (std::set<ProcessItem*>::iterator i = halted_items.begin() ; i != halted_items.end() ; i++ )
		{
			if( (*i)->isReady() )
			{
				item = *i;
				halted_items.erase( item );
				return item;
			}
		}
		
		// If not process in the halt list is ready,
		// we get the highest priority element in the queue of pending processes
		
		for ( std::set<ProcessItem*>::iterator i =  items.begin() ; i!= items.end() ; i++)
		{
			if( !item )
				item = *i;
			else
			{
				if( (*i)->priority > item->priority )
					item = *i;
			}
		}
		
		if( item )
			items.erase( item );	// Remove form the pending list
		
		// It is null if no process is required to be executed
		return item;
		
	}	
	
	void Engine::finishProcessItem( ProcessItem *item )
	{
		pthread_mutex_lock(&elements_mutex);
		running_items.erase(item);
		pthread_mutex_unlock(&elements_mutex);
		
		checkBackgroundProcesses();
	}
	
	void Engine::haltProcessItem( ProcessItem *item )
	{
		pthread_mutex_lock(&elements_mutex);
		
		running_items.erase(item);
		halted_items.insert(item);
		
		pthread_mutex_unlock(&elements_mutex);
		
		checkBackgroundProcesses();
		
	}
	
	
	void Engine::checkBackgroundProcesses()
	{
		pthread_mutex_lock(&elements_mutex);
		
		// Get the next process item to process ( if CPU slots available )
		ProcessItem * item = NULL;
		
		do
		{
			
			item = NULL;
			if( (int)running_items.size() < num_processes )
			{
				// Space for another process running... get the item to be executed or continued
				item = _getNextItemToRun();
			}
			
			if( item )
			{
				running_items.insert( item );	// Insert in the set of running processes
				
				switch (item->state) {
					case ProcessItem::queued:
						item->state = ProcessItem::running;
						item->runInBackground();
						break;
					case ProcessItem::halted:
						item->state = ProcessItem::running;
						item->unHalt();
						break;
					default:
						assert(false);
						break;
				}
			}
			
		} while ( item != NULL);
		
		LM_T( LmtEngine , ("Engine state for background process: Pending %u Running %u Halted %u", items.size() , running_items.size() , halted_items.size()  ) );

		pthread_mutex_unlock(&elements_mutex);
	}
	
#pragma mark -----
	
	void Engine::add( DiskOperation *operation )
	{
		pthread_mutex_lock(&elements_mutex);
		pending_operations.push_back( operation );
		pthread_mutex_unlock(&elements_mutex);

		// Check if we can start this operation
		checkDiskOperations();
	}
	
	void Engine::finishDiskOperation( DiskOperation *operation )
	{
		pthread_mutex_lock(&elements_mutex);
		
		running_operations.erase( operation );
		
		diskStatistics.add( operation );
		
		pthread_mutex_unlock(&elements_mutex);
		
		// Add a notification for this ( removed when delegate is notified )
		add( new DiskManagerNotification( operation ) );
		
		// Check if there are more operation to be executed
		checkDiskOperations();
	}
	
	// Check if we can run more disk operations
	void Engine::checkDiskOperations()
	{
		pthread_mutex_lock(&elements_mutex);

		
		while( ( pending_operations.size() > 0 ) && ( running_operations.size() < (size_t)num_disk_operations ) )
		{
			// Extract the next operation
			DiskOperation *operation = pending_operations.extractFront();
			
			running_operations.insert( operation );
			
			// Run in background
			operation->runInBackGround();
		}
		
		
		pthread_mutex_unlock(&elements_mutex);
		
	}
	
	
	
#pragma mark ----
	
	
	void Engine::fill(network::WorkerStatus*  ws)
	{
		// Fill the memory manager stuff
		memoryManager.fill( ws );

		
		pthread_mutex_lock(&elements_mutex);
		
		
		// Process Manager Status
		// ----------------------------------------------------------------------------
		std::ostringstream process_manager_status;
		
		process_manager_status << "\n\t\tRunning: ";
		for ( std::set<ProcessItem*>::iterator i = running_items.begin () ; i != running_items.end() ; i++ )
			process_manager_status << "[" << (*i)->getStatus() << "] ";
		
		process_manager_status << "\n\t\tHalted: ";
		for ( std::set<ProcessItem*>::iterator i = halted_items.begin () ; i != halted_items.end() ; i++ )
			process_manager_status << "[" << (*i)->getStatus() << "] ";
		
		process_manager_status << "\n\t\tQueued: ";
		for ( std::set<ProcessItem*>::iterator i = items.begin () ; i != items.end() ; i++ )
			process_manager_status << "[" << (*i)->getStatus() << "] ";
		
		ws->set_process_manager_status( process_manager_status.str() );
		
		// Disk Manager
		// ----------------------------------------------------------------------------
		std::ostringstream disk_manager_status;

		disk_manager_status << "\n\t\tRunning: ";
		for ( std::set<DiskOperation*>::iterator i = running_operations.begin() ; i != running_operations.end() ; i++)
			disk_manager_status << "[" << (*i)->getDescription() << "] ";
		
		disk_manager_status << "\n\t\tQueued: ";
		for ( au::list<DiskOperation>::iterator i = pending_operations.begin() ; i != pending_operations.end() ; i++)
			disk_manager_status << "[" << (*i)->getShortDescription() << "] ";

		disk_manager_status << "\n\t\tStatistics: ";
		disk_manager_status << diskStatistics.getStatus();
		
		ws->set_disk_manager_status( disk_manager_status.str() );
		
		pthread_mutex_unlock(&elements_mutex);
		
	}	
	
	
	
}