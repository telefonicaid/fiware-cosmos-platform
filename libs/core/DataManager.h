#ifndef _H_DATA_MANAGER
#define _H_DATA_MANAGER

#include "data.pb.h"

#define DataManagerBufferLength	2000

namespace ss {

	

	class DataManager
	{
		std::string fileName;
		FILE *file;
		char buffer[DataManagerBufferLength];	// Longer instructions will never be recorded or runned

	protected:
		
		au::Lock lock;			// Lock to protect multi-thread
		
	public:
		
		DataManager( )
		{
			fileName = "no_file";
			file = NULL;
		}

		/**
		 Funciton to init the log system
		 */
		
		void initDataManager(  )
		{
			fileName = getLogFileName();
			file = fopen(fileName.c_str(), "w");
		}
		
		bool process( size_t task_id , std::string command )
		{
			lock.lock();
			bool answer = run( task_id , command );
			lock.unlock();
			return answer;
		}
		
		void commit( size_t task_id)
		{
			// log this action is finished
		}
		
	private:
		
		bool run( size_t task_id , std::string command )
		{
			// Log and run operation
			if( _log( task_id , command ) )
				return _run( task_id , command );
			else
				return false;
		}
		
		bool _log( size_t task_id , std::string command )
		{
			if( !file )
				return false;
			
			// Log the action
			data::Command c;
			
			c.set_task_id( task_id );
			c.set_command( command );
			
			size_t message_size = c.ByteSize();
			size_t packet_size = c.ByteSize() + sizeof( size_t );
			
			*((size_t*)buffer) = message_size ;
			c.SerializeToArray( buffer + sizeof( size_t ) , message_size	);
			
			if( fwrite(buffer, packet_size , 1 , file) == 1)
				return true;
			else
				return false;
			
		}

		/**
		 Unique interface to update the status of this DataManager
		 */
		
		virtual bool _run( size_t task_id , std::string command )=0;

		/**
		 Function to recover the file where logs are stored
		 */
		
		virtual std::string getLogFileName( )=0;
		
		
	};

}

#endif