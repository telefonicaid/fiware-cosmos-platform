


#include <time.h>
#include <sys/time.h>
#include <signal.h>   // SIGKILL

#include "logMsg/logMsg.h"                          // LM_X

#include "au/Stopper.h"                             // au::Stopper
#include "au/ErrorManager.h"                       // au::ErrorManager


#include "Notification.h"                           // engine::Notification

#include "engine/EngineElement.h"					// engine::EngineElement
#include "engine/ProcessItem.h"                     // engine::ProcessItem
#include "engine/DiskOperation.h"					// engine::DiskOperation
#include "engine/EngineNotificationElement.h"       // engine::EngineNotificationElement

#include "engine/Engine.h"							// Own interface

#define ENGINE_MAX_RUNNING_TIME     60

namespace engine
{

    // Thread stuff
    // ---------------------------------------------------------------------------
    
    // General mutex to protect global variable engine and block the main thread if necessary
    au::Token engine_token("EngineToken");         
    
#pragma mark ENGINE
	
	Engine *engine = NULL;		// Static singlelton for thir worker
	

    // Thread creation functions
    // ---------------------------------------------------------------------------
    
    
	void* runEngineBakground(void* e )
	{
        engine->run();
		return NULL;
	}

	void* runEngineCheckBakground(void* e)
	{
        while( true )
        {
            bool valid = false;
            
            {
                au::TokenTaker tt(&engine_token);
                
                if( !engine )
                {
                    LM_M(("Background thread for engine cheking ended since engine is not active anymore"));
                    return NULL; // No more check necessary
                }
                
                valid =  engine->check();
            }
            
            if ( !valid )
                LM_X(1, ("Error in the check-engine"));
            else
                sleep( ENGINE_MAX_RUNNING_TIME /2 );

        }
		return NULL;
	}
    
    void destroyEngine()
    {
        // When finishing this thread, exit que engine to finish the app
        engine::Engine::destroy();
    }

    bool isEngineInitialized()
    {
        // Mutex protection
        au::TokenTaker tt(&engine_token);
        return ( engine != NULL );
    }
    
	
	Engine::Engine()
	{
		flag_quit = false;                          // By default, this flag is false
		flag_running =  true;                       // The run method is automatically called 
		
		sleeping_time_seconds = 0;                  // Init the sleep time variable
        running_element = NULL;                     // Initially, no running element

	}
	
	Engine::~Engine()
	{
        
		LM_M(("Engine: Destructor"));
        
        // clear the pending elements to be executed
        elements.clearList();
        
	}
	
	void Engine::init()
	{
        // Mutex protection
        au::TokenTaker tt(&engine_token);
        
		if( engine )
        {
			LM_W(("Please, init the engine only once "));
            return;
        }
		else
        {
            
            atexit( destroyEngine );
            
            LM_T( LmtEngine , ("Engine init"));
            engine = new Engine();

            // Add a simple periodic element to not die inmediatelly
            EngineElement *element = new NotificationElement( new Notification("alive") , 10 );
            engine->elements.push_back( element );
            
            LM_T( LmtEngine , ("Running engine in background...") );
            pthread_create(&engine->t, 0, runEngineBakground, NULL);
            
            //LM_T( LmtEngine , ("Running paralel thread to check engine"));
            //pthread_create(&engine->t_check, NULL, runEngineCheck, NULL);
        }	
	}
    
    void Engine::destroy()
    {
        LM_M(("Destroying engine"));
        
        {
            // Mutex protection
            au::TokenTaker tt(&engine_token);
            
            if ( !engine )
                return;
            
            // Mark as quit
            engine->flag_quit = true;
        }
        
        while( true )
        {
            
            {
                // Mutex protection
                au::TokenTaker tt(&engine_token);
                
                if( !engine->flag_running )
                {
                    // Destoy engine element
                    delete engine;
                    
                    // Put the global variable to NULL to avoid more adds...
                    engine = NULL;
                    
                    return;
                }
                
                LM_M(("Waiting engine main thread to finish"));
                sleep(1);
            }
        }
    }
    
    // ------------------------------------------------------------------------------------------------------------------------------
    
	bool Engine::check(  )
	{
        time_t time_in_seconds = cronometer.diffTimeInSeconds();
        
        if( running_element && ( time_in_seconds > ENGINE_MAX_RUNNING_TIME  ) )
        {
            LM_W(("Excesive time (%d secs, max %d secs) for engine Element '%s'." ,  
                  time_in_seconds , ENGINE_MAX_RUNNING_TIME, running_element->getDescription().c_str() ));
            return false;
        }
        
        return true;
	}
    
    std::string Engine::str()
    {
        // Mutex protection
        au::TokenTaker tt(&engine_token);
        
        if( engine )
            return engine->_str();
        else
            return "Engine is not initialized";
        
    }
    
    void Engine::getNextElement( )
    {

        // Mutex protection
        au::TokenTaker tt(&engine_token , "Engine::getNextElement");
        
        // Defauly values
        running_element = NULL;
        sleeping_time_seconds = 0;
        
        while( true )
        {
            
            if( elements.size() == 0)
            {
                // No more things to do
                LM_T( LmtEngine, ("SamsonEngine: No more elements to process in the engine. Quitting ..."));
                flag_quit = true;       // Flag this engine as to be finished
                return; // Return with default values
            }
            
            // Keep a total counter of loops
            counter++;  
            
            time_t now = time(NULL);
            
            if( now >= elements.front()->getThriggerTime() )
            {
                running_element = elements.front();
                elements.pop_front();
                
                
                return;
            }
            else
            {
                
                time_t trigger_time = elements.front()->getThriggerTime();
                
                if( trigger_time > now )
                    sleeping_time_seconds =  trigger_time - now;
                else
                    LM_X(1,("Time triggered is not greater than now..."));
                
                // LM_M(("Sleeping time in seconds %d" , _sleeping_seconds ));
                
                struct timeval tv;
                struct timespec ts;
                gettimeofday(&tv, NULL);
                ts.tv_sec = tv.tv_sec + sleeping_time_seconds;
                ts.tv_nsec = 0;
                
                // Sleep a bit
                if( sleeping_time_seconds > 0 )
                {
                    tt.stop(sleeping_time_seconds);
                }
                else
                    LM_W(("Error in sleeping time at Engine"));
                
            }
        }
    }
    
	void Engine::run()
	{
		
		LM_T( LmtEngine , ("Engine run"));

		flag_running =  true;   // Make sure we activate this flag ( is also initialized as true )
		counter = 0;            // Init the counter to elements managed by this run-time
        
		while( true )
		{
            // Get next element or sleep time
            getNextElement( );
            
            if( flag_quit )
            {
                flag_running = false;	// Flag to indicate that the engine is not running any more
                return;                 // If flagged as quit, just return to finish this main thread
            }

            // Run or sleep
            
            if ( !running_element )
                LM_X(1,("Internal errro at Engine"));
            
            
            {
                // Execute the item selectd as running_element
                
                LM_T( LmtEngine, ("[START] Engine executing %s" , running_element->getDescription().c_str()));
                
                // Reset cronometer for this particular task
                cronometer.reset();
                
                running_element->run();
                
                // Compute the time spent in this element
                time_t t = cronometer.diffTimeInSeconds();
                if( t > 60 )
                    LM_W(("Task %s spent %d seconds. This should not be more than 60", running_element->getDescription().c_str() , (int)t ));
                
                LM_T( LmtEngine, ("[DONE] Engine executing %s" , running_element->getDescription().c_str()));
                
                EngineElement * _running_element = running_element;
                
                running_element = NULL; // Put running element to NULL
                
                if( _running_element->isRepeated() )
                {
                    // Insert again
                    _running_element->Reschedule();
                    add( _running_element );
                }
                else
                    delete _running_element;
                
            }
            
                
            
		}
		
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
		
        
        std::ostringstream engine_state;
        
        engine_state << "Engine Loops: " << counter << "\n";
        
        if( running_element )
            engine_state << "\t\t\tCurrent:\n\t\t\t\t" << running_element->getDescription() << "\n";
        else
            engine_state << "( Sleeping for " << sleeping_time_seconds << ")\n";
        
        engine_state <<  "\t\t\tQueue: " << elements.size() << "\n";
        for ( au::list<EngineElement>::iterator el = elements.begin() ; el != elements.end() ; el++)
            engine_state << "\t\t\t\t[" << (*el)->getShortDescription() <<"]\n";
                
        return engine_state.str();
        
        
	}	
    
    // Functions to register objects ( general and for a particular notification )
    void Engine::register_object( Object* object )
    {
        // Mutex protection
        au::TokenTaker tt(&engine_token , "Engine::register_object" );
        
        if (engine)
            engine->objectsManager.add( object );
        else
            LM_W(("Not adding an Object since Engine is not initialized."));
        
    }
    
    void Engine::register_object_for_channel( Object* object, const char* channel )
    {
        // Mutex protection
        au::TokenTaker tt(&engine_token);
        
        if (engine)
            engine->objectsManager.add( object , channel );
        else
            LM_W(("Not adding an Object for channel %s since Engine is not initialized." , channel ));
    }

    
    // Generic method to unregister an object
    void Engine::unregister_object( Object* object )
    {
        // Mutex protection
        au::TokenTaker tt(&engine_token);
        
        if (engine)
            engine->objectsManager.remove( object );
        else
            LM_W(("Not adding an Object since Engine is not initialized."));
    }

    
    // Run a particular notification
    void Engine::send( Notification * notification )
    {
        
        if (!engine)
            LM_X(1,("Internal error at Engine"));
        
        engine->objectsManager.send( notification );
    }

    
    // Add a notification
    void Engine::notify( Notification*  notification )
    {
        // Push a notification element with the notification
        add( new NotificationElement( notification ) );
    }

    void Engine::notify( Notification*  notification , int seconds )
    {
        // Push a notification element with the notification ( in this case with periodic time )
        add( new NotificationElement( notification , seconds ) );
    }
    
    // Function to add a simple foreground tasks 
    void Engine::add( EngineElement *element )
    {
        // Mutex protection
        au::TokenTaker tt(&engine_token);

        if( !engine )
        {
            LM_W(("Not adding EngineElement since it is not activated" ));
            delete element;
            return;
        }
        
        if( engine->flag_quit )
        {
            LM_W(("Not adding EngineElement since it is flagged as \"quit\"" ));
            delete element;
            return;
        }
        
        engine->elements.insert( engine->_find_pos( element ) ,  element );

        // Wake up all the process from lock
        tt.wakeUp();
        
    }
    
    
    
    
}
