#ifndef SAMSON_MODULES_MANAGER_H
#define SAMSON_MODULES_MANAGER_H

#include <map>                       /* std::map */
#include <string>                    /* std::string */


#include "au/CommandLine.h"          /* AUCommandLine                            */
#include "au/ErrorManager.h"         /* Lock                            */
#include "au/Singleton.h"
#include "au/containers/map.h"       // au::map
#include "au/mutex/Token.h"          /* Lock                            */
#include "au/string.h"               // au::xml_...

#include "samson/common/coding.h"    // ss:KVInfo
#include "samson/common/samson.pb.h"  // samson::network::...
#include "samson/common/status.h"

#include "samson/module/Factory.h"      // au::Factory
#include "samson/module/KVFormat.h"     // samson::KVFormat
#include "samson/module/Module.h"    // samson::Module

#include "samson/common/Visualitzation.h"

namespace samson {
class Data;
class DataInstance;
class Operation;

class ModulesManager {
public:

  ~ModulesManager();

  void addModulesFromDefaultDirectory();
  void addModulesFromDirectory(std::string dir_name);
  void clearModulesManager();

  // Get collection for queries
  samson::gpb::Collection *getModulesCollection(const Visualization& visualitzation);
  samson::gpb::Collection *getDatasCollection(const Visualization& visualitzation);
  samson::gpb::Collection *getOperationsCollection(const Visualization& visualitzation);

  // Local table of modules
  std::string GetTableOfModules();
  
  // Unique interface to get data and operations
  Data *getData(std::string name);
  Operation *getOperation(std::string name);

  // Satic method to analyze module files
  static Status loadModule(std::string path, Module **module, std::string *version_string);

private:

  ModulesManager();  // !< Private constructor to implement singleton
  friend class au::Singleton<ModulesManager>;

  void closeHandlers();

  // Add Modules funcitons
  void addModule(std::string path);
  void addModules();

  au::Token token_modules;                     // !< General lock for modules accessquit
  au::map< std::string, Module > modules;      // Individual modules ( just for listing )
  std::vector<void *> handlers;                // Open handlers
};
}

#endif  // ifndef SAMSON_MODULES_MANAGER_H
