#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>

#include "samsonVersion.h"       /* SAMSON_VERSION                           */
#include "samsonDirectories.h"   /* SAMSON_*_DIRECTORY                       */
#include "samsonTuning.h"        /* SAMSON_DEFAULT_MEMORY/CORES              */
#include "NetworkManager.h"
#include "TaskManager.h"
#include "Module.h"
#include "MemoryController.h"
#include "KVManager.h"
#include "SSMonitor.h"
#include "ModulesManager.h"
#include "SSLogger.h"
#include "KVSetMaintainer.h"
#include "Task.h"                /* Task                                     */
#include "SSManager.h"           /* Own interface                            */



namespace ss
{
	
	void createDir( const char *dirName )
	{
		// Check if exist...
		struct stat info;
		int res = stat(dirName, &info);
		
		if (res == -1)	//Error probably because file does not exist
		{
			if( mkdir( dirName ,S_IRWXU) )
			{
				std::cerr << "Error: not possible to access/create directory " << dirName << std::endl;
				exit(1);
			}
		} 
		else 
		{
			if (!S_ISDIR(info.st_mode)) {
				std::cerr << "Error: not possible to access/create directory " << dirName << " because it is not a directory" << std::endl;
				exit(1);
			}
		}
		
		
		// Check if we have the right permissions
		if (access(dirName, R_OK | W_OK | X_OK))
		{
			std::cerr << "Error: not possible to access/create directory " << dirName << " (insufficient access rigths)" << std::endl;
			exit(1);
		}		
	}
	
#pragma mark SSManagerConsole
	
	void SSManagerConsole::evalCommand(std::string command )
	{
		// Run the command in the platform
		CommandResponse response = TaskManager::shared()->runCommand( NULL, command , false );
		
		if ( response.error )
			writeErrorOnConsole( response.message );
		else
			writeBlockOnConsole( response.message );
		
		// Check spetial command "quit"
		AUCommandLine cmd;
		cmd.parse(command);
		if( cmd.get_argument(0) == "quit")
			quit();	//Quit console
		if( cmd.get_argument(0) == "crash")
			exit(0);	//Quit console
		
	}
	
	
	std::string SSManagerConsole::getPrompt()
	{
		return "SAMSON> ";
	}
	
	std::string SSManagerConsole::getHeader()
	{
		std::ostringstream o;
		o << "SAMSON v." << SAMSON_VERSION << std::endl;
		return o.str();
	}
	
	
#pragma mark Singleton for SSManager
	
	SSManager * ssManager = NULL;
	
	SSManager* SSManager::shared()
	{
		if( !ssManager )
			ssManager = new SSManager();
		
		return ssManager;
	}
	
	SSManager::SSManager(  ) 
	{
	}

	void SSManager::run( int args , const char *argv[] )
	{
		// Set the global variable to unfinish
		finish = false;
		
		// Create the workspace/state/log/tmp directories
		createDir( SAMSON_BASE_DIRECTORY);
		createDir( SAMSON_WORKSPACE_DIRECTORY);
		createDir( SAMSON_LOG_DIRECTORY);
		createDir( SAMSON_DATA_DIRECTORY);
		
		AUCommandLine cmdLine;
		cmdLine.set_flag_uint64("memory", SAMSON_DEFAULT_MEMORY);
		cmdLine.set_flag_uint64("cores", SAMSON_DEFAULT_CORES);

		cmdLine.set_flag_boolean("console");
		cmdLine.set_flag_boolean("nconsole");
		cmdLine.parse(args, argv);
		
		// Create all necessary elements
		
		SSLogger::shared();
		ModulesManager::shared();
		KVManager::shared()->addAvailableMemory( cmdLine.get_flag_uint64("memory") );
		
		NetworkManager::shared();		// Run the main network interface
		SimpleNetworkManager::shared();	// Run the simple server

		SSMonitor::shared();			// Run the main monitoring tool

		
		// run the thread to keep KVSets on disk and on memory
		runKVSetMaintainer();
		
		// Run automatic tasks scheduler thread
		runAutomaticTasksScheduler();
		
		// Run tasks workers
		runTasksWorkers( cmdLine.get_flag_uint64("cores") );
		
		
		// Run monitorization thread
		runMonitorizationThread();
		
		SSLogger::shared()->log( SSLogger::message ,  "SAMSON Platform starts");
		
		
		if( cmdLine.get_flag_bool("console") )
		{
			
			// Run the cosole
			console = new SSManagerConsole( false );
			console->run();
			delete console;
		}
		else if( cmdLine.get_flag_bool("nconsole") )
		{
			// Run the ncurses cosole
			console = new SSManagerConsole( true );
			console->run();
			delete console;
		}
		else
		{
			while( !finish )
				sleep(1);
		}
		
		
		SSLogger::log( SSLogger::message , "SAMSON Platform finish normally" );
		exit(0);
	}
	
	
	

	
	

	
	
}

