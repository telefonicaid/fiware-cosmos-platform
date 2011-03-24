/* ****************************************************************************
*
* FILE                 ModulesManager.cpp - 
*
*/
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <iostream>
#include <iomanip>
#include <ios>
#include <string>

#include <samson/samsonVersion.h>   /* SAMSON_VERSION                           */
#include "samsonDirectories.h"		/* SAMSON_MODULES_DIRECTORY                 */
#include <samson/Module.h>
#include "CommandLine.h"			/* AUCommandLine                            */
#include "ModulesManager.h"			/* Own interface                            */
#include <samson/Data.h>			/* ss::system::UInt ... */
#include "logMsg.h"
#include "traceLevels.h"			// Lmt...
#include "MessagesOperations.h"		// evalHelpFilter(.)
#include "SamsonSetup.h"			// ss::SamsonSetup

namespace ss
{
	/**
	 Utility function to fill data
	 */
	
	
	void fillKVFormat( network::KVFormat* format , KVFormat f )
	{
		format->set_keyformat( f.keyFormat );
		format->set_valueformat( f.valueFormat );
	}
	
	void fillKVInfo( network::KVInfo* info , KVInfo i )
	{
		info->set_size( i.size );
		info->set_kvs( i.kvs );
	}
	
	void fillFullKVInfo( network::KVInfo* info , FullKVInfo i )
	{
		info->set_size( i.size );
		info->set_kvs( i.kvs );
	}
	
	
#pragma mark ----
	
	static ModulesManager *modulesManager=NULL;
	
	ModulesManager::ModulesManager()
	{
		reloadModules();
	}
	
	ModulesManager::~ModulesManager()
	{
        LM_T(LmtModuleManager,("Destroying ModulesManager"));
	}
	
	void free_ModulesManager(void)
	{
		if( modulesManager )
			delete modulesManager;
		modulesManager = NULL;
	}
	
	void ModulesManager::init()
	{
		if(modulesManager)
			LM_X(1,("Error initializing Modules Manager twice"));
		modulesManager = new ModulesManager();
		atexit(free_ModulesManager);
	}
	
	ModulesManager* ModulesManager::shared()
	{
		if( !modulesManager )
			LM_X(1,("Modules Manager not initialized"));

		return modulesManager;
	}
	
	void ModulesManager::addModules()
	{
		// Load modules from "~/.samson/modules" && "/etc/samson/modules"
		std::string modules_dir = SamsonSetup::shared()->modulesDirectory;
		addModulesFromDirectory( modules_dir );
		 
	}

	void ModulesManager::addModulesFromDirectory( std::string dir_name )
	{	
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dir_name.c_str())) == NULL) {
			//logError( "Error opening directory for modules " + dir_name );
			return;
		}
		
		while ((dirp = readdir(dp)) != NULL) {
			
			std::string path = dir_name + "/" + dirp->d_name;
			
			struct ::stat info;
			stat(path.c_str(), &info);
			
			if( S_ISREG(info.st_mode) )
			{
				LM_T( LmtModuleManager ,  ("Adding module from %s", path.c_str()));
				addModule( path );
			}
			
		}
		closedir(dp);
		
	}

	
	void ModulesManager::reloadModules()
	{
		LM_T(LmtModuleManager,("Reloading modules"));
		
		lock.lock();
		
		// Clear this module itself (operations and datas)
		clearModule();
		
		LM_T(LmtModuleManager,("Adding modules again..."));
		
		// Add modules again
		addModules();
		
		/*
		 Spetial operation to be moved to a proper file
		 */
		
		Operation *compact = new Operation( "samson.compact" , Operation::system );
		compact->inputFormats.push_back( ss::KVFormat::format("*" ,"*") );
		add( compact );
		
		lock.unlock();
	}
	
	typedef Module*(*moduleFactory)();
	typedef std::string(*getVersionFunction)();

	void ModulesManager::addModule( std::string path )
	{
		
		void *hndl = dlopen(path.c_str(), RTLD_NOW);
		if(hndl == NULL){
			//logError( "KVWriter " + path );
			return;
		}
		void *mkr = dlsym(hndl, "moduleCreator");		
		void *getVersionPointer = dlsym(hndl, "getSamsonVersion");
		
		if( mkr && getVersionPointer )
		{
			moduleFactory f = (moduleFactory)mkr;
			getVersionFunction fv = (getVersionFunction)getVersionPointer;
			
			Module *module = f();
			std::string platform_version  = fv();
			
			if ( !module )
			{
				LM_E(( "Not possible to load module at path %s (not container found)" , path.c_str()));
				dlclose(hndl);
				return;
			}
			
			if( platform_version == SAMSON_VERSION )
			{
				//SSLogger::log( SSLogger::message, "Loaded module at path" + path );
				module->hndl = hndl;
				addModule( module );
			}
			else
			{
				//SSLogger::log( SSLogger::warning, "Not possible to load module at path" + path + " (wrong API version)" );
				dlclose(hndl);
				delete module;
				
			}
		}
		else
		{
			dlclose(hndl);
			//SSLogger::log( SSLogger::warning, "Not possible to link module at path" + path );
		}
		
		
		
	}

	void ModulesManager::addModule(  Module *container )
	{
		// Copy all the opertion to this top-level module
		moveFrom( container	);
		delete container;
	}
	
	void ModulesManager::fill( network::OperationList *ol , std::string command  )
	{
		au::CommandLine cmdLine;
		cmdLine.set_flag_string("begin" , "");
		cmdLine.set_flag_string("end" , "");
		cmdLine.parse(command);
		
		std::string begin = cmdLine.get_flag_string("begin");
		std::string end = cmdLine.get_flag_string("end");
		
		
		lock.lock();
		
		{
			for (std::map<std::string , Operation*>::iterator j = operations.begin() ; j != operations.end() ; j++ )
			{
				Operation * op = j->second;
				
				if( filterName( op->getName() , begin , end ) )
				{
					
					network::Operation *o = ol->add_operation();
					o->set_name( j->first );
					o->set_help( op->help() );
					o->set_help_line( op->helpLine() );
					
					// Format
					std::vector<KVFormat> input_formats = op->getInputFormats();
					std::vector<KVFormat> output_formats = op->getOutputFormats();
					
					for (size_t i = 0 ; i < input_formats.size() ; i++)
						fillKVFormat( o->add_input() , input_formats[i] );
					
					for (size_t i = 0 ; i < output_formats.size() ; i++)
						fillKVFormat( o->add_output() , output_formats[i] );
				}
				
			}
		}
		
		lock.unlock();		
	}
	
	void ModulesManager::fill( network::DataList *dl, std::string command  )
	{
		au::CommandLine cmdLine;
		cmdLine.set_flag_string("begin" , "");
		cmdLine.set_flag_string("end" , "");
		cmdLine.parse(command);
		
		std::string begin = cmdLine.get_flag_string("begin");
		std::string end = cmdLine.get_flag_string("end");
		
		
		lock.lock();
		
		{
			for (std::map<std::string , Data*>::iterator j = datas.begin() ; j != datas.end() ; j++ )
			{
				
				if( filterName( j->first , begin ,end ) )
				{
					Data * data = j->second;
					network::Data *d = dl->add_data();
					d->set_name( j->first );
					d->set_help( data->help() );
				}
				
			}
		}
		
		lock.unlock();		
	}
	
	
}
