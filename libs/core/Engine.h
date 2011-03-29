

#ifndef _H_SAMSON_ENGINE
#define _H_SAMSON_ENGINE

#include <pthread.h>
#include <list>
#include <string>
#include <iostream>                 // std::cout
#include "au_map.h"                 // au::list
#include "MemoryManager.h"          // ss::MemoryManager
#include "SharedMemoryManager.h"    // ss::SharedMemoryManager
#include "DiskStatistics.h"         // ss::DiskStatistics

namespace ss
{

	class EngineElement;
	class ProcessItem;
	class DiskOperation;
	
	/**
	 Main engine platform
	 */
	
	class Engine
	{
		
		// Foreground processes
		// --------------------------------------------------------------------
		
		au::list<EngineElement> elements;			// Elements of the samson engine
		EngineElement *running_element;				// Element that is currently running


		// Background processes
		// --------------------------------------------------------------------
		int num_processes;							// Number of maximum simultaneous process running ( from setup )
		std::set<ProcessItem*> items;				// List of items to be executed ( all priorities  )
		std::set<ProcessItem*> running_items;		// Set of items currently being executed
		std::set<ProcessItem*> halted_items;		// Set of items currently being executed but halted
		
		// Disk Operations
		int num_disk_operations;						// Number of paralell disk operations allowed
		au::list<DiskOperation> pending_operations;		// List of pending operations
		std::set<DiskOperation*> running_operations;	// Running operations
		DiskStatistics diskStatistics;
		
		// Engine Status Flags 
		// ---------------------------------------------------------------------------
		bool _quit;									// Flag to indicate that a quit has been called, so no more tasks can be added
		bool _running;								// Flag to indicate that the main-thread for the Engine is working
		
		// Thread stuff
		// ---------------------------------------------------------------------------
		pthread_mutex_t elements_mutex;				// Mutex to protect elements
		pthread_cond_t elements_cond;				// Conditional to block the thread while waiting the next event
		pthread_t t;								// Thread to run the engine in background ( if necessary )
		
		
	public:
		
		MemoryManager memoryManager;				// Memory manager
		SharedMemoryManager sharedMemoryManager;	// SharedMemory manager
		
	public:
		
		Engine();
		~Engine();
		
		/*
		 Singleton management
		 */
		static void init();
		static void destroy();
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
		void addProcessItem( ProcessItem *item );			// Function to add a Process. It will be notifier by delegate mechanism
		
		void finishProcessItem( ProcessItem *item );		// Notification that this ProcessItem has finished
		void haltProcessItem( ProcessItem *item );			// Notification that this ProcessItem is halted ( blocked until output memory is ready ) 

		void add( DiskOperation *operation );				// Add a disk operation to be executed in the background
		
		void finishDiskOperation( DiskOperation *diskOperation );	// Notification that a disk operation has finished
		
		void checkBackgroundProcesses();	// Check background process in order to see if new threads have to be created
		void checkDiskOperations();			// Check if we can run more disk operations
		
		/*
		 Methods to get information about the state
		 */
		
		// Function to fill part of the message sent to the controller ( informing about status )
		void fill(network::WorkerStatus*  ws);				

	private:

		// Find the position in the list to inser a new element
		std::list<EngineElement*>::iterator _find_pos( EngineElement *e);

		// Get the next background element to be executed in a parallel thread
		ProcessItem* _getNextItemToRun();
		
		
	};
	
};


#endif