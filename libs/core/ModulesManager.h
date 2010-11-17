#ifndef SAMSON_MODULES_MANAGER_H
#define SAMSON_MODULES_MANAGER_H

#include "Lock.h"                /* Lock                            */
#include <map>					/* std::map */
#include <string>				/* std::string */
#include "samson.pb.h"			// ss::network::...
#include "samson/KVFormat.h"	// ss::KVFormat
#include "KVInfo.h"				// ss::KVInfo

namespace ss {

	class Module;
	class Data;
	class DataInstance;
	class Operation;
	
	class ModulesManager
	{
		
		au::Lock lock;							//!< General lock for modules access
		std::map<std::string, Module*> modules;	//!< Modules managed by the platform
		
	public:

		ModulesManager();				//!< Private constructor to implement singleton
				
	private:
		
		void addModulesFromDirectory( std::string dir_name );
		void addModule( std::string path );
		void addModule(  Module *container );
		
		
		Module *_getModule( std::string name );
		std::string getObjectName( std::string name );
		
		
		void addModules();
		
	public:
		
		void reloadModules();
		
		Data *getData( std::string name );
		bool checkData( std::string name );
		
		DataInstance *newDataInstance( std::string name );
		Operation *getOperation( std::string name);

		Module *getModule( std::string name );
		
		std::string showModules();
		std::string showModule(std::string module);
		std::string showFind( std::string command);
		
		void helpDatas( network::HelpResponse *response , network::Help help  );
		void helpOperations( network::HelpResponse *response, network::Help help  );
		
	};
	
	// Utility functions
	void fillKVFormat( network::KVFormat* format , KVFormat f );
	void fillKVInfo( network::KVInfo* info , KVInfo i );
	
	
	
}

#endif
