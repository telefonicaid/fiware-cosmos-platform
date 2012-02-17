#ifndef SAMSON_MODULES_MANAGER_H
#define SAMSON_MODULES_MANAGER_H

#include <map>					/* std::map */
#include <string>				/* std::string */


#include "au/Token.h"                /* Lock                            */
#include "au/string.h"              // au::xml_...
#include "au/ErrorManager.h"                /* Lock                            */
#include "au/CommandLine.h"			/* AUCommandLine                            */
#include "au/map.h"                 // au::map

#include "samson/common/samson.pb.h"			// samson::network::...
#include "samson/common/coding.h"				// ss:KVInfo
#include "samson/common/status.h"

#include "samson/module/KVFormat.h"     // samson::KVFormat
#include "samson/module/Module.h"		// samson::Module
#include "samson/module/Factory.h"      // au::Factory

namespace samson {

	class Data;
	class DataInstance;
	class Operation;
	
	class ModulesManager : public Module
	{
		
		au::Token token;			//!< General lock for modules access

        au::map< std::string  , Module > modules;  // Individual modules ( just for listing )
        
		ModulesManager();		//!< Private constructor to implement singleton
        
	public:

		~ModulesManager();
		
		static void init();
		static void destroy();
		static ModulesManager* shared();
		        
	private:
		
		// Add Modules funcitons
		void addModulesFromDirectory( std::string dir_name );
		void addModule( std::string path );
		void addModule(  Module *container );
		void addModules();

    public:
        
        static Status loadModule( std::string path , Module** module , std::string* version_string );
        
	public:
		
		// Reload modules from default directories
		void reloadModules();

        // get xml information
        void getInfo( std::ostringstream& output);
        
        // Get collection for queries
        samson::network::Collection* getModulesCollection(VisualitzationOptions options ,  std::string pattern );
        samson::network::Collection* getDatasCollection(VisualitzationOptions options ,  std::string pattern );
        samson::network::Collection* getOperationsCollection(VisualitzationOptions options ,  std::string pattern );
        
	};
	
}

#endif
