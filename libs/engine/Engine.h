

#ifndef _H_SAMSON_ENGINE
#define _H_SAMSON_ENGINE

#include <pthread.h>
#include <list>
#include <string>
#include <iostream>                 // std::cout
#include <set>                      // std::set
#include "au/list.h"                // au::list
#include "au/Cronometer.h"                  // au::Cronometer
#include "engine/EngineNotification.h"      // engine::EngineNotification

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
		int _sleeping_seconds;                          // Time of sleep in the last call
		
		// Engine Status Flags 
		// ---------------------------------------------------------------------------
		bool flag_quit;									// Flag to indicate that a quit has been called, so no more tasks can be added
		bool flag_running;								// Flag to indicate that the main-thread for the Engine is working
		
		// Thread stuff
		// ---------------------------------------------------------------------------
		pthread_mutex_t elements_mutex;                 // Mutex to protect elements
		pthread_cond_t elements_cond;                   // Conditional to block the thread while waiting the next event

		pthread_t t;                                    // Thread to run the engine in background ( if necessary )
        pthread_t t_check;                              // Secondary thread to check that an "element" in the Engine take execessive time
		
        size_t counter;                                 // Counter

        au::Cronometer cronometer;                      // Cronometer to count the time spend by the current element ( maximum 60 seconds )
        
        friend class NotificationElement;
                
        EngineNotificationSystem notificationSystem;    // Notification system
		Engine();
        
	public:
		
		~Engine();
		
		/*
		 Singleton management
		 */
		static void init();

		/*
		 Main run methonds
		 */
		
		static void run();
		static void runInBackground();
		
        // Function to add a simple foreground tasks 
		static void add( EngineElement *element );	

        // Methods to add notifications
        static void notify( Notification*  notification );
        static void notify( Notification*  notification , int seconds ); // Repeated notification

        // Add and remove listeners
        static void add( std::string name , NotificationListener*listener);
        static void remove( NotificationListener* listener );

        // Debug string
        static std::string str();
        
	private:

		// Find the position in the list to inser a new element
		std::list<EngineElement*>::iterator _find_pos( EngineElement *e);

		// Internal run function
        void _run();
      
		// Function to add an element in the system ( inside or not )
		
        void _add( EngineElement *element );	// Function to add a simple foreground tasks 
        

        // real call to the notification ( only from NotificationElement )
        static void _notify( Notification *notification );
                     
        
        /*
         Debug string
         */
        std::string _str();
        

    public:
        
        // Only used internally
        void _quit();

        // Used internally for debugging
        void _check();
        
	};
	
};

#endif
