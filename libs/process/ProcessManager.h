#ifndef _H_PROCESS_MANAGER
#define _H_PROCESS_MANAGER

#include "Token.h"				// au::Token
#include "Stopper.h"			// au::Stopper
#include <list>					// std::list
#include "au_map.h"				// au::list
#include "samson.pb.h"			// ss::network::...
#include <set>					// std::set

/**
 ProcessManager
 
 Class to manage pending "items" to be executed
 An item is a unit of generic processing where there is no need for IO
 
 At the moment it is implemented as a simple list of pending things.
 It could be improved implementing a priority level indicator
 
 Bug known: If stopper and token are disconnected, there is a possibility to not wake up processes if a task finish exactly after a new task
 is squeduled and the wake up command is executed. It is a minor stuff is multiple operations are performed
 
 */

namespace ss {

	class ProcessItem;

	class ProcessManager
	{
		
		std::set<ProcessItem*> items;					// List of items to be executed ( all priorities  )

		au::Token token;								// Token to unique access the items list
		au::Stopper stopper;							// Stopper for the threads where there is nothing to do
		
		int num_processes;								// Number of maximum simultaneous process running ( from setup )
		
		std::set<ProcessItem*> running_items;			// Set of items currently being executed
		std::set<ProcessItem*> halted_items;			// Set of items currently being executed but halted
						
		ProcessManager ( );								// Private constructor ( singleton )

		
	public:
		
		static void init();									// Unique function to init this where there is only a single thread
		static ProcessManager *shared();					// Common function to access the singleton implementation

		void addProcessItem( ProcessItem *item );			// Function to add a Process. It will be notifier by delegate mechanism
		
		std::string getStatus();							// Function to get a string version of the internal status ( running processes )
			
		void fill(network::WorkerStatus*  ws);				// Function to fill part of the message sent to the controller ( informing about status )
				
	public:
		
		void runThread( );									// Only executed by thread_create
		
	public:
		
		void notifyFinishProcessItem( ProcessItem *item );	// Notification that this ProcessItem has finished

		void notifyHaltProcessItem( ProcessItem *item );	// Notification that this ProcessItem is halted ( blocked until output memory is ready ) 

	private:
		
		ProcessItem* _getNextItemToRun();

		
	};

}

#endif
