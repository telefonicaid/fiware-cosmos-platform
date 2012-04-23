
/* ****************************************************************************
 *
 * FILE            Engine.h
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * Engine: Singlenton object with all the main funcitonalities of the engine library
 *
 * ****************************************************************************/

#ifndef _H_SAMSON_ENGINE
#define _H_SAMSON_ENGINE

#include <pthread.h>
#include <list>
#include <string>
#include <iostream>                         // std::cout
#include <set>                              // std::set

#include "au/containers/list.h"                        // au::list
#include "au/mutex/Token.h"                       // au::Token
#include "au/Cronometer.h"                  // au::Cronometer
#include "au/namespace.h"                // NAMESPACE_BEGIN & NAMESPACE_END
#include "au/containers/vector.h"
#include "au/mutex/TokenTaker.h"
#include "au/mutex/Token.h"

#include "engine/Object.h"                  // engine::EngineNotification
#include "engine/ObjectsManager.h"          // engine::ObjectsManager
#include "engine/EngineElementCollection.h"

NAMESPACE_BEGIN(au)
class Error;
class Token;
NAMESPACE_END

NAMESPACE_BEGIN(engine)

class EngineElement;
class ProcessItem;
class DiskOperation;
class Notification;

// ---------------------------------------------------
//
// EngineElementCollection
//
// Main Engine platform
// ---------------------------------------------------


class Engine
{
    // Common engine instance
    static Engine* engine;


    // Collection of items
    EngineElementCollection engine_element_collection;
    
    // Management of objects ( notification )
    au::Token token_objectsManager;                                
    ObjectsManager objectsManager;                  

    // Counter of EngineElement processed
    size_t counter;                                 

    // Thread to run the engine in background ( if necessary )
    pthread_t t;                                    
    
public:
    
    bool quitting;                                  // Flag used to indicate to threads that engine will finish
    bool running_thread;                            // Flag to indicate that background thread is running

private:    
    
    Engine();
    
public:
    
    au::Cronometer uptime;                          // Total up time
    
    ~Engine();
    
    static void destroy();
    static void init();
    static void stop();
    static Engine* shared();
    
public:
    
    // Methods only executed from the thread-creation-functions ( never use directly )
    void run();
        
    
public:
    
    // get xml information
    void getInfo( std::ostringstream& output);
    
private:
    
    friend class Object;
    
    // Functions to register objects ( general and for a particular notification )
    void register_object( Object* object );
    void register_object_for_channel( Object* object, const char* channel );
    
    // Generic method to unregister an object
    void unregister_object( Object* object );
    
public:
    
    // Add a notification
    void notify( Notification*  notification );
    void notify_extra( Notification*  notification );
    void notify( Notification*  notification , int seconds ); // Repeated notification
    
    // Function to add a simple foreground tasks 
    void add( EngineElement *element );	
    
    // Get an object by its registry names
    Object* getObjectByName( const char *name );
    
    // Info functions 
    int getNumElementsInEngineStack();
    double getMaxWaitingTimeInEngineStack();
    
private:
    
    friend class NotificationElement;
    
    // Run a particular notification
    // Only executed from friend class "NotificationElement"
    void send( Notification * notification );
    
    // Run a particular engine element
    void runElement( EngineElement* running_element );


    
};

NAMESPACE_END

#endif
