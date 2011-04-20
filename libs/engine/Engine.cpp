

#include "logMsg.h"				// LM_X
#include <time.h>
#include <sys/time.h>

#include "Engine.h"							// Own interface
#include "EngineElement.h"					// ss::EngineElement
#include "ProcessItem.h"					// ss::ProcessItem
#include "DiskOperation.h"					// ss::DiskOperation
#include "EngineNotificationElement.h"      // ss::EngineNotificationElement

namespace engine
{

    
#pragma mark ENGINE
	
	Engine *engine = NULL;		// Static singlelton for thir worker
	
	void*runEngineBakground(void*e)
	{
        Engine::run();
		return NULL;
	}
	
    void destroy_engine()
    {
        LM_M(("Engine terminating..."));
        
        if( engine )
        {
            engine->_quit();   // Quit if still not quited
            delete engine;
        }
        engine = NULL;
        
        LM_M(("Engine terminated"));
    }
	
	Engine::Engine()
	{
		flag_quit = false;					       // By default, this flag is false
		flag_running =  false;				       // Not running until "run" method is called
		
		_sleeping_seconds = 0;
    
        running_element = NULL;
        
		pthread_mutex_init(&elements_mutex, 0);			// Mutex to protect elements
		pthread_cond_init(&elements_cond, 0) ;			// Conditional to block the thread while waiting the next event
        
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
		if( engine )
			LM_X(1,("Please, init the engine only once "));
		
		LM_T( LmtEngine , ("Engine init"));

		engine = new Engine();
	
        // At exit destroy
        atexit( destroy_engine  );

	}
    
    // ------------------------------------------------------------------------------------------------------------------------------
    
    void Engine::run()
    {
        if (!engine)
            LM_X(1, ("Please init engine before executing"));
        
        engine->_run();
    }
    
    void Engine::runInBackground()
    {
        if (!engine)
            LM_X(1, ("Please init engine before executing"));

		pthread_create(&engine->t, 0, runEngineBakground, NULL);	
        
    }
        
    void Engine::add( EngineElement *element )
    {
        if (engine)
            engine->_add( element );
        else
        {
            LM_W(("Not adding an EngineElement since Engine is not initialized"));
            delete element;
        }
        
    }
    
    void Engine::notify( Notification*  notification )
    {
        if (engine)
        {
            // Add an element to notify latter
            engine->add( new NotificationElement( notification ) );
        }
        else
        {
            LM_W(("Removing a Notification since engine is not initialized"));
            delete notification;
        }
        
    }

    void Engine::_notify( Notification*  notification )
    {
        if ( engine )
        {
            // Add an element to notify latter
            engine->notificationSystem.notify( notification );
        }
        else
        {
            LM_W(("Removing a Notification since engine is not initialized"));
            delete notification;
        }
        
    }
    
    
    
    void Engine::notify( Notification*  notification , int seconds )
    {
        if (engine)
            engine->add( new NotificationElement( notification, seconds ) );
        else
        {
            LM_W(("Removing a Notification since engine is not initialized"));
            delete notification;
        }
        
    }
    
    void Engine::add( std::string name , NotificationListener*listener)
    {
        if( engine )
            engine->notificationSystem.add( name , listener );
        else
            LM_W(("Not adding a listener since engine is not initialized"));
    }

    void Engine::remove( NotificationListener* listener )
    {
        if( engine )
            engine->notificationSystem.remove( listener );
        else
            LM_W(("Not removing a listener since engine is not initialized"));
        
    }

    std::string Engine::str()
    {
        if( engine )
            return engine->_str();
        else
            return "Engine not initialized";
        
    }

    
    
    // ------------------------------------------------------------------------------------------------------------------------------
	

	void Engine::_run()
	{
		// Keep the thread for not calling quit from the same thread
		t = pthread_self();	

		// Repeated notification for not ending the loop of the engine
		notify( new Notification( "endless loop notification" ) , 5 );
		
		LM_T( LmtEngine , ("Engine run"));
		flag_running =  true;
		
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

				flag_running = false;	// Flag to indicate that the engine is not running any more
				
				return;
			}

			if( now >= elements.front()->getThriggerTime() )
			{
				//LM_M(("Something to execute...."));
				
				running_element = elements.front();
				elements.pop_front();

				// Unlock the thread
				pthread_mutex_unlock(&elements_mutex);
				
				// Execute the item

				LM_T( LmtEngine, ("[START] Engine executing %s" , running_element->getDescription().c_str()));

				running_element->run();

				LM_T( LmtEngine, ("[DONE] Engine executing %s" , running_element->getDescription().c_str()));
                    
                EngineElement * _running_element = running_element;

                running_element = NULL; // Put running element to NULL for the str function
                
				if( _running_element->isRepeated() )
				{
					// Insert again
					_running_element->Reschedule();
					add( _running_element );
				}
				else
					delete _running_element;
                
				
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
	
	void Engine::_quit()
	{
        
	   //LM_M(("Quitting samson engine...."));
        
       if( ! flag_running )
            return; // Not necessary to quit anything.

		// Flag to avoid more "adds"
		flag_quit = true;	
        
		pthread_mutex_lock(&elements_mutex);

		// Remove All elements
		elements.clearList();		
		
		// Wake up the main thread to process this element if necessary	
		pthread_cond_signal(&elements_cond);
		
		pthread_mutex_unlock(&elements_mutex);
			
		// If we are calling quit from another thread, we will wait until the main thead is finished
		if( pthread_self() != t )
		{
			while( flag_running )
            {
                LM_M(("Awaiting samson engine to finish...."));
				sleep(1);
            }
		}
		
	}
			
	void Engine::_add( EngineElement *element )
	{
		pthread_mutex_lock(&elements_mutex);
		
		if( flag_quit )
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
	
    
	
#pragma mark ----

    std::string Engine::_str()
	{
		// Fill the memory manager stuff
		
		pthread_mutex_lock(&elements_mutex);
        
        std::ostringstream engine_state;
        
        engine_state << "Engine Loops: " << counter << "\n";
        
        if( running_element )
            engine_state << "\t\t\tCurrent:\n\t\t\t\t" << running_element->getDescription() << "\n";
        else
            engine_state << "( Sleeping for " << _sleeping_seconds << ")\n";
        
        engine_state <<  "\t\t\tQueue: " << elements.size() << "\n";
        for ( au::list<EngineElement>::iterator el = elements.begin() ; el != elements.end() ; el++)
            engine_state << "\t\t\t\t[" << (*el)->getShortDescription() <<"]\n";
        
		pthread_mutex_unlock(&elements_mutex);
        
        return engine_state.str();
        
        
	}	
}
