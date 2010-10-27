#ifndef _H_DATA_MANAGER
#define _H_DATA_MANAGER

#include "data.pb.h"
#include "traces.h"
#include <iostream>
#include <fstream>			// ifstream , ofstream
#include "Lock.h"			// au::Lock

namespace ss {

	class LogFile
	{
		std::ofstream output;
		std::ifstream input;
		
		char *buffer;
		size_t buffer_size;
		
	public:
		
		LogFile()
		{
			buffer = NULL;
			buffer_size = 0;
		}
		
		~LogFile()
		{
			if( buffer )
				free( buffer );
		}
		
		void checkBuffer( size_t size ) 
		{
			if( buffer_size < size )
			{
				if( buffer )
					free(buffer);
				
				buffer =  (char *) malloc(size);
				buffer_size = size;
			}
		}
		
		
		bool openToRead( std::string fileName )
		{
			input.open( fileName.c_str() );
			return (input.is_open());
		}
		
		bool openToWrite( std::string fileName )
		{
			output.open( fileName.c_str()  );
			return (output.is_open());
		}
		

		bool read( data::Command &c )
		{
			assert(input.is_open());
			
			if(  input.eof() )
				return false;

			size_t message_size;
			input.read((char*)&message_size, sizeof(size_t));

			if(  input.eof() )
				return false;
			
			checkBuffer(message_size);

			input.read( buffer , message_size );
			c.ParseFromArray(buffer,message_size);
			
			return true;
		}
		
		bool write( size_t task_id , std::string command , data::Command::Status status )
		{
			data::Command c;
			
			c.set_task_id( task_id );
			c.set_command( command );
			c.set_status( status );
			
			return write( c );
		}
		
		bool write( data::Command &c )
		{
			
			assert( output.is_open() );
			
			size_t message_size = c.ByteSize();
			size_t packet_size = c.ByteSize() + sizeof( size_t );

			//std::cout << "Packet " << message_size << std::endl;
			
			// Make sure we have space int the buffer
			checkBuffer( packet_size );

			// Write the size first
			*((size_t*)buffer) = message_size ;	// Write the length
			
			// Write the message itself in the buffer
			c.SerializeToArray( buffer + sizeof( size_t ) , message_size	);

			// Write the buffer to disk
			if( output.write(buffer, packet_size)  )
			{
				output.flush();
				return true;
			}
			else
				return false;
			
		}
		
		
	};
	

	class DataManager
	{
		LogFile file;

	protected:
		
		au::Lock lock;			// Lock to protect multi-thread
		
	public:
		
		DataManager( )
		{
		}

		virtual ~DataManager() {}

		/**
		 Funciton to init the log system
		 */
		
		void initDataManager( std::string  fileName  )
		{
			file.openToWrite( fileName );
		}
		
		bool beginTask( size_t task_id )
		{
			lock.lock();
			bool answer = file.write( task_id , "" ,data::Command_Status_Begin  );
			lock.unlock();
			return answer;
		}
		
		bool finishTask( size_t task_id)
		{
			lock.lock();
			bool answer =  file.write( task_id , "" ,data::Command_Status_Finish  );
			lock.unlock();
			return answer;
			
		}
	
		
		bool runOperationOfTask( size_t task_id , std::string command   )
		{
			// Log and run operation
			
			lock.lock();
			bool answer = false;	// By default if no log is possible
			
			if( file.write( task_id , command ,data::Command_Status_Operation  ) )
				answer =  _run( task_id , command );
			else
				answer = false;
			
			lock.unlock();
			
			return answer;
		}
		
		void commit( size_t task_id)
		{
			// log this action is finished
		}
		
	private:

		/**
		 Unique interface to update the status of this DataManager
		 */
		
		virtual bool _run( size_t task_id , std::string command )=0;
		
		
	};

}

#endif
