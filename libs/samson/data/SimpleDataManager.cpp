
/* ****************************************************************************
 *
 * FILE            SimpleDataManager.cpp
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         Samson
 *
 * DATE            8/27/11
 *
 * DESCRIPTION
 *
 *  Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#include "SimpleDataManager.h"			// Own interface

namespace samson {
    
	std::string today()
	{
		
		time_t t = time(NULL);
		struct tm timeinfo;
		char buffer_time[100];
		
		localtime_r ( &t , &timeinfo );
		strftime (buffer_time,100,"%d/%m/%Y (%X)",&timeinfo);
		
		return std::string( buffer_time );
	}
    
    
    SimpleDataManager::SimpleDataManager( std::string _fileName , SimpleDataManagerInterface * delegate )
    {
        // File name used to log activity
        fileName = _fileName;
        
        // Init the task counter to "1"
		task_counter = 1;
        
        // Recovering previous state
        recoveryPreviousState( delegate );

        file = new LogFile( fileName );
        
		// Open in write mode
		if( !file->openToAppend( ) )
		{
			LM_X(1, ("Error opening controller log file %s" , file->getFileName().c_str() ));
		}
        
		// Lod init session
		file->write( 0 , today() ,data::Command_Action_Session );
        
        
    }
    
    SimpleDataManager::~SimpleDataManager()
    {
        if( file )
            delete file;
    }
    
    
    // Public methos
    
    size_t SimpleDataManager::beginTask( std::string comment  )
    {
        size_t task_id = task_counter++;
        file->write( task_id , comment , data::Command_Action_Begin  );
        return task_id;
    }
    
    void SimpleDataManager::log( size_t task_id , std::string comment )
    {
        file->write( task_id , comment , data::Command_Action_Operation  );
    }

    void SimpleDataManager::finishTask( size_t task_id , std::string comment )
    {
        file->write( task_id , comment , data::Command_Action_Finish );
    }
    
    void SimpleDataManager::cancelTask( size_t task_id , std::string comment )
    {
        file->write( task_id , comment , data::Command_Action_Cancel );
    }
    
    void SimpleDataManager::addComment( size_t task_id , std::string comment)
    {
        file->write( task_id , comment , data::Command_Action_Comment );
    }
    
    void SimpleDataManager::simple_log( std::string command )
    {
        size_t tmp = beginTask( "SimpleLog with command " + command  );
        log( tmp , command );
        finishTask( tmp , "End of a simpleLog with command " + command  );
    }
        
    
    
    
    void SimpleDataManager::recoveryPreviousState( SimpleDataManagerInterface * delegate )
    {
        LogFile recoveryLogFile( fileName );
        
        if( !recoveryLogFile.openToRead() )
        {
            // Not previous file, so we cannot recover anything
            return;
        }
        
        SimpleDataManagerRecovery recoverySystem( delegate );
        
        data::Command c;
        while( recoveryLogFile.read( c ) )
        {
            // Process command from the file
            recoverySystem.push( c );
        }

        // Close the file
        recoveryLogFile.close();
        
    }


}