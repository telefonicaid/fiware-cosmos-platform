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
/* ****************************************************************************
 *
 * FILE                 ModulesManager.cpp -
 *
 */

#include "samson/module/ModulesManager.h"       /* Own interface                            */

#include <dirent.h>
#include <dlfcn.h>
#include <fnmatch.h>
#include <iomanip>
#include <sys/stat.h>
#include <sys/types.h>

#include <ios>
#include <iostream>
#include <string>

#include "au/mutex/TokenTaker.h"                // au::TokenTake
#include "au/tables/Table.h"

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"                 // Lmt...
#include "samson/common/MessagesOperations.h"   // evalHelpFilter(.)
#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup
#include "samson/common/gpb_operations.h"
#include "samson/common/samsonDirectories.h"    /* SAMSON_MODULES_DIRECTORY                 */
#include <samson/module/Data.h>                 /* samson::system::UInt ... */
#include <samson/module/Module.h>
#include <samson/module/samsonVersion.h>   /* SAMSON_VERSION                           */

namespace samson {
ModulesManager::ModulesManager() {
  LOG_V(logs.modules_manager, ("Creating ModulesManager"));
}

ModulesManager::~ModulesManager() {
  LOG_V(logs.modules_manager, ("ModulesManager destructor, calling clearModulesManager"));
  ClearModulesManager();
}

void ModulesManager::ClearModulesManager() {
  // Clear all modules ( handlers are closed in modules destuctors )
  modules_.clearMap();
}

void ModulesManager::AddModulesFromDefaultDirectory(au::ErrorManager & error) {
  AddModulesFromDirectory(au::Singleton<SamsonSetup>::shared()->modules_directory(), error);
}

void ModulesManager::AddModulesFromDirectory(const std::string& dir_name, au::ErrorManager & error) {
  DIR *dp;
  struct dirent *dirp;

  if ((dp = opendir(dir_name.c_str())) == NULL) {
    // logError( "Error opening directory for modules " + dir_name );
    LM_E(("Error opening directory for modules at dir_name:%s", dir_name.c_str()));
    return;
  }

  while ((dirp = readdir(dp)) != NULL) {
    std::string path = dir_name + "/" + dirp->d_name;

    struct ::stat info;
    if (stat(path.c_str(), &info) == -1) {
      LM_E(("Skipping file with path:%s because stat() failed", path.c_str()));
      continue;
    }

    if (S_ISREG(info.st_mode)) {
      LOG_V(logs.modules_manager, ("Adding module from path:'%s'!", path.c_str()));
      au::ErrorManager error_module;
      AddModule(path, error_module);

      // Propagate messages, warnings and errors
      error.Add(error_module, au::str("Loading %s:", path.c_str()));
    }
  }
  closedir(dp);
}

typedef Module *(*moduleFactory)();
typedef const char *(*getVersionFunction)();

void ModulesManager::AddModule(const std::string& path, au::ErrorManager & error) {
  // If the same path is being previously loaded, just skip it
  if (modules_.findInMap(path) != NULL) {
    return;  // No error is notified
  }
  // Load module from path
  Module *module = LoadModule(path, error);
  if (error.HasErrors()) {
    return;
  }

  // Check if there is a module with the same name ( other file )
  au::map<std::string, Module>::iterator it;
  for (it = modules_.begin(); it != modules_.end(); ++it) {
    if (module->name == it->second->name) {
      error.AddError(au::str("There is a previously laoded module with the same name %s", module->name.c_str()));
      delete module;
      return;
    }
  }

  LOG_V(logs.modules_manager, ("Adding module %s (%s) with %d ops & %d data-types",
                               module->name.c_str(),
                               path.c_str(),
                               static_cast<int>(module->operations.size()),
                               static_cast<int>(module->datas.size())));

  // Insert in the modules map
  modules_.insertInMap(path, module);
}

Module *ModulesManager::LoadModule(const std::string& path, au::ErrorManager & error) {
  LOG_V(logs.modules_manager, ("Adding module at path %s", path.c_str()));

  void *hndl = dlopen(path.c_str(), RTLD_NOW);
  if (hndl == NULL) {
    error.AddError("Wrong format. Unable to 'dlopen' file.");
    LOG_W(logs.modules_manager, ("Unable to 'dlopen' file '%s'. dlerror: '%s'", path.c_str(), dlerror()));
    return NULL;
  }

  void *mkr = dlsym(hndl, "moduleCreator");
  if (mkr == NULL) {
    error.AddError("Wrong format. Unable to do 'dlsym' ");
    LOG_W(logs.modules_manager, ("Unable to do 'dlsym' for file '%s'. dlerror: '%s'", path.c_str(), dlerror()));
    dlclose(hndl);
    return NULL;
  }

  void *getVersionPointer = dlsym(hndl, "getSamsonVersion");
  if (getVersionPointer == NULL) {
    error.AddError("Wrong format. Unable to do 'dlsym' ");
    LOG_W(logs.modules_manager, ("Not possible to dlsym for file '%s' with dlerror():'%s'", path.c_str(), dlerror()));
    dlclose(hndl);
    return NULL;
  }

  moduleFactory f = (moduleFactory)mkr;
  getVersionFunction fv = (getVersionFunction)getVersionPointer;

  std::string version_string = fv();
  if (version_string != SAMSON_VERSION) {
    error.AddError(au::str("Wrong version string ( %s != %s )", version_string.c_str(), SAMSON_VERSION));
    dlclose(hndl);
    return NULL;
  }

  // Get module and get version
  Module *module = f();

  if (!module) {
    error.AddError("Error running module method to get a general instance");
    dlclose(hndl);
    return NULL;
  }

  // Extra information for this module
  module->set_file_name(path);
  module->set_hndl(hndl);  // Deleting the module, close this handler

  return module;
}

std::string ModulesManager::GetTableOfModules() const {
  au::tables::Table table("Name,left|Version|#Operations|#Datas|Author,left");

  table.setTitle("Modules");
  au::map<std::string, Module>::const_iterator it;
  for (it = modules_.begin(); it != modules_.end(); ++it) {
    std::string name = it->second->name;

    Module *module = it->second;
    au::StringVector values;
    values.Push(module->name);
    values.Push(module->version);
    values.Push(module->operations.size());
    values.Push(module->datas.size());
    values.Push(module->author);
    table.addRow(values);
  }
  return table.str();
}

au::SharedPointer<gpb::Collection> ModulesManager::GetModulesCollection(const Visualization& visualitzation) const {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("modules");
  au::map<std::string, Module>::const_iterator it;
  for (it = modules_.begin(); it != modules_.end(); ++it) {
    std::string name = it->second->name;
    if (::fnmatch(visualitzation.pattern().c_str(), name.c_str(), FNM_PATHNAME) == 0) {
      Module *module = it->second;

      gpb::CollectionRecord *record = collection->add_record();

      ::samson::add(record, "name", module->name, "left,different");
      ::samson::add(record, "version", module->version, "different");
      ::samson::add(record, "#operations", module->operations.size(), "f=uint64,sum");
      ::samson::add(record, "#datas", module->datas.size(), "f=uint64,sum");
      ::samson::add(record, "author", module->author, "left,different");
    }
  }
  gpb::Sort(collection.shared_object(), "name");
  return collection;
}

au::SharedPointer<gpb::Collection> ModulesManager::GetDatasCollection(const Visualization& visualization) const {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("datas");
  au::map<std::string, Module>::const_iterator it;
  for (it = modules_.begin(); it != modules_.end(); ++it) {
    Module *module = it->second;

    std::map<std::string, Data *>::iterator it_datas;
    for (it_datas = module->datas.begin(); it_datas != module->datas.end(); it_datas++) {
      std::string name = it_datas->first;
      Data *data = it_datas->second;

      if (::fnmatch(visualization.pattern().c_str(), name.c_str(), FNM_PATHNAME) == 0) {
        gpb::CollectionRecord *record = collection->add_record();

        ::samson::add(record, "name", data->_name, "different");
        ::samson::add(record, "help", data->help(), "left,different");
      }
    }
  }
  return collection;
}

au::SharedPointer<gpb::Collection> ModulesManager::GetOperationsCollection(const Visualization& visualization) const {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("operations");
  au::map<std::string, Module>::const_iterator it;
  for (it = modules_.begin(); it != modules_.end(); ++it) {
    Module *module = it->second;

    std::map<std::string, Operation *>::iterator it_operation;
    for (it_operation = module->operations.begin(); it_operation != module->operations.end(); it_operation++) {
      std::string name = it_operation->first;
      Operation *operation = it_operation->second;

      if (::fnmatch(visualization.pattern().c_str(), name.c_str(), FNM_PATHNAME) == 0) {
        gpb::CollectionRecord *record = collection->add_record();

        ::samson::add(record, "name", operation->_name, "left,different");
        ::samson::add(record, "type", operation->getTypeName(), "different");

        // ::samson::add( record , "help" , operation->helpLine()    , "" );
        if (visualization.get_flag("-v")) {
          ::samson::add(record, "inputs", operation->inputFormatsString(), "");
          ::samson::add(record, "outputs", operation->outputFormatsString(), "");
        }
      }
    }
  }
  return collection;
}

Data *ModulesManager::GetData(const std::string& name) const {
  // Search in all modules ( inefficient but generic )

  au::map<std::string, Module>::const_iterator it_modules;
  for (it_modules = modules_.begin(); it_modules != modules_.end(); it_modules++) {
    Data *data = it_modules->second->getData(name);
    if (data) {
      return data;
    }
  }
  return NULL;
}

Operation *ModulesManager::GetOperation(const std::string& name) const {
  // Search in all modules ( inefficient but generic )

  au::map<std::string, Module>::const_iterator it_modules;
  for (it_modules = modules_.begin(); it_modules != modules_.end(); it_modules++) {
    Operation *operation = it_modules->second->getOperation(name);
    if (operation) {
      return operation;
    }
  }
  return NULL;
}
}
