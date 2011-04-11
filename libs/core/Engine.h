

#ifndef _H_SAMSON_ENGINE
#define _H_SAMSON_ENGINE

#include <pthread.h>
#include <list>
#include <string>
#include <iostream>                 // std::cout
#include "au_map.h"                 // au::list
#include "MemoryManager.h"          // ss::MemoryManager
#include "DiskStatistics.h"         // ss::DiskStatistics
#include <set>                      // std::set
#include "EngineNotification.h"     // ss::EngineNotification
#include "ProcessManager.h"         // ss::ProcessManager
#include "DiskManager.h"            // ss::DiskManager

namespace ss
{

	class EngineElement;
	class ProcessItem;
	class DiskOperation;
	class EngineNotification;
 
	/**
	 Main engine platform
	 */
	
	class Engine
	{
		
		// Foreground processes
		// --------------------------------------------------------------------
		
		au::list<EngineElement> elements;			// Elements of the samson engine
		EngineElement *running_element;				// Element that is currently running
		int _sleeping_seconds;                       // Time of sleep in the last call
		
		// Engine Status Flags 
		// ---------------------------------------------------------------------------
		bool _quit;									// Flag to indicate that a quit has been called, so no more tasks can be added
		bool _running;								// Flag to indicate that the main-thread for the Engine is working
		
		// Thread stuff
		// ---------------------------------------------------------------------------
		pthread_mutex_t elements_mutex;				// Mutex to protect elements
		pthread_cond_t elements_cond;				// Conditional to block the thread while waiting the next event
		pthread_t t;								// Thread to run the engine in background ( if necessary )
		
        size_t counter;                             // Counter
        
	public:
		
        EngineNotificationSystem notificationSystem;    // Notification system
        
		MemoryManager memoryManager;                    // Memory manager		
        ProcessManager processManager;                  // Process manager for background jobs
        DiskManager diskManager;                        // Disk manager
        
	public:
		
		Engine();
		~Engine();
		
		/*
		 Singleton management
		 */
		static void init();
		static Engine *shared();

		/*
		 Main run methonds
		 */
		
		void run();
		void runInBackground();
        void quit();
		
		/*
		 Methods to add and remove foregrond and background processes
		 */
		
		// Function to add an element in the system ( inside or not )
		void add( EngineElement *element );					// Function to add a ForeGroundProcess 

        /*
         Method to add a notification ( a particular EngineElement )
         */
        
        void notify( EngineNotification*  notification );
        void notify( EngineNotification*  notification , int seconds ); // Repeated notification
        
		/*
		 Methods to get information about the state
		 */
		
		// Function to fill part of the message sent to the controller ( informing about status )
		void fill(network::WorkerStatus*  ws);				

	private:

		// Find the position in the list to inser a new element
		std::list<EngineElement*>::iterator _find_pos( EngineElement *e);
		
	};
	
};

#endif