/* ****************************************************************************
*
* FILE                     DataManager.cpp - 
*
* AUTHOR                   Andreu Urruela
*
* CREATION DATE            Nov 8 2010
*
*/
#include "DataManager.h"					// Own interface
#include "DataManagerCommandResponse.h"		// ss::DataManagerCommandResponse
#include "LogFile.h"						// ss::LogFile
#include "DataManagerItem.h"				// ss::DataManagerItem
#include <ctime>							// time_t

namespace ss {


	DataManager::DataManager( std::string  fileName  ) : file( fileName ) 
	{
		task_counter = 1;
		// Open in write mode
		if( !file.openToAppend( ) )
		{
			std::cerr << "Error opening controller log file " << file.getFileName() << std::endl;
			exit(1);
		}
		
		
	}	

	void DataManager::initSession()
	{
		// Reload data from the log file
		_reloadData();
		
		// Lod init session
		file.write( 0 , today() ,data::Command_Action_Session );
	}
	
	
	
	void DataManager::_reloadData()
	{
		
		// Clear state
		_clear();
		
		// Open in read mode to recover state
		LogFile read_file( file.getFileName() );	

		// Map of current tasks with the list of previous commands to run
		au::map<size_t,DataManagerItem> task;	
		
		if( read_file.openToRead() )
		{
			
			data::Command c;
			while( read_file.read( c ) )
			{
				size_t task_id = c.task_id();
				
				switch (c.action()) {
					case data::Command_Action_Begin:
					{
						// Make sure we never repeate task id
						if( task_counter <= task_id)
							task_counter = task_id+1;
						
						// Create the task item to run
						DataManagerItem *item = task.findInMap( task_id );
						if( !item )
						{
							task.insertInMap( task_id  , new DataManagerItem( task_id ) );
						}
					}
						break;
					case data::Command_Action_Finish:
					{
						// Extract the item with all the commands
						DataManagerItem *item = task.extractFromMap( task_id );

						_beginTask(task_id);
						item->run(this);
						_finishTask(task_id);
						delete item;
						

					}
						break;
					case data::Command_Action_Cancel:
					{
						// Extract the item but no run any of them
						DataManagerItem *item = task.extractFromMap( task_id );
						delete item;

					}
						break;
						
					case data::Command_Action_Comment:
						// Nothing to do here
						break;
						
					case data::Command_Action_Operation:
					{
						DataManagerItem *item = task.findInMap( task_id );
						
						if( item )
						{
							std::string command = c.command();
							item->addCommand( command );	// Record to undo commands if necessary
						}
						else 
						{
							std::cout << "Warning: Ignored command since task is not iniciated " << c.command() << "\n";
							// Ignore command since it has not been iniciated before...
						}
						
					}
						break;
						
					case data::Command_Action_Session:
					{
						task.clearMap();	// Remove all current tasks ( like if all of them were cancel )
					}						
						break;
				}
			}
		}
		
		// Close the log file as "read mode"
		read_file.close();
		
	}
	
	
	std::string DataManager::today()
	{
		
		time_t t = time(NULL);
		struct tm timeinfo;
		char buffer_time[100];
		
		localtime_r ( &t , &timeinfo );
		strftime (buffer_time,100,"%d/%m/%Y (%X)",&timeinfo);
		
		return std::string( buffer_time );
	}
	
	void DataManager::beginTask( size_t task_id  , std::string command )
	{
		lock.lock();
		
		// Update the counter so coherent task ids are given latter on
		if ( task_counter <= task_id )
			task_counter = task_id+1;

		if( active_tasks.find( task_id ) == active_tasks.end() )
		{
			active_tasks.insert( task_id );
			file.write( task_id , command , data::Command_Action_Begin  );
		}
		
		_beginTask(task_id);
		
		lock.unlock();
	}
	
	void DataManager::finishTask( size_t task_id )
	{
		lock.lock();
		
		if( active_tasks.find( task_id ) != active_tasks.end() )
		{
			active_tasks.erase( task_id );
			file.write( task_id , "" , data::Command_Action_Finish  );
		}
		
		_finishTask(task_id);
		
		lock.unlock();
	}
	
	void DataManager::cancelTask( size_t task_id, std::string error )
	{
		lock.lock();
		
		if( active_tasks.find( task_id ) != active_tasks.end() )
		{
			active_tasks.erase( task_id );
			file.write( task_id , "" , data::Command_Action_Cancel  );
		}
		
		_reloadData();	// Reload data to make the cancel effect
		
		lock.unlock();
		
	}
	
	void DataManager::addComment( size_t task_id , std::string comment)
	{
		lock.lock();
		
		file.write( task_id , comment ,data::Command_Action_Comment  );
		
		lock.unlock();
	}
	

	DataManagerCommandResponse DataManager::runOperation( size_t task_id , std::string command )
	{
		lock.lock();
		DataManagerCommandResponse ans = _runOperation( task_id , command );
		lock.unlock();
		return ans;

	}
	
	
	DataManagerCommandResponse DataManager::_runOperation( size_t task_id , std::string command )
	{
		DataManagerCommandResponse ans;
		
		if( active_tasks.find( task_id ) != active_tasks.end() )
		{
			ans = _run( task_id, command );
			file.write( task_id , command ,data::Command_Action_Operation );
			
			// Log the answer for debugging
			std::ostringstream output;
			output << "operation " << command << ": ";
			if( ans.error )
				output << "RESPONSE ERROR: " << ans.output;
			else
				output << "RESPONSE: " << ans.output;
		
			file.write(task_id, output.str() , data::Command_Action_Comment );
			
		}
		else
		{
			ans.error = true;
			ans.output = "Task was not active in the data manager, so this command was not executed";
		}
		
		return ans;
	}
	
	
	
	
	
	
}
