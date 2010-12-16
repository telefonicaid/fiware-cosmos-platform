

#ifndef _H_WORKER_TASK_ITEM
#define _H_WORKER_TASK_ITEM

#include <cstring>				// size_t		
#include <assert.h>				// assert(.)
#include <string>				// std::string
#include <sstream>				// std::ostringstream
#include "FileManager.h"		// ss::FileManager ( and derivates )
#include "samson.pb.h"			// ss::network::...
#include "Status.h"				// au::Status
#include "coding.h"				// ss::ProcessAssistantSharedFile
#include "Lock.h"				// au::Lock

namespace ss {

	class ProcessAssistant;
	class WorkerTask;
	
	class WorkerTaskItem : public au::Status , public FileManagerDelegate
	{
		
	public:
		
		enum State
		{
			definition,				// Defining the task
			no_memory,				// Waiting for a shared memory item to be assigned
			ready_to_load_inputs,	// Ready to load data from inputs
			loading_inputs,			// Loading data from disk
			ready_to_run,			// Ready to be process by any ProcessAssistant
			running					// It has beent taken by a ProcessAssistant (should report finish)
		};
		
		
	private:
		
		int shm_input;				// Shared memory identifier for input ( if necessary ) -1 --> no assigned
		State state;				// Main state of this item
		
	public:

		WorkerTask *task;			// Pointer to the parent task
						
		int item_id;				// Item id ( from 0 to ...)

		au::Lock lock;				// Lock to protect num_input_files and confirmed_input_files
		int num_input_files;		// Number of input files requested for this item during loading phase	
		int confirmed_input_files;	// Number of files confirmed by FileManager
		
		network::WorkerTask workerTask;	// copy of the message sent by controller ( environment inside )

		// Error management
		bool error;
		std::string error_message;
		
		WorkerTaskItem( const network::WorkerTask &_workerTask );
		virtual ~WorkerTaskItem(){}

		// Set the parent task
		void setTaskAndItemId( WorkerTask *task , int itemId);
		
		
		// Function to define the required input files for this item ( this depends on the concrete type of item)
		void addInputFiles( FileManagerReadItem *item );	// Function used inside setupInputs in subclasses
		
		// Call back from the File Manager to confirm that an input file is ready
		void fileManagerNotifyFinish(size_t id, bool success);
		
		
		// General setup while asking if it is ready
		
		bool isReadyToRun();

		void setError( std::string _error_message );

		void start();
		
		void setup();
		
		void freeResources();
		
		State getState();
		
		// Funciton to get the run-time status of this object
		void getStatus( std::ostream &output , std::string prefix_per_line );
		
		std::string getStatus();
		
		// Particular operations to run
		virtual void run(ProcessAssistant *pa)=0;
		virtual void receiveCommand( ProcessAssistant *pa , network::ProcessMessage p )=0;
		
		
		// Utility function to get the o-th output queue
		network::Queue getOutputQueue(int o);

		int get_shm_input()
		{
			return shm_input;
		}
		
		static const char * getStateDescription( State s )
		{
			switch (s) {
				case definition:	return "Definition";
				case no_memory:		return "Memory";
				case ready_to_load_inputs: return "Ready to load";
				case loading_inputs: return "Loading";
				case ready_to_run:	return "ready_to_run";
				case running:		return "running";
			}
			return "unknown";
			
		}
		
	protected:
		
		FileManagerReadItem * getFileMangerReadItem( ProcessAssistantSharedFile* file  );
		
		virtual void setupInputs(){}	
		
	};
}
#endif
