

#include "logMsg.h"				// LM_X
#include <time.h>
#include <sys/time.h>

#include "Engine.h"							// Own interface
#include "EngineElement.h"					// ss::EngineElement
#include "SamsonSetup.h"					// ss::SamsonSetup
#include "ProcessItem.h"					// ss::ProcessItem
#include "DiskOperation.h"					// ss::DiskOperation
#include "EngineNotificationElement.h"      // ss::EngineNotificationElement

namespace ss
{

    
#pragma mark ENGINE
	
	Engine *e=NULL;		// Static singlelton for thir worker
	
	void*runEngineBakground(void*e)
	{
		((Engine*)e)->run();
		return NULL;
	}
	
    void destroy_engine()
    {
        if( e )
        {
            e->quit();   // Quit if still not quited
            delete e;
        }
        e = NULL;
    }
	
	Engine::Engine()
	{
		_quit = false;									// By default, this flag is false
		_running =  false;								// Not running until "run" method is called
		
        _sleeping_seconds = 0;
        
		pthread_mutex_init(&elements_mutex, 0);			// Mutex to protect elements
		pthread_cond_init(&elements_cond, 0) ;			// Conditional to block the thread while waiting the next event
        
        // Add the MemoryManager as a listner for the memory_request channel
        notificationSystem.add( notification_memory_request, &memoryManager );
        
        // Add the processManager as a listener for process request
        notificationSystem.add( notification_process_request , &processManager );

        // Add the diskManager as a listener for disk operations
        notificationSystem.add( notification_disk_operation_request , &diskManager );
        
	}
	
	Engine::~Engine()
	{
        
		//LM_M(("Destroying Samson Engine"));
        
		pthread_mutex_destroy(&elements_mutex);			// Mutex to protect elements
		pthread_cond_destroy(&elements_cond) ;			// Conditional to block the thread while waiting the next event
        
        // clear the pending elements to be executed
        elements.clearList();
        
	}
	
	void Engine::init()
	{
		if( e )
			LM_X(1,("Error initializing twice the Samson Engine"));
		
		LM_T( LmtEngine , ("Engine init"));

		e = new Engine();
	
        
        // At exit destroy
        atexit( destroy_engine  );

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

		// Repeated notification for not ending the loop of the engine
        notify( new EngineNotification( notification_nothing ) , 5 );
		
		LM_T( LmtEngine , ("Engine run"));
		_running =  true;
		
        counter = 0;    // Init the counter to elements managed by this run-time
		while( true )
		{
			counter++;  // Keep a total counter of loops
            
			pthread_mutex_lock(&elements_mutex);

			time_t now = time(NULL);
			//LM_M(("Loop of the engine time: %u elements: %d", now , elements.size() ));
			
			if( elements.size() == 0)
			{
				// No more things to do
				LM_T( LmtEngine, ("SamsonEngine: No more elements to process in the engine. Quitting ..."));
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
                
                time_t trigger_time = elements.front()->getThriggerTime();
                
                if( trigger_time > now )
                    _sleeping_seconds =  trigger_time - now;
                else
                    LM_X(1,("Time triggered is not greater than now..."));
				
                // LM_M(("Sleeping time in seconds %d" , _sleeping_seconds ));
                
				struct timeval tv;
				struct timespec ts;
				gettimeofday(&tv, NULL);
				ts.tv_sec = tv.tv_sec + _sleeping_seconds;
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
        
	   //LM_M(("Quitting samson engine...."));
        
       if( ! _running )
            return; // Not necessary to quit anything.

		// Flag to avoid more "adds"
		_quit = true;	
        
        // LM_M(("Quitting process manager...."));
        
        // Quit the process manager means remove all pending processes and wait for the current ones.
        processManager.quit();

        // LM_M(("Quitting disk manager...."));
        
        // Remove pending disk operations and wait for the running ones
        diskManager.quit();
        
		pthread_mutex_lock(&elements_mutex);

		// Remove All elements
		elements.clearList();		
		
		// Wake up the main thread to process this element if necessary	
		pthread_cond_signal(&elements_cond);
		
		pthread_mutex_unlock(&elements_mutex);
			
		// If we are calling quit from another thread, we will wait until the main thead is finished
		if( pthread_self() != t )
		{
			
			// LM_M(("Awaiting samson engine to finish...."));
			
			while( _running )
				sleep(1);
			
			//LM_M(("Samson engine completely finished"));
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
	
	
	std::list<EngineElement*>::iterator Engine::_find_pos( EngineElement *e)
	{
		for (std::list<EngineElement*>::iterator i = elements.begin() ; i != elements.end() ; i++)
		{
			if( (*i)->getThriggerTime() > e->getThriggerTime() )
				return i;
		}
		
		return elements.end();
	}
	
#pragma mark -----
    
    void Engine::notify( EngineNotification*  notification )
    {
        // Add an element to notify latter
        add( new EngineNotificationElement( notification ) );
    }
    
    void Engine::notify( EngineNotification*  notification , int seconds )
    {
        // Add an element to notify latter
        add( new EngineNotificationElement( notification , seconds ) );
    }
    
	
#pragma mark ----
	
	void Engine::fill(network::WorkerStatus*  ws)
	{
		// Fill the memory manager stuff
		memoryManager.fill( ws );
        diskManager.fill( ws );
        processManager.fill( ws );
		
		pthread_mutex_lock(&elements_mutex);
        
        std::ostringstream engine_state;
        engine_state << "Loops: " << counter;
        engine_state << " Current: ";
        
        if( running_element )
            engine_state << running_element->getDescription();
        else
            engine_state << "( Sleeping for " << _sleeping_seconds << ")";
        
        engine_state <<  " Queue: " << elements.size() << " ";
        for ( au::list<EngineElement>::iterator el = elements.begin() ; el != elements.end() ; el++)
            engine_state << "[" << (*el)->getShortDescription() <<"]";
        
		pthread_mutex_unlock(&elements_mutex);
        
        ws->set_engine_status( engine_state.str() );
        
        
	}	
	
}
