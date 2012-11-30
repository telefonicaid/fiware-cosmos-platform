/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#ifndef SAMSON_MODULES_MANAGER_H
#define SAMSON_MODULES_MANAGER_H

#include <map>                         /* std::map */
#include <string>                      /* std::string */

#include "au/CommandLine.h"            /* AUCommandLine                            */
#include "au/ErrorManager.h"           /* Lock                            */
#include "au/containers/SharedPointer.h"
#include "au/containers/map.h"       // au::map
#include "au/mutex/Token.h"            /* Lock                            */
#include "au/singleton/Singleton.h"
#include "au/string/StringUtilities.h"  // au::xml_...
#include "samson/common/coding.h"    // ss:KVInfo
// samson::network::...
#include "samson/common/Visualitzation.h"
#include "samson/common/status.h"
#include "samson/module/Factory.h"      // au::Factory
#include "samson/module/KVFormat.h"     // samson::KVFormat
#include "samson/module/Module.h"    // samson::Module
namespace samson {
class Data;
class DataInstance;
class Operation;

class ModulesManager {
public:

  ~ModulesManager();

  void AddModulesFromDefaultDirectory(au::ErrorManager & error);
  void AddModulesFromDirectory(const std::string& dir_name, au::ErrorManager & error);
  void ClearModulesManager();

  // Get collection for queries
  au::SharedPointer<gpb::Collection> GetModulesCollection(const Visualization& visualitzation) const;
  au::SharedPointer<gpb::Collection> GetDatasCollection(const Visualization& visualitzation) const;
  au::SharedPointer<gpb::Collection> GetOperationsCollection(const Visualization& visualitzation) const;

  // Local table of modules
  std::string GetTableOfModules() const;

  // Unique interface to get data and operations
  Data *GetData(const std::string& name) const;
  Operation *GetOperation(const std::string& name) const;

  // Static method to analyze module files
  static Module *LoadModule(const std::string& path, au::ErrorManager & error);

private:

  ModulesManager();     // !< Private constructor to implement singleton
  friend class au::Singleton<ModulesManager>;

  // Add Modules functions
  void AddModules();
  void AddModule(const std::string& path, au::ErrorManager & error);

  au::map<std::string, Module> modules_;     // Individual modules ( just for listing )
};
}

#endif  // ifndef SAMSON_MODULES_MANAGER_H
