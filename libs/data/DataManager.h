#ifndef _H_DATA_MANAGER
#define _H_DATA_MANAGER

#include <iostream>
#include <fstream>			  // ifstream , ofstream

#include "logMsg.h"           // LM_*
#include "dataTracelevels.h"  // LMT_*

#include "data.pb.h"
#include "traces.h"
#include "Lock.h"				// au::Lock
#include "au_map.h"				// au::map
#include <sstream>				// std::ostringstream

namespace ss
{

	class LogFile
	{
		std::string fileName;
		
		std::ofstream output;
		std::ifstream input;
		
		char *buffer;
		size_t buffer_size;
		
	public:
		
		LogFile( std::string _fileName )
		{
			fileName = _fileName;
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
		
		
		void openToRead( )
		{
            LM_T(LMT_FILE, ("opening '%s'", fileName.c_str()));

			input.open( fileName.c_str() );
			if (output.is_open())
				LM_T(LMT_FILE, ("successfully opened '%s'", fileName.c_str()));
			else
				LM_E(("error opening '%s'", fileName.c_str()));

			if (!input.is_open())
			{
				std::cerr << "Error opening log-file of dataManager: " << fileName << std::endl;
				exit(0);
			}
		}
		
		void openToAppend( )
		{
			LM_T(LMT_FILE, ("opening '%s'", fileName.c_str()));

			output.open( fileName.c_str() , std::ios::app );
			if (output.is_open())
				LM_T(LMT_FILE, ("successfully opened '%s'", fileName.c_str()));
			else
				LM_E(("error opening '%s'", fileName.c_str()));

			if (!output.is_open())
			{
				std::cerr << "Error opening log-file of dataManager: " << fileName << std::endl;
				exit(0);
			}
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
		
		void write( size_t task_id , std::string command , data::Command::Action action )
		{
			data::Command c;
			
			c.set_task_id( task_id );
			c.set_command( command );
			c.set_action( action );
			
			write( c );
		}
		
		void write( data::Command &c )
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
				output.flush();
			else
			{
				std::cerr << "Error writing in log file of data manager: " << fileName << std::endl;
				exit(0);
			}
		}
		
		void close()
		{
			if( input.is_open() )
				input.close();
			
			if( output.is_open() )
				output.close();
		}
	};

	class DataManager;
	
	class DataManagerItem
	{
	public:
		
		size_t task_id;						// Id of the task
		std::vector<std::string> command;	// List of commands for this task
		
		DataManagerItem( size_t id )
		{
			task_id = id;
		}
		
		void addCommand( std::string c)
		{
			command.push_back(c);
		}
		
		void un_run( DataManager *manager );
		
	};
	
	
	class DataManagerCommandResponse
	{
	public:
		
		DataManagerCommandResponse()
		{
			error = false;
		}
		
		bool error;
		std::string output;
	};
	
	class DataManager
	{
		LogFile file;
		au::map<size_t,DataManagerItem> task;	// Map of current task with the list of previous commands

	protected:
		
		au::Lock lock;			// Lock to protect multi-thread
		
	public:
		
		DataManager( std::string  fileName  ) : file( fileName )
		{
			
		}
		
		void init()
		{
			// Open in read mode to recover state
			file.openToRead();
			
			data::Command c;
			while( file.read( c ) )
			{
				switch (c.action()) {
					case data::Command_Action_Begin:
						_beginTask( c.task_id() , false );
						break;
					case data::Command_Action_Finish:
						_finishTask( c.task_id() , false );
						break;
					case data::Command_Action_Cancel:
						_cancelTask( c.task_id() , false );
						break;
					case data::Command_Action_Comment:
						_addComment( c.task_id() , c.command() , false );
						break;
					case data::Command_Action_Operation:
						_runOperation( c.task_id() , c.command() , false );
						break;
					default:
						break;
				}
			}
			
			// Clear all non-ended tasks
			_clear();
			
			// Close the log file as "read mode"
			file.close();
			
			// Open in write mode
			file.openToAppend( );
			
		}

		virtual ~DataManager() {}

		/**
		 Funciton to init the log system
		 */
		
		void beginTask( size_t task_id  )
		{
			lock.lock();
			_beginTask( task_id , true );
			lock.unlock();
		}
		
		void finishTask( size_t task_id )
		{
			lock.lock();
			_finishTask( task_id , true );
			lock.unlock();
		}
		
		void cancelTask( size_t task_id )
		{
			lock.lock();
			_cancelTask( task_id ,true );
			lock.unlock();
			
		}
		
		// Add comment to the log file ( for debugging )
		void addComment( size_t task_id , std::string comment)
		{
			lock.lock();
			_addComment( task_id , comment, true);
			lock.unlock();
		}
		
		
		DataManagerCommandResponse runOperation( size_t task_id , std::string command )
		{
			lock.lock();
			DataManagerCommandResponse ans =  _runOperation( task_id , command , true );
			lock.unlock();
			
			return ans;
		}
		
		

		

	private:
		
		
		DataManagerItem* _beginTask( size_t task_id , bool log )
		{
			// Finish a previous one if the same task id exist ( rare but tolerated )
			_cancelTask( task_id , log );
			
			DataManagerItem *item = new DataManagerItem( task_id );
			task.insertInMap( task_id , item );
			
			if ( log )
				file.write( task_id , "" , data::Command_Action_Begin  );
			
			return item;
		}
		void _cancelTask( size_t task_id, bool log )
		{
			DataManagerItem *item = task.extractFromMap( task_id );
			if ( item )
			{
				item->un_run(this);
				delete item;
				if( log )
					file.write( task_id , "" ,data::Command_Action_Cancel  );
			}
		}		
		
		void _finishTask( size_t task_id, bool log )
		{
			if( log )
				file.write( task_id , "" , data::Command_Action_Finish  );
			
			DataManagerItem *item = task.extractFromMap( task_id );
			if( item )
				delete item;
		}
		
		void _addComment( size_t task_id , std::string comment, bool log)
		{
			if( log ) 
				file.write( task_id , comment ,data::Command_Action_Comment  );
		}
		
		DataManagerCommandResponse _runOperation( size_t task_id , std::string command , bool log)
		{
			
			if( log )
				file.write( task_id , command ,data::Command_Action_Operation );
			
			DataManagerItem *item = task.findInMap( task_id );
			
			if( item )
				item->addCommand( command );	// Record to undo commands if necessary
			else {
				item = _beginTask(task_id , log );
				item->addCommand(command);
			}
			
			return _run( command );
		}		
		
		// Remove all non-terminated actions 
		void _clear()
		{

			std::map<size_t,DataManagerItem*>::iterator i;
			for (i = task.begin() ; i != task.end() ; i++)
			{
				DataManagerItem *item = i->second;
				item->un_run(this);
				delete item;
			}
			task.clear();
			
		}
		
		
	private:

		friend class DataManagerItem;
		
		/**
		 Unique interface to update the status of this DataManager
		 */
		
		virtual DataManagerCommandResponse _run( std::string command )=0;

		/**
		 Unique interface to undo a particular task
		 No error is possible here since we are just unding previous commands
		 */
		
		virtual void _un_run( std::string command )=0;
		
		
	};

}

#endif
