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

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"			// Lmt...

#include "au/TokenTaker.h"                          // au::TokenTake

#include "samson/common/samsonDirectories.h"		/* SAMSON_MODULES_DIRECTORY                 */
#include "samson/common/MessagesOperations.h"		// evalHelpFilter(.)
#include "samson/common/SamsonSetup.h"			// samson::SamsonSetup

#include <samson/module/samsonVersion.h>   /* SAMSON_VERSION                           */
#include <samson/module/Module.h>
#include "samson/module/ModulesManager.h"			/* Own interface                            */
#include <samson/module/Data.h>			/* samson::system::UInt ... */

#include "select.h"

namespace samson
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
	
	ModulesManager::ModulesManager() : token("ModulesManager")
	{
		reloadModules();
	}
	
	ModulesManager::~ModulesManager()
	{
        // Remove the main instances of the modules created while loading from disk
        modules.clearMap();
        LM_T(LmtModuleManager,("Destroying ModulesManager"));
	}
    
	
	void ModulesManager::init()
	{
		if(modulesManager)
			LM_X(1,("Error initializing Modules Manager twice"));
		modulesManager = new ModulesManager();
	}

    void ModulesManager::destroy(void)
	{
		if( modulesManager )
			delete modulesManager;
		modulesManager = NULL;
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
		addModulesFromDirectory( SamsonSetup::shared()->modulesDirectory() );
	}

	void ModulesManager::addModulesFromDirectory( std::string dir_name )
	{	
        LM_T(LmtModuleManager,("Adding modules from directory %s", dir_name.c_str() ));
        
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
				addModule( path );
			
		}
		closedir(dp);
		
	}

	
	void ModulesManager::reloadModules()
	{
        au::TokenTaker tt( &token , "ModulesManager::reloadModules");
        
		LM_T(LmtModuleManager,("Reloading modules"));
		
		LM_T(LmtModuleManager,("Clear previous operations and data-types"));
		clearModule();  		// Clear this module itself (operations and datas)
        
        // Close handlers for all open modules
		LM_T(LmtModuleManager,("Closing previous modules ( if any )"));
        au::map< std::string  , Module >::iterator m; 
        for ( m =  modules.begin(); m != modules.end() ; m++)
        {
            Module *module = m->second;
            int res = dlclose( module->hndl);
            if( res != 0)
                LM_W(("Error while closing module %s", module->name.c_str() )); 
        }

		LM_T(LmtModuleManager,("Remove list of previous modules"));
        modules.clearMap();     // Remove all modules stored here
		
		// Add modules again
		addModules();
		
        // Add the select operation
        add( new SelectOperation() );
        
		/*
		 Spetial operation to be moved to a proper file
		 */
		
		Operation *compact = new Operation( "samson.compact" , Operation::system );
		compact->inputFormats.push_back( samson::KVFormat::format("*" ,"*") );
		add( compact );
        
		
	}
	
	typedef Module*(*moduleFactory)();
	typedef std::string(*getVersionFunction)();

	void ModulesManager::addModule( std::string path )
	{
        LM_T(LmtModuleManager,("Adding module at path %s", path.c_str() ));
		
		void *hndl = dlopen(path.c_str(), RTLD_NOW);
		if(hndl == NULL)
        {
            LM_W(("Not possible to dlopen for file '%s' with dlerror():'%s'", path.c_str(), dlerror() ));
			return;
		}
        
		void *mkr = dlsym(hndl, "moduleCreator");		
		if(mkr == NULL)
        {
            LM_W(("Not possible to dlsym for file '%s' with dlerror():'%s'", path.c_str(), dlerror() ));
			dlclose(hndl);
			return;
		}
        
		void *getVersionPointer = dlsym(hndl, "getSamsonVersion");
		if(getVersionPointer == NULL)
        {
            LM_W(("Not possible to dlsym for file '%s' with dlerror():'%s'", path.c_str(), dlerror() ));
			dlclose(hndl);
			return;
		}
		
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
            LM_T(LmtModuleManager,("Module %s compiled for version %s ... OK!" , module->name.c_str() , platform_version.c_str()  ));
            LM_T(LmtModuleManager,("Adding module %s (%s) %d ops & %d data-types" , 
                        module->name.c_str() , 
                        path.c_str() , 
                        (int)module->operations.size(),
                        (int)module->datas.size()
                                   ));
            
            module->hndl = hndl;
            addModule( module );
            
            // Insert in the map of modules for listing
            //LM_M(("Adding module %s version %s from %s with %d operations and %d data-types" , module->name.c_str() , module->version.c_str(), path.c_str() , module->operations.size() ,  module->datas.size() ));
            modules.insertInMap(path, module);
        }
        else
        {
            LM_W(("Not loading file %s since its using a different API version %s vs %s" , path.c_str() , platform_version.c_str() , SAMSON_VERSION ));
            delete module;
            
            // Close dynamic link
            dlclose(hndl);
        }

        
		
	}

	void ModulesManager::addModule(  Module *container )
	{
		// Copy all the opertion to this top-level module
		copyFrom( container	);
	}
	
	void ModulesManager::fill( network::OperationList *ol , std::string command  )
	{
        au::TokenTaker tt( &token , "ModulesManager::fill");
        
		au::CommandLine cmdLine;
		cmdLine.set_flag_string("begin" , "");
		cmdLine.set_flag_string("end" , "");
		cmdLine.parse(command);
		
		std::string begin = cmdLine.get_flag_string("begin");
		std::string end = cmdLine.get_flag_string("end");
		
		
		
		{
			for (std::map<std::string , Operation*>::iterator j = operations.begin() ; j != operations.end() ; j++ )
			{
				Operation * op = j->second;
				
				if( filterName( op->getName() , begin , end ) )
				{
					
					network::Operation *o = ol->add_operation();
					o->set_name( j->first );
                    o->set_type( op->getTypeName() );
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
		
	}
	
    
    void ModulesManager::getInfo( std::ostringstream& output)
    {
        au::TokenTaker tt(&token);			//!< General lock for modules access
        
        au::xml_open(output , "modules_manager" );
        au::xml_iterate_map( output , "modules" , modules );
        au::xml_close(output , "modules_manager" );
    }
    
	void ModulesManager::fill( network::DataList *dl, std::string command  )
	{
        au::TokenTaker tt( &token , "ModulesManager::fill");
        
		au::CommandLine cmdLine;
		cmdLine.set_flag_string("begin" , "");
		cmdLine.set_flag_string("end" , "");
		cmdLine.parse(command);
		
		std::string begin = cmdLine.get_flag_string("begin");
		std::string end = cmdLine.get_flag_string("end");
		
		
		
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
		
	}
	
}
