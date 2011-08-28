
/* ****************************************************************************
 *
 * FILE            SimpleDataManager.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         Samson
 *
 * DATE            8/27/11
 *
 * DESCRIPTION
 *
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_Samson_SimpleDataManager
#define _H_Samson_SimpleDataManager

#include <string>                           // std::string

#include "samson/data/LogFile.h"            // samson::LogFile

namespace samson {
    
    class SimpleDataManagerInterface
    {
    public:
        // Public method to update internal status in recovery mode
        virtual void runRecoveryCommand( std::string command )=0;
    };
    
    class SimpleDataManager
    {
        std::string fileName;                   // File name used to write/read activity logs
        
		LogFile* file;							// File to write
		size_t task_counter;					// Internal counter to give new task ids
        
	public:
        
        // Default constructor
        SimpleDataManager( std::string fileName , SimpleDataManagerInterface *delegate );
        ~SimpleDataManager();
        
        
        // Methods to report activity
		size_t beginTask( std::string comment  );
		void log( size_t task_id , std::string command );
		void cancelTask( size_t task_id , std::string comment );
		void addComment( size_t task_id , std::string comment);
		void finishTask( size_t task_id , std::string comment );

        
        // Simple method to log 1 command task
        void simple_log( std::string command );
        
    private:
        // Function to recover the previous state
        void recoveryPreviousState( SimpleDataManagerInterface * delegate );
        
    };
    
    
    // Recovery system
    
    class SimpleDataManagerRecoveryItem
    {
		size_t task_id;                         // Id of the task
		std::vector<std::string> command;       // List of commands for this task
		
    public:
        
		SimpleDataManagerRecoveryItem( size_t id )
		{
			task_id = id;
		}
        
		void addCommand( std::string c )
		{
			command.push_back(c);
		}
		
		void run( SimpleDataManagerInterface *delegate )
        {
            for ( size_t i = 0 ; i < command.size() ; i ++)
                delegate->runRecoveryCommand( command[i] );
        }
        
    };
    
    class SimpleDataManagerRecovery
    {
        // Map of current tasks with the list of previous commands to run
		au::map<size_t,SimpleDataManagerRecoveryItem> items;	

        // Delegate to run stuff to update internal status
        SimpleDataManagerInterface *delegate;
        
    public:
        
        SimpleDataManagerRecovery( SimpleDataManagerInterface *_delegate )
        {
            delegate = _delegate;
        }
        
        ~SimpleDataManagerRecovery()
        {
            // Clear the open operations
            items.clearMap();
        }

        
        void push( const data::Command &c )
        {
            size_t task_id = c.task_id();
            
            switch (c.action()) 
            {
                    
                case data::Command_Action_Begin:
                {
                    // Create the task item to run
                    SimpleDataManagerRecoveryItem *item = items.findInMap( task_id );
                    
                    if( !item )
                        items.insertInMap( task_id  , new SimpleDataManagerRecoveryItem( task_id ) );
                    else
                        LM_W(("Global job with id %u started twice. Skipping...",task_id));
                }
                    break;
                    
                case data::Command_Action_Finish:
                {
                    // Extract the item with all the commands
                    SimpleDataManagerRecoveryItem *item = items.extractFromMap( task_id );
                    
                    if( item )
                    {
                        item->run( delegate );
                        delete item;
                    }
                    
                }
                    break;
                    
                case data::Command_Action_Cancel:
                {
                    // Extract the item but no run any of them
                    SimpleDataManagerRecoveryItem *item = items.extractFromMap( task_id );
                    if( item )
                        delete item;
                    else
                        LM_W(("Global job with id %u canceles but never started. Skipping...",task_id));
                    
                    
                }
                    break;
                    
                case data::Command_Action_Comment:
                    // Nothing to do here
                    break;
                    
                case data::Command_Action_Operation:
                {
                    SimpleDataManagerRecoveryItem *item = items.findInMap( task_id );
                    
                    if( item )
                    {
                        std::string command = c.command();
                        item->addCommand( command );	// Record to undo commands if necessary
                    }
                    else 
                        LM_W(("Ignored command '%s' since  global job %u is started.",  c.command().c_str() , task_id));
                    
                    
                }
                    break;
                    
                case data::Command_Action_Session:
                {
                    items.clearMap();	// Remove all current tasks ( like if all of them were cancel )
                }						
                    break;
            }            
        }
        
        
        
    };
    
}
#endif
