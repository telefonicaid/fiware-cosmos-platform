#ifndef SAMSON_MODULES_MANAGER_H
#define SAMSON_MODULES_MANAGER_H

#include <map>                       /* std::map */
#include <string>                    /* std::string */


#include "au/CommandLine.h"          /* AUCommandLine                            */
#include "au/ErrorManager.h"         /* Lock                            */
#include "au/containers/map.h"       // au::map
#include "au/mutex/Token.h"          /* Lock                            */
#include "au/string.h"               // au::xml_...

#include "samson/common/coding.h"    // ss:KVInfo
#include "samson/common/samson.pb.h" // samson::network::...
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
  std::string owner_;                          // Module who created the ModulesManager
  au::Token token_modules;                     // !< General lock for modules accessquit

  au::map< std::string, Module > modules;      // Individual modules ( just for listing )


  std::vector<void *> handlers;                // Open handlers

  ModulesManager(std::string calling_module);  // !< Private constructor to implement singleton

public:

  ~ModulesManager();

  static void init(std::string calling_module);
  static void destroy(std::string calling_module);
  static ModulesManager *shared();
  std::string get_owner() {
    return owner_;
  };


public:

  static Status loadModule(std::string path, Module **module, std::string *version_string);

  std::string getModuleFileName(std::string module);

public:

  // Reload modules from default directories
  void reloadModules();

  // get xml information
  void getInfo(std::ostringstream& output);

  // Get collection for queries
  samson::gpb::Collection *getModulesCollection(const Visualization& visualitzation);
  samson::gpb::Collection *getDatasCollection(const Visualization& visualitzation);
  samson::gpb::Collection *getOperationsCollection(const Visualization& visualitzation);


  // Get Data &* Operation
  Data *getData(std::string name);
  Operation *getOperation(std::string name);


private:

  void closeHandlers();
  void clearModulesManager();

  // Add Modules funcitons
  void addModulesFromDirectory(std::string dir_name);
  void addModule(std::string path);
  void addModules();
};
}

#endif // ifndef SAMSON_MODULES_MANAGER_H
