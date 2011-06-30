

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
		
		// Foreground processes
		// --------------------------------------------------------------------
		
		au::list<EngineElement> elements;               // Elements of the samson engine
		EngineElement *running_element;                 // Element that is currently running
		int sleeping_time_seconds;                          // Time of sleep in the last call
		
		// Engine Status Flags 
		// ---------------------------------------------------------------------------
		bool flag_quit;									// Flag to indicate that a quit has been called, so no more tasks can be added
		bool flag_running;								// Flag to indicate that the main-thread for the Engine is working

		pthread_t t;                                    // Thread to run the engine in background ( if necessary )
        pthread_t t_check;                              // Secondary thread to check that an "element" in the Engine take execessive time
		
        size_t counter;                                 // Counter of EngineElement processed

        au::Cronometer cronometer;                      // Cronometer to count the time spend by the current element ( maximum 60 seconds )
        
        friend class NotificationElement;
                
        ObjectsManager objectsManager;                  // Management of objects ( notification )
        
		Engine();
        
	public:
		
		~Engine();
		
		/*
		 Singleton management
		 */
		static void init();
		static void destroy();

    private:
        
        friend class NotificationListener;
        
    public:
        
        // Debug string
        static std::string str();

        
		// Methods only executed from the thread-creation-functions ( never use directly )
        void run();
        bool check(  );
        
	private:

		// Find the position in the list to inser a new element
		std::list<EngineElement*>::iterator _find_pos( EngineElement *e);
        
        // Internal function to get the next element to be processed or the sleep time until the next one.
        // Return NULL & 0 if no more element to process
        void getNextElement( );
        
        /*
         Debug string
         */
        std::string _str();
        

    public:

        // Functions to register objects ( general and for a particular notification )
        static void register_object( Object* object );
        static void register_object_for_channel( Object* object, const char* channel );
        
        // Generic method to unregister an object
        static void unregister_object( Object* object );
        
        // Add a notification
        static void notify( Notification*  notification );
        static void notify( Notification*  notification , int seconds ); // Repeated notification
        
        // Function to add a simple foreground tasks 
		static void add( EngineElement *element );	

    private:
        
        // Run a particular notification
        static void send( Notification * notification );
        
    };
    
    
	
};

#endif
