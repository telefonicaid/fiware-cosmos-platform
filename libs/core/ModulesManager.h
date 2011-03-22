#ifndef SAMSON_MODULES_MANAGER_H
#define SAMSON_MODULES_MANAGER_H

#include "Lock.h"                /* Lock                            */
#include <map>					/* std::map */
#include <string>				/* std::string */
#include "samson.pb.h"			// ss::network::...
#include "samson/KVFormat.h"	// ss::KVFormat
#include "samson/Module.h"		// ss::Module
#include "coding.h"				// ss:KVInfo

namespace ss {

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
