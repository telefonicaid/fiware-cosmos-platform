


#include <time.h>
#include <sys/time.h>
#include <signal.h>   // SIGKILL

#include "logMsg/logMsg.h"                          // LM_X

#include "au/string.h"                             // au::xml_...
#include "au/TokenTaker.h"                          // au::TokenTake
#include "au/ErrorManager.h"                       // au::ErrorManager
#include "au/Token.h"                       // au::Token
#include "au/xml.h"         // au::xml...
#include "au/ThreadManager.h"


#include "Notification.h"                           // engine::Notification

#include "engine/EngineElement.h"					// engine::EngineElement
#include "engine/ProcessItem.h"                     // engine::ProcessItem
#include "engine/DiskOperation.h"					// engine::DiskOperation
#include "engine/NotificationElement.h"       // engine::EngineNotificationElement

#include "engine/MemoryManager.h"
#include "engine/DiskManager.h"
#include "engine/ProcessManager.h"

#include "engine/Engine.h"							// Own interface

// Goyo. From 60 to 60000
#define ENGINE_MAX_RUNNING_TIME     60000

NAMESPACE_BEGIN(engine)

//Initialise singleton instance pointer to NULL
Engine *Engine::engine = NULL;

void* runEngineBackground(void* e)
{    
    e = NULL;

    Engine::shared()->running_thread = true;
    Engine::shared()->run();
    Engine::shared()->running_thread = false;
    
    return e;
}

Engine::Engine() : token("EngineToken")
{
    running_element = NULL;         // Initially, no running element
    quitting = false;               // Put this to "true" when destroying Engine
    running_thread = false;         // No running thread at the moment
}

Engine::~Engine()
{
    // Remove pending elements in Engine
    repeated_elements.clearList();
    normal_elements.clearList();
}

Engine* Engine::shared()
{
    if (!engine)
    {
	   LM_E(("Engine was not initialised. Calling Engine::init()"));
	   init();
	}
    return engine;
}

void Engine::init()
{
    LM_V(("Engine init"));
    
    if ( engine )
    {
        LM_W(("Init engine twice... just ignoring"));
        return;
    }
    
    // Create the unique engine entity
    engine = new Engine();
    
    // Add a simple periodic element to not die inmediatelly
    EngineElement *element = new NotificationElement( new Notification("alive") , 10 );
    engine->repeated_elements.push_back( element );
    
    LM_T( LmtEngine , ("Running engine in background...") );
    au::ThreadManager::shared()->addThread("Engine", &engine->t, 0, runEngineBackground, NULL );
}

void Engine::destroy()
{
    LM_V(("Engine destroy"));
    
    if (!engine)
    {
        LM_W(("Not possible to destroy Engine since it was not initialized"));
        return;
    }

    // Set this falg as true
    engine->quitting = true;
    
    // Wait for the main thread
    au::CronometerNotifier cronometer_notifier(2); // Notify every 2 seconds on screen that we are waiting...
    while( engine->running_thread )
    {
        usleep(100000);
        if( cronometer_notifier.touch() )
            LM_M(("Waiting engine main thread to finish"));
    }
    
    delete engine;
    engine = NULL;
}

void sleep_select( double time )
{
	fd_set          fds;
	struct timeval  timeVal;

    timeVal.tv_sec  = 0;
    timeVal.tv_usec = 0;
        
    if( time >= 1.0 )
    {
        timeVal.tv_sec = (long) fabs(time);
        time -= timeVal.tv_sec;
    }
    
    timeVal.tv_usec = time*1000000;
    
    
    
    FD_ZERO(&fds);        
    int r = select( 1 , NULL, &fds, NULL, &timeVal);
    
    if( r != 0 )
        LM_W(("Select returned %d in sleep_select [ timeval %l %l ]",r  , timeVal.tv_sec , timeVal.tv_usec ));
    
}

void Engine::run()
{
    
    LM_T( LmtEngine , ("Engine run"));
    
    counter = 0;            // Init the counter to elements managed by this run-time
    
    au::Cronometer engine_cronometer;
    
    
    while( true )
    {
        
        if( engine_cronometer.diffTime() > 5.0 )
        {
            //LM_M(("Engine alive..."));
            engine_cronometer.reset();
        }
        
        // Keep a total counter of loops
        counter++;  
        
        // Finish this thread if necessary
        if( quitting )
            return; 
        
        // Check if there are elements in the list
        bool isEmpty;
        {
            // Mutex protection
            au::TokenTaker tt(&token , "Engine::getNextElement");
            isEmpty = ( ( repeated_elements.size() == 0 ) && ( normal_elements.size() == 0) );
        }
        
        if( isEmpty )
        {
            LM_T( LmtEngine, ("SamsonEngine: No more elements to process in the engine. Quitting ..."));
            return;
        }
        
        // ------------------------------------------------------------------------------------
        // Try to get the next element in the repeat_elements list , if not , normal elements
        // ------------------------------------------------------------------------------------
        double t_sleep = 0.1; // Default sleep time if no periodic elements
        
        {
            // Mutex protection to access elements
            au::TokenTaker tt(&token , "Engine::getNextElement");
            
            if( normal_elements.size() > 30 )
                LM_W(("Execesive number of elements in the engine sack %d" , (int) normal_elements.size() ));
            
            if( repeated_elements.size() > 0 )
            {
                t_sleep = repeated_elements.front()->getTimeToTrigger();
            
                // Ready to be executed
                if( t_sleep < 0.01 )
                {
                    running_element = repeated_elements.front();
                    repeated_elements.pop_front();
                }
            }
            
            if( !running_element && (normal_elements.size() > 0 ) )
            {
                running_element = normal_elements.front();
                normal_elements.pop_front();
            }
            
        }

        /*
        if( running_element )
            LM_W(("Engine status next: %s normal: %lu periodid: %lu " , running_element->getDescription().c_str() , normal_elements.size() , repeated_elements.size() ));
        else
            LM_W(("Engine status sleep time: %f normal: %lu periodid: %lu " , t_sleep , normal_elements.size() , repeated_elements.size() ));
        */
         
        if( !running_element )
        {
            if ( t_sleep > 0.1 )
                t_sleep = 0.1; // Max time to sleep to avoid locks...
                         // Sleeping a bit for the next engine element...
            LM_T( LmtEngine, ("Engine: Sleeping %.2f secs ..." , t));
            {
                au::ExecesiveTimeAlarm alarm( "Engine sleep2" , 2*t_sleep );
                sleep_select( t_sleep );
            }
            
            continue; // Loop again to check again...
        }
        
        
        {
            // Execute the item selected as running_element
            LM_T( LmtEngine, ("[START] Engine executing %s" , running_element->getDescription().c_str() ));
            
            int waiting_time = running_element->getWaitingTime();
            if ( waiting_time > 10 )
            {
                LM_W(("Engine is running an element has been waiting %d seconds", waiting_time ));
                //LM_W(("Engine element to execute now: %s" , running_element->getDescription().c_str() ));
            }
            if ( waiting_time > 100 )
            {
                // Print entire engine items...
                
                au::list<EngineElement>::iterator it_elements;
                for( it_elements = repeated_elements.begin() ; it_elements != repeated_elements.end() ; it_elements++ )
                {
                    EngineElement* element = *it_elements;
                    LM_M(("ENGINE REPEATED ELEMENT: %s",element->getDescription().c_str()));
                }

                for( it_elements = normal_elements.begin() ; it_elements != normal_elements.end() ; it_elements++ )
                {
                    EngineElement* element = *it_elements;
                    LM_M(("ENGINE NORMAL ELEMENT: %s",element->getDescription().c_str()));
                }
                
                LM_X(1,("Engine is running an element has been waiting %d seconds", waiting_time ));
            }
            
            {
                // Run the item controlling excesive time
                au::Cronometer c;
                
                // Run the running element ;)
                running_element->run();
                
                int execution_time = c.diffTime();
                if ( execution_time > 10 )
                {
                    LM_W(("Engine has executed an item in %d seconds." , execution_time ));
                    LM_W(("Engine Executed item: %s" ,  running_element->getDescription().c_str()  ));
                }
            }
            
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


std::list<EngineElement*>::iterator Engine::_find_pos_in_repeated_elements( EngineElement *e)
{
    for (std::list<EngineElement*>::iterator i = repeated_elements.begin() ; i != repeated_elements.end() ; i++)
    {
        if ( (*i)->getTimeToTrigger() > e->getTimeToTrigger() )
            return i;
    }
    
    return repeated_elements.end();
}

#pragma mark ----

// get xml information
void Engine::getInfo( std::ostringstream& output)
{
    // Mutex protection
    au::TokenTaker tt(&token , "Engine::str");
    
    au::xml_open(output, "engine");
    
    au::xml_simple(output , "loops" , counter );
    
    if( running_element )
        au::xml_simple( output , "running_element" , running_element->getDescription() );
    else
        au::xml_simple( output , "running_element" , "No running element" );
    
    
    au::xml_simple( output , "uptime" , uptime.diffTimeInSeconds() );
    
    au::xml_close(output , "engine");
    
}


// Functions to register objects ( general and for a particular notification )
void Engine::register_object( Object* object )
{
    // Mutex protection
    au::TokenTaker tt(&token , "Engine::register_object" );
    
    objectsManager.add( object );
}

void Engine::register_object_for_channel( Object* object, const char* channel )
{
    // Mutex protection
    au::TokenTaker tt(&token,"Engine::register_object_for_channel");
    
    objectsManager.add( object , channel );
}

// Generic method to unregister an object
void Engine::unregister_object( Object* object )
{
    // Mutex protection
    au::TokenTaker tt(&token , "Engine::unregister_object");
    
    objectsManager.remove( object );
}


// Run a particular notification
// Only executed from friend class "NotificationElement"
void Engine::send( Notification * notification )
{
    objectsManager.send( notification );
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
    au::TokenTaker tt(&token);

    // Insert an element in the engine
    
    if( element->isRepeated() )
        repeated_elements.insert( _find_pos_in_repeated_elements( element ) ,  element );
    else
        normal_elements.push_back( element );
        
    // Wake up main thread if sleeping
    tt.wakeUp();
}

// Get an object by its registry names
Object* Engine::getObjectByName( const char *name )
{
    // Mutex protection
    au::TokenTaker tt(&token);
    
    return objectsManager.getObjectByName(name);
    
}

NAMESPACE_END
