
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

#include "engine/EngineService.h"
#include "engine/Object.h"                  // engine::EngineNotification
#include "engine/ObjectsManager.h"          // engine::ObjectsManager

NAMESPACE_BEGIN(engine)
class Error;
class Token;
NAMESPACE_END

NAMESPACE_BEGIN(engine)

class EngineElement;
class ProcessItem;
class DiskOperation;
class Notification;

/**
 Main engine platform
 */

class Engine
{
    // Common engine instance
    static Engine* engine;

    // Elements of the samson engine to be repeated periodically
    au::list<EngineElement> repeated_elements;           
    
    // General fifo elements of the SAMSON Engine
    au::list<EngineElement> normal_elements;               
    
    // Element that is currently running
    EngineElement *running_element;                      
    
    // General mutex to protect global variable engine and block the main thread if necessary
    au::Token token;                                
    
    pthread_t t;                                    // Thread to run the engine in background ( if necessary )
    
public:
    
    bool quitting;                                  // Flag used to indicate to threads that engine will finish
    bool running_thread;                            // Flag to indicate that background thread is running

private:    
    
    size_t counter;                                 // Counter of EngineElement processed
    
    ObjectsManager objectsManager;                  // Management of objects ( notification )
    
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
    

private:
    
    // Find the position in the list to inser a new element
    std::list<EngineElement*>::iterator _find_pos_in_repeated_elements( EngineElement *e);
    
    
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
    
    
};

NAMESPACE_END

#endif
