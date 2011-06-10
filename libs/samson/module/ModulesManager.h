#ifndef SAMSON_MODULES_MANAGER_H
#define SAMSON_MODULES_MANAGER_H

#include <map>					/* std::map */
#include <string>				/* std::string */


#include "au/Lock.h"                /* Lock                            */
#include "au/Error.h"                /* Lock                            */
#include "au/CommandLine.h"			/* AUCommandLine                            */

#include "samson/common/samson.pb.h"			// samson::network::...
#include "samson/common/coding.h"				// ss:KVInfo

#include "samson/module/KVFormat.h"     // samson::KVFormat
#include "samson/module/Module.h"		// samson::Module
#include "samson/module/Factory.h"      // au::Factory

namespace samson {

	class Data;
	class DataInstance;
	class Operation;
	
	class ModulesManager : public Module
	{
		
		au::Lock lock;			//!< General lock for modules access
		
	public:

		ModulesManager();		//!< Private constructor to implement singleton
		~ModulesManager();
		
		static void init();
		static ModulesManager* shared();
		        
	private:
		
		// Add Modules funcitons
		void addModulesFromDirectory( std::string dir_name );
		void addModule( std::string path );
		void addModule(  Module *container );
		void addModules();
		
	public:
		
		// Reload modules from default directories
		void reloadModules();

		// Fill Information for network messages
		void fill( network::OperationList *ol , std::string command  );
		void fill( network::DataList *dl, std::string command  );
		
	};
	
	
	// Utility functions
	void fillKVFormat( network::KVFormat* format , KVFormat f );
	void fillKVInfo( network::KVInfo* info , KVInfo i );
	void fillFullKVInfo( network::KVInfo* info , FullKVInfo i );
	
	
}

#endif
