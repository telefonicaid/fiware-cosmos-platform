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
#include <fnmatch.h>

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"			// Lmt...

#include "au/mutex/TokenTaker.h"                          // au::TokenTake

#include "samson/common/MessagesOperations.h"
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
	
	static ModulesManager *modulesManager=NULL;
	
	ModulesManager::ModulesManager() : token("ModulesManager")
	{
	    LM_T(LmtModuleManager,("Creating ModulesManager"));
		reloadModules();
	}
	
	ModulesManager::~ModulesManager()
	{
        LM_T(LmtModuleManager,("Destroying ModulesManager"));
        clearModulesManager();
	}
    
    void ModulesManager::clearModulesManager()
    {
        // Remove the main instances of the modules created while loading from disk
        modules.clearMap();
        
        // Close handlers
        closeHandlers();
    }
    
    void ModulesManager::closeHandlers()
    {
        for ( size_t i = 0 ; i < handlers.size() ; i++ )
            dlclose( handlers[i] );
        handlers.clear();
    }
    
    
    void ModulesManager::destroy()
    {
		if (!modulesManager)
			LM_RVE(("Attempt to destroy a non-initialized Modules Manager"));
        
		LM_T(LmtModuleManager, ("Destroying ModulesManager"));
        delete modulesManager;
        
        modulesManager = NULL;
    }
	
	void ModulesManager::init()
	{
	    LM_T(LmtModuleManager, ("ModulesManager::init() called"));
		if(modulesManager)
		{
			LM_W(("Error initializing Modules Manager twice, ignoring"));
			return;
		}
        
        LM_V(("Init ModulesManager"));
		modulesManager = new ModulesManager();
		LM_T(LmtModuleManager, ("ModulesManager created"));
	}
    
	ModulesManager* ModulesManager::shared()
	{
		if( !modulesManager )
			LM_X(1,("Modules Manager not initialized"));

		return modulesManager;
	}
	


    std::string ModulesManager::getModuleFileName( std::string module_name )
    {
        au::TokenTaker tt( &token , "ModulesManager::reloadModules");
        Module* module = modules.findInMap(module_name);
        if( !module )
            return "";
        
        return module->file_name;
    }

	
	void ModulesManager::reloadModules()
	{
        au::TokenTaker tt( &token , "ModulesManager::reloadModules");

		LM_T(LmtModuleManager,("Reloading modules"));
        
        // Clear all previous modules
        clearModulesManager();
		
		// Add modules again
		addModules();
	}
	
    void ModulesManager::addModules()
	{
		addModulesFromDirectory( SamsonSetup::shared()->modulesDirectory() );
	}
    
	void ModulesManager::addModulesFromDirectory( std::string dir_name )
	{	
        LM_T(LmtModuleManager,("Adding modules from directory %s", dir_name.c_str() ));
        
		DIR *dp;
		struct dirent *dirp;
		if((dp  = opendir(dir_name.c_str())) == NULL) {
			//logError( "Error opening directory for modules " + dir_name );
			LM_E(("Error opening directory for modules at dir_name:%s", dir_name.c_str()));
			return;
		}
		
		while ((dirp = readdir(dp)) != NULL) {
			
			std::string path = dir_name + "/" + dirp->d_name;
			
			struct ::stat info;
			if (stat(path.c_str(), &info) == -1)
			{
				LM_E(("Skipping file with path:%s because stat() failed", path.c_str()));
				continue;
			}
			
			if( S_ISREG(info.st_mode) )
				addModule( path );
			
		}
		closedir(dp);
		
	}
    
    
	typedef Module*(*moduleFactory)();
	typedef std::string(*getVersionFunction)();

    
    
	void ModulesManager::addModule( std::string path )
	{
        LM_T(LmtModuleManager,("Adding module at path %s", path.c_str() ));
		
        // Dynamic link open
		void *hndl = dlopen(path.c_str(), RTLD_NOW);
        
		if(hndl == NULL)
        {
            LM_W(("Unable to 'dlopen' file '%s'. dlerror: '%s'", path.c_str(), dlerror() ));
			return;
		}

		void *mkr = dlsym(hndl, "moduleCreator");		
		if(mkr == NULL)
        {
            LM_W(("Unable to do 'dlsym' for '%s'. dlerror: '%s'", path.c_str(), dlerror() ));
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
            LM_E(( "Not possible to load module at path %s (no container found)" , path.c_str()));
            dlclose(hndl);
            return;
        }
        
        // Save the full path of this module
        module->file_name = path; 

        // Check platform version
        if( platform_version == SAMSON_VERSION )
        {
            Module * previous_module = modules.findInMap( module->name );

            if( previous_module != NULL )
            {
                LM_W(("Error loading module from file %s since it is already loaded" , module->file_name.c_str() ));
                delete module;
                dlclose(hndl);
            }
            
            LM_T(LmtModuleManager,("Module %s compiled for version %s ... OK!" , module->name.c_str() , platform_version.c_str()  ));
            LM_T(LmtModuleManager,("Adding module %s (%s) %d ops & %d data-types" , 
                        module->name.c_str() , 
                        path.c_str() , 
                        (int)module->operations.size(),
                        (int)module->datas.size()
                                   ));
            
            // Insert in the modules map
            modules.insertInMap(module->name, module);
            
            // Keep handler in a vector to close latter
            handlers.push_back(hndl);
            
        }
        else
        {
            LM_W(("Not loading file %s since its using a different API version %s vs %s" , path.c_str() , platform_version.c_str() , SAMSON_VERSION ));
            delete module;
            
            // Close dynamic link
            dlclose(hndl);
        }
		
	}
    
    Status ModulesManager::loadModule( std::string path , Module** module , std::string* version_string )
	{
        LM_T(LmtModuleManager,("Adding module at path %s", path.c_str() ));
		
		void *hndl = dlopen(path.c_str(), RTLD_NOW);
		if(hndl == NULL)
        {
            LM_W(("Unable to 'dlopen' file '%s'. dlerror: '%s'", path.c_str(), dlerror() ));
			return Error;
		}
        
		void *mkr = dlsym(hndl, "moduleCreator");		
		if(mkr == NULL)
        {
            LM_W(("Unable to do 'dlsym' for file '%s'. dlerror: '%s'", path.c_str(), dlerror() ));
			dlclose(hndl);
			return Error;
		}
        
		void *getVersionPointer = dlsym(hndl, "getSamsonVersion");
		if(getVersionPointer == NULL)
        {
            LM_W(("Not possible to dlsym for file '%s' with dlerror():'%s'", path.c_str(), dlerror() ));
			dlclose(hndl);
			return Error;
		}
		
        moduleFactory f = (moduleFactory)mkr;
        getVersionFunction fv = (getVersionFunction)getVersionPointer;
        
        // Get module and get version
        *module = f();
        *version_string = fv();
        
        return OK;
	}

    
    void ModulesManager::getInfo( std::ostringstream& output)
    {
        au::TokenTaker tt(&token);			//!< General lock for modules access
        
        au::xml_open(output , "modules_manager" );
        au::xml_iterate_map( output , "modules" , modules );
        au::xml_close(output , "modules_manager" );
    }
    
    samson::network::Collection* ModulesManager::getModulesCollection(VisualitzationOptions options ,  std::string pattern )
    {
        if( options == normal )
        {
            
        }
        
        samson::network::Collection* collection = new samson::network::Collection();
        collection->set_name("modules");
        au::map< std::string , Module >::iterator it;
        for( it = modules.begin() ; it != modules.end() ; it++ )
        {
            std::string name = it->second->name;
            if( ::fnmatch( pattern.c_str() , name.c_str() , FNM_PATHNAME ) == 0 )
            {
                Module * module = it->second;
                
                network::CollectionRecord* record = collection->add_record();
                
                ::samson::add( record , "name"        , module->name              , "left,different" );
                ::samson::add( record , "version"     , module->version           , "different" );
                ::samson::add( record , "#operations" , module->operations.size() , "f=uint64,sum" );
                ::samson::add( record , "#datas"      , module->datas.size()      , "f=uint64,sum" );
                ::samson::add( record , "author"      , module->author            , "left,different" );
                
            }
        }
        return collection;
        
    }
    samson::network::Collection* ModulesManager::getDatasCollection(VisualitzationOptions options ,  std::string pattern )
    {
        if( options == normal )
        {
            
        }

        samson::network::Collection* collection = new samson::network::Collection();
        collection->set_name("datas");
        
        au::map< std::string , Module >::iterator it;
        for( it = modules.begin() ; it != modules.end() ; it++ )
        {
            Module * module = it->second;
            
            std::map<std::string, Data*>::iterator it_datas;
            for( 
                it_datas = module->datas.begin() ; 
                it_datas != module->datas.end()  ; 
                it_datas++)
            {
                
                std::string name = it_datas->first;
                Data * data = it_datas->second;
                
                if( ::fnmatch( pattern.c_str() , name.c_str() , FNM_PATHNAME ) == 0 )
                {
                    
                    network::CollectionRecord* record = collection->add_record();
                    
                    ::samson::add( record , "name" , data->_name    , "different" );
                    ::samson::add( record , "help" , data->help()   , "left,different" );
                }
                
            }
        }
        return collection;
        
    }

    samson::network::Collection* ModulesManager::getOperationsCollection(VisualitzationOptions options ,  std::string pattern )
    {
        
        samson::network::Collection* collection = new samson::network::Collection();
        collection->set_name("operations");
        
        au::map< std::string , Module >::iterator it;
        for( it = modules.begin() ; it != modules.end() ; it++ )
        {
            Module * module = it->second;
            
            std::map<std::string, Operation*>::iterator it_operation;
            for( 
                it_operation = module->operations.begin() ; 
                it_operation != module->operations.end()  ; 
                it_operation++)
            {
                
                std::string name = it_operation->first;
                Operation * operation = it_operation->second;
                
                if( ::fnmatch( pattern.c_str() , name.c_str() , FNM_PATHNAME ) == 0 )
                {
                    
                    network::CollectionRecord* record = collection->add_record();
                    
                    ::samson::add( record , "name" , operation->_name         , "left,different" );
                    ::samson::add( record , "type" , operation->getTypeName() , "different" );
                    
                    //::samson::add( record , "help" , operation->helpLine()    , "" );
                    if( options == verbose )
                    {
                        ::samson::add( record , "inputs"  , operation->inputFormatsString()  , "" );
                        ::samson::add( record , "outputs" , operation->outputFormatsString()  , "" );
                    }
                }
                
            }
        }
        return collection;        
    }

    
    Data* ModulesManager::getData( std::string name )
    {
        // Searhc in all modules ( inefficient but generic )
        au::map< std::string  , Module >::iterator it_modules;
        for( it_modules = modules.begin() ; it_modules != modules.end() ; it_modules++ )
        {
            Data* data = it_modules->second->getData(name);
            if( data )
                return data;
        }
        return NULL;
    }
    
    Operation* ModulesManager::getOperation( std::string name )
    {
        // Searhc in all modules ( inefficient but generic )
        au::map< std::string  , Module >::iterator it_modules;
        for( it_modules = modules.begin() ; it_modules != modules.end() ; it_modules++ )
        {
            Operation* operation = it_modules->second->getOperation(name);
            if( operation )
                return operation;
        }
        return NULL;        
    }

	
}
