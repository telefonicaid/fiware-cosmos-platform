

#ifndef _H_SAMSON_ENGINE
#define _H_SAMSON_ENGINE

#include <pthread.h>
#include <list>
#include <string>
#include <iostream>                         // std::cout
#include <set>                              // std::set

#include "au/list.h"                        // au::list
#include "au/Cronometer.h"                  // au::Cronometer


#include "engine/Object.h"      // engine::EngineNotification
#include "engine/ObjectsManager.h"          // engine::ObjectsManager

namespace au {
    class Error;
    class Token;
}

namespace engine
{

	class EngineElement;
	class ProcessItem;
	class DiskOperation;
	class Notification;
 
	/**
	 Main engine platform
	 */
	
	class Engine
	{
		
        friend class NotificationElement;
        friend class NotificationListener;
        
		// List of engine elements
		// --------------------------------------------------------------------
		
		au::list<EngineElement> elements;               // Elements of the samson engine
        
		EngineElement *running_element;                 // Element that is currently running
		int sleeping_time_seconds;                      // Time of sleep in the last call
		
        au::Token *token;                               // General mutex to protect global variable engine and block the main thread if necessary
        
		pthread_t t;                                    // Thread to run the engine in background ( if necessary )
        pthread_t t_check;                              // Secondary thread to check that an "element" in the Engine take execessive time
		
        size_t counter;                                 // Counter of EngineElement processed

        au::Cronometer cronometer;                      // Cronometer to count the time spend by the current element ( maximum 60 seconds )

        ObjectsManager objectsManager;                  // Management of objects ( notification )
        
		Engine();

	public:
		
		~Engine();
        
        static void init();
        static Engine* shared();
        
    public:
        
		// Methods only executed from the thread-creation-functions ( never use directly )
        void run();
        void check();
        
	private:

		// Find the position in the list to inser a new element
		std::list<EngineElement*>::iterator _find_pos( EngineElement *e);
        
        // Internal function to get the next element to be processed or the sleep time until the next one.
        void getNextElement( );

    public:
        
        // Information string for debugging
        std::string str();

   private:

        std::string _str();

    public:

        // Functions to register objects ( general and for a particular notification )
        void register_object( Object* object );
        void register_object_for_channel( Object* object, const char* channel );
        
        // Generic method to unregister an object
        void unregister_object( Object* object );
        
        // Add a notification
        void notify( Notification*  notification );
        void notify( Notification*  notification , int seconds ); // Repeated notification
        
        // Function to add a simple foreground tasks 
		void add( EngineElement *element );	

    private:
        
        // Run a particular notification
        // Only executed from friend class "NotificationElement"
        void send( Notification * notification );
        
    };
    
    
	
};

#endif
