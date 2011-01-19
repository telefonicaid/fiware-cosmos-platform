#ifndef _H_PROCESS_MANAGER
#define _H_PROCESS_MANAGER

#include "Token.h"				// au::Token
#include "Stopper.h"			// au::Stopper
#include <list>					// std::list
#include "au_map.h"				// au::list
#include "samson.pb.h"			// ss::network::...

/**
 ProcessManager
 
 Class to manage pending "items" to be executed
 An item is a unit of generic processing where there is no need for IO
 
 At the moment it is implemented as a simple list of pending things.
 It could be improved implementing a priority level indicator
 
 */

namespace ss {

	class ProcessItem;

	class ProcessManager
	{
		
		au::list<ProcessItem> items;		//	List of items to be executed

		au::Token token;					// Token to unique access the items list
		au::Stopper stopper;				// Stopper for the threads where there is nothing to do
		
		int num_processes;					// Number of simultaneous process running ( from setup )

		size_t id;							// Counter to give an id to each process
		
		ProcessManager ( );					// Private constructor ( singleton )

		ProcessItem **runing_item;			// Pointers to the running items
		
	public:
		
		static void init();
		static ProcessManager *shared();

		void addProcessItem( ProcessItem *item );

		std::string getStatus();
		
		// Fill information
		void fill(network::WorkerStatus*  ws);
		
	private:
		
		ProcessItem * getNextItemToProcess();

		
	public:
		void runThread(int id);	// Only executed by thread_create

		
	};

}

#endif
