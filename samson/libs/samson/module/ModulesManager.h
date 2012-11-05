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

#include <map>                       /* std::map */
#include <string>                    /* std::string */

#include "au/CommandLine.h"          /* AUCommandLine                            */
#include "au/containers/map.h"       // au::map
#include "au/containers/SharedPointer.h"
#include "au/ErrorManager.h"         /* Lock                            */
#include "au/mutex/Token.h"          /* Lock                            */
#include "au/singleton/Singleton.h"
#include "au/string/StringUtilities.h"               // au::xml_...
#include "samson/common/coding.h"    // ss:KVInfo
#include "samson/common/samson.pb.h"  // samson::network::...
#include "samson/common/status.h"
#include "samson/common/Visualitzation.h"
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

    void addModulesFromDefaultDirectory();
    void addModulesFromDirectory(std::string dir_name);
    void clearModulesManager();

    // Get collection for queries
    au::SharedPointer<gpb::Collection> GetModulesCollection(const Visualization& visualitzation);
    au::SharedPointer<gpb::Collection> GetDatasCollection(const Visualization& visualitzation);
    au::SharedPointer<gpb::Collection> GetOperationsCollection(const Visualization& visualitzation);

    // Local table of modules
    std::string GetTableOfModules();

    // Unique interface to get data and operations
    Data *getData(std::string name);
    Operation *getOperation(std::string name);

    // Static method to analyze module files
    static Status loadModule(std::string path, Module **module, std::string *version_string);

  private:

    ModulesManager();   // !< Private constructor to implement singleton
    friend class au::Singleton<ModulesManager>;

    void closeHandlers();

    // Add Modules functions
    void addModule(std::string path);
    void addModules();

    au::map<std::string, Module> modules;   // Individual modules ( just for listing )
    std::vector<void *> handlers;   // Open handlers
};
}

#endif  // ifndef SAMSON_MODULES_MANAGER_H
