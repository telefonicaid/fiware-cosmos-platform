


#include <time.h>
#include <sys/time.h>
#include <signal.h>   // SIGKILL

#include "logMsg/logMsg.h"                          // LM_X

#include "au/string.h"                             // au::xml_...
#include "au/TokenTaker.h"                          // au::TokenTake
#include "au/ErrorManager.h"                       // au::ErrorManager
#include "au/Token.h"                       // au::Token
#include "au/xml.h"         // au::xml...


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

// Common engine
Engine *engine = NULL;

void* runEngineBakground(void* e )
{
    engine->run();
    return NULL;
}

Engine::Engine() :  EngineService("Engine") , token("EngineToken")
{
    running_element = NULL;                         // Initially, no running element
    flag_finish_threads = false;                    // Put this to "true" when destroying Engine
}

Engine::~Engine()
{
    // Waiting for all threads to finish
    engine->finish_threads();
    
    // Remove pending elements in Engine
    elements.clearList();
}

Engine* Engine::shared()
{
    if (!engine )
        LM_X(1,("Please init Engine with Engine::init()"));
    return engine;
}


void Engine::init()
{
    if ( engine )
    {
        LM_W(("Init engine twice... just ignoring"));
        return;
    }
    
    // Create the unique engine entity
    engine = new Engine();
    
    // Add a simple periodic element to not die inmediatelly
    EngineElement *element = new NotificationElement( new Notification("alive") , 10 );
    engine->elements.push_back( element );
    
    LM_T( LmtEngine , ("Running engine in background...") );
    pthread_create(&engine->t, 0, runEngineBakground, NULL);
    
    // Function to remove this engine and all EngineServices like MemoryMamager, DiskManager, ProcessManager etc...
    atexit( destroy_engine_services );
}

void Engine::finish_threads()
{
    flag_finish_threads = true;
    LM_M(("Waiting threads of Engine to finish..."));
    pthread_join(t, NULL);
    LM_M(("All Engine threads finished"));
}





void Engine::run()
{
    
    LM_T( LmtEngine , ("Engine run"));
    
    counter = 0;            // Init the counter to elements managed by this run-time
    
    while( true )
    {
        // Keep a total counter of loops
        counter++;  
        
        // Finish this thread if necessary
        if( flag_finish_threads )
            return; 
        
        // Check if there are elements in the list
        bool isEmpty;
        {
            // Mutex protection
            au::TokenTaker tt(&token , "Engine::getNextElement");
            isEmpty = ( elements.size() == 0);
        }
        
        if( isEmpty )
        {
            LM_T( LmtEngine, ("SamsonEngine: No more elements to process in the engine. Quitting ..."));
            return;
        }
        
        // Get time for the next engine element
        double t;
        {
            // Mutex protection to access elements
            au::TokenTaker tt(&token , "Engine::getNextElement");
            t = elements.front()->getTimeToTrigger();
        }
        
        if( t > 0 )
        {
            if ( t > 0.1 )
                t = 0.1; // Max time to sleep to avoid locks...
            
            // Sleeping a bit for the next engine element...
            LM_T( LmtEngine, ("Engine: Sleeping %.2f secs ..." , 2*t));
            
            {
                // Mutex protection to show list
                au::TokenTaker tt(&token , "Engine::getNextElement");
                LM_T( LmtEngine, ("--------------------------------------------------------" ));
                au::list<EngineElement>::iterator it_elements;
                for (it_elements = elements.begin() ; it_elements != elements.end() ; it_elements++ )
                {
                    EngineElement *element = *it_elements;
                    LM_T( LmtEngine, ("%s" , element->getShortDescription().c_str() ));    
                }
                LM_T( LmtEngine, ("--------------------------------------------------------" ));
            }
            
            {
                au::ExecesiveTimeAlarm alarm( "Engine sleep" , t );
                int microsecons = t*1000000;
                usleep( microsecons );
            }
            continue; // Loop again to check again...
        }
        
        // Get the top element to be executed
        {
            // Mutex protection to access elements
            au::TokenTaker tt(&token , "Engine::getNextElement");
            
            running_element = elements.front();
            elements.pop_front();
        }
        
        // Run or sleep
        if ( !running_element )
            LM_X(1,("Internal error at Engine"));
        
        {
            // Execute the item selected as running_element
            LM_T( LmtEngine, ("[START] Engine executing %s" , running_element->getDescription().c_str() ));
            
            int excesive_time = - running_element->getTimeToTrigger();
            if ( excesive_time > 10 )
            {
                LM_W(("Task %s delayed %d seconds in starting. This should not be more than 10"
                      , running_element->getDescription().c_str() 
                      , excesive_time 
                      ));
            }
            
            {
                // Run the item controlling excesive time
                au::ExecesiveTimeAlarm alarm( au::str("ProcessItem run '%s'",running_element->getDescription().c_str() ));
                running_element->run();
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


std::list<EngineElement*>::iterator Engine::_find_pos( EngineElement *e)
{
    for (std::list<EngineElement*>::iterator i = elements.begin() ; i != elements.end() ; i++)
    {
        if ( (*i)->getTimeToTrigger() > e->getTimeToTrigger() )
            return i;
    }
    
    return elements.end();
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
    
    au::xml_iterate_list( output , "elements" , elements );
    
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
    elements.insert( _find_pos( element ) ,  element );
    
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