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

	void DataManager::init()
	{
		// Open in read mode to recover state ( if any previous file )
		if( file.openToRead() )
		{
			
			data::Command c;
			while( file.read( c ) )
			{
				switch (c.action()) {
					case data::Command_Action_Begin:
						_beginTask( c.task_id() ,  c.command() , false );
						break;
					case data::Command_Action_Finish:
						_finishTask( c.task_id() , false );
						break;
					case data::Command_Action_Cancel:
						_cancelTask( c.task_id() , c.command() , false );
						break;
					case data::Command_Action_Comment:
						_addComment( c.task_id() , c.command() , false );
						break;
					case data::Command_Action_Operation:
						_runOperation( c.task_id() , c.command() , false );
						break;
					case data::Command_Action_Session:
						_initSession( c.command() , false );
						break;
				}
			}
		}
		
		// Clear all non-ended tasks
		_clear();
		
		// Close the log file as "read mode"
		file.close();
		
		// Open in write mode
		if( !file.openToAppend( ) )
		{
			std::cerr << "Error opening controller log file " << file.getFileName() << std::endl;
			exit(1);
		}
		
		
		_initSession( today() , true );	// Init a session logging data
		
	}
	
	
	std::string DataManager::today()
	{
		time_t t;
		time( &t );
		char *time = ctime ( &t );
		time[24] = '\0';	// Remove tha additional "\n"
		return std::string( time );
	}
	
	void DataManager::beginTask( size_t task_id  , std::string command )
	{
		lock.lock();
		_beginTask( task_id , command,  true );
		lock.unlock();
	}
	
	void DataManager::finishTask( size_t task_id )
	{
		lock.lock();
		_finishTask( task_id , true );
		lock.unlock();
	}
	
	void DataManager::cancelTask( size_t task_id, std::string error )
	{
		lock.lock();
		_cancelTask( task_id ,error, true );
		lock.unlock();
		
	}
	
	void DataManager::addComment( size_t task_id , std::string comment)
	{
		lock.lock();
		_addComment( task_id , comment, true);
		lock.unlock();
	}
	
	
	DataManagerCommandResponse DataManager::runOperation( size_t task_id , std::string command )
	{
		lock.lock();
		DataManagerCommandResponse ans =  _runOperation( task_id , command , true );
		lock.unlock();
		return ans;
	}
	
	DataManagerItem* DataManager::_beginTask( size_t task_id , std::string command,  bool log )
	{
		
		// Update the counter so no new task can be given
		if ( task_counter <= task_id )
			task_counter = task_id+1;
		
		// Finish a previous one if the same task id exist ( rare but tolerated )
		_cancelTask( task_id , "Error: New task with the same id", log );
		
		DataManagerItem *item = new DataManagerItem( task_id );
		task.insertInMap( task_id , item );
		
		if ( log )
			file.write( task_id , command , data::Command_Action_Begin  );
		
		return item;
	}
	
	void DataManager::_cancelTask( size_t task_id, std::string error, bool log )
	{
		DataManagerItem *item = task.extractFromMap( task_id );
		if ( item )
		{
			item->un_run(this);
			delete item;
			if( log )
				file.write( task_id , error ,data::Command_Action_Cancel  );
		}
	}		
	
	void DataManager::_finishTask( size_t task_id, bool log )
	{
		if( log )
			file.write( task_id , "" , data::Command_Action_Finish  );
		
		DataManagerItem *item = task.extractFromMap( task_id );
		if( item )
			delete item;
	}
	
	void DataManager::_addComment( size_t task_id , std::string comment, bool log)
	{
		if( log ) 
			file.write( task_id , comment ,data::Command_Action_Comment  );
	}
	
	DataManagerCommandResponse DataManager::_runOperation( size_t task_id , std::string command , bool log)
	{
		if( log )
			file.write( task_id , command ,data::Command_Action_Operation );
		
		DataManagerItem *item = task.findInMap( task_id );
		
		if( item )
		{
			item->addCommand( command );	// Record to undo commands if necessary
		}
		else 
		{
			item = _beginTask(task_id , "Automatically created task?" , log );
			item->addCommand(command);
		}
		
		return _run( command );
	}		
	
	void DataManager::_initSession( std::string command , bool log )
	{
		if ( log )
			file.write( 0 , command ,data::Command_Action_Session );

		// Clear all previous actions
		_clear();
	}
	
	// Remove all non-terminated actions 
	void DataManager::_clear()
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
	
}
