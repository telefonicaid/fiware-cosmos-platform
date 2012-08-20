


#include <time.h>
#include <sys/time.h>
#include <signal.h>   // SIGKILL

#include "logMsg/logMsg.h"                          // LM_X

#include "au/string.h"                             // au::xml_...
#include "au/mutex/TokenTaker.h"                          // au::TokenTake
#include "au/ErrorManager.h"                       // au::ErrorManager
#include "au/mutex/Token.h"                       // au::Token
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

namespace engine {


std::list<EngineElement*>::iterator EngineElementCollection::_find_pos_in_repeated_elements( EngineElement *e )
{
    for (std::list<EngineElement*>::iterator i = repeated_elements.begin() ; i != repeated_elements.end() ; i++)
    {
        if ( (*i)->getTimeToTrigger() > e->getTimeToTrigger() )
            return i;
    }
    
    return repeated_elements.end();
}




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

Engine::Engine()
{
    quitting = false;               // Put this to "true" when destroying Engine
    running_thread = false;         // No running thread at the moment
    last_uptime_mark = 0;           // Uptime mark used to count time for different activites
}

Engine::~Engine()
{
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
    LM_VV(("Engine init"));
    
    if ( engine )
    {
        LM_W(("Init engine twice... just ignoring"));
        return;
    }
    
    // Create the unique engine entity
    engine = new Engine();
    
    // Add a simple periodic element to not die inmediatelly
    EngineElement *element = new NotificationElement( new Notification("alive") , 10 );
    engine->engine_element_collection.add( element );
    
    LM_T( LmtEngine , ("Running engine in background...") );
    au::ThreadManager::shared()->addThread("Engine", &engine->t, 0, runEngineBackground, NULL );
}


void Engine::stop()
{
    // Set this flag as true to force main thread to finish
    if ( engine )
        engine->quitting = true;
}

void Engine::destroy()
{
    LM_V(("Engine destroy"));
    
    if (!engine)
    {
        LM_W(("Not possible to destroy Engine since it was not initialized"));
        return;
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

void Engine::runElement( EngineElement* running_element )
{
    activity_monitor.StartActivity( running_element->getName() );
    
    // Execute the item selected as running_element
    LM_T( LmtEngineTime, ("[START] Engine:  executing %s" , running_element->str().c_str() ));
    
    // Print traces for debugging strange situations
    int waiting_time = running_element->getWaitingTime();
    if ( waiting_time > 10 )
    {
        LM_W(("Engine is running an element that has been waiting %d seconds", waiting_time ));
        LM_W(("Engine element to execute now: %s" , running_element->str().c_str() ));
        
        if ( waiting_time > 100 )
        {
            // Print all elements with traces for debuggin...
            engine_element_collection.print_elements();
        }
    }
    
    
    {
        // Run the item controlling excesive time
        au::Cronometer c;
        
        // Run the running element ;)
        running_element->run();
        
        int execution_time = c.seconds();
        if ( execution_time > 10 )
        {
            LM_W(("Engine has executed an item in %d seconds." , execution_time ));
            LM_W(("Engine Executed item: %s" ,  running_element->str().c_str()  ));
        }
    }
    
    LM_T( LmtEngineTime, ("[DONE] Engine:  executing %s" , running_element->str().c_str()));
    
    // Collect information about this execution
    activity_monitor.StartActivity( "engine_management" );
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
        if( quitting )
            return; 
        
        // Check if there are elements in the list
        if ( engine_element_collection.isEmpty() )
        {
            LM_T( LmtEngine, ("SamsonEngine: No more elements to process in the engine. Quitting ..."));
            return;
        }

        // Warning if we have a lot of elements in the engine stack
        size_t num_engine_elements = engine_element_collection.getNumEngineElements();
        LM_T( LmtEngine, ("Number of elements in the engine stack %lu" , num_engine_elements ));
        
        if( num_engine_elements > 10000 )
            LM_W(("Execesive number of elements in the engine stack %lu" , num_engine_elements ));

        // ------------------------------------------------------------------------------------
        // Try to get the next element in the repeat_elements list 
        // if not there , try normal elements...
        // if not, run extra elements and loop again...
        // ------------------------------------------------------------------------------------
        

        // Try if next repeated element is ready to be executed
        EngineElement * element = engine_element_collection.getNextRepeatedEngineElement();
        if( element )
        {
            runElement(element);
            element->Reschedule(); // Reschedule this item
            add( element );
            continue;
        }
        
        // Try next normal item
        element = engine_element_collection.getNextNormalEngineElement();
        if( element )
        {
            runElement(element);
            delete element; // Remove this normal element
            continue;
        }
        
        // Get a vector with all extra elements to be executed
        std::vector<EngineElement*> extra_elements = engine_element_collection.getExtraElements();
        
        // Run all the elements
        for ( size_t i = 0 ; i < extra_elements.size() ; i++ )
        {
            EngineElement *element = extra_elements[i];
            runElement(element);
            element->Reschedule(); // reinit internal counters...
        }

        // If normal elements to be executed, do not sleep
        size_t num_normal_elements =  engine_element_collection.getNumNormalEngineElements();
        if( num_normal_elements > 0 )
        {
            LM_T( LmtEngine, ("Do not sleep since it seems there are %lu elements in the engine" , 
                              num_normal_elements));
            continue; // Do not sleep here
        }
        
        // If next repeated elements is close, do not sleep
        double t_next_repeated_elements = engine_element_collection.getTimeForNextRepeatedEngineElement();
        LM_T( LmtEngine, ("Engine: Next repeated item in %.2f secs ..." , t_next_repeated_elements));
        if( t_next_repeated_elements < 0.01 )
            continue;

        activity_monitor.StartActivity("sleep");

        sleep_select( 0.1 );

        activity_monitor.StartActivity( "engine_management" );

    }
}

int Engine::getNumElementsInEngineStack()
{
    return engine_element_collection.getNumEngineElements();
}

double Engine::getMaxWaitingTimeInEngineStack()
{
    return engine_element_collection.getMaxWaitingTimeInEngineStack();
}

#pragma mark ----

// get xml information
void Engine::getInfo( std::ostringstream& output)
{
    au::xml_open(output, "engine");
    
    au::xml_simple(output , "loops" , counter );
    
    au::xml_simple( output , "uptime" , uptime.seconds() );
    
    au::xml_close(output , "engine");
    
}


// Functions to register objects ( general and for a particular notification )
void Engine::register_object( Object* object )
{
    objectsManager.add( object );
}

void Engine::register_object_for_channel( Object* object, const char* channel )
{
    objectsManager.add( object , channel );
}

// Generic method to unregister an object
void Engine::unregister_object( Object* object )
{
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

void Engine::notify_extra( Notification*  notification )
{
    // Push a notification element with the notification
    NotificationElement* notification_element = new NotificationElement( notification );
    notification_element->set_as_extra();
    add( notification_element  );
}

void Engine::notify( Notification*  notification , int seconds )
{
    // Push a notification element with the notification ( in this case with periodic time )
    add( new NotificationElement( notification , seconds ) );
}

// Function to add a simple foreground tasks 
void Engine::add( EngineElement *element )
{
    // Add a new item in the engine_element_collection
    engine_element_collection.add( element );
}

// Get an object by its registry names
Object* Engine::objectByName( const char *name )
{
    return objectsManager.objectByName(name);
    
}


}
