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
#ifndef SAMSON_MODULE_H
#define SAMSON_MODULE_H

#include <dlfcn.h>
#include <map>
#include <string>

#include <samson/module/Data.h>                   /* Data                                     */
#include <samson/module/Operation.h>              /* Operation                                */

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"                   // LmtModuleManager


namespace samson {
class Module {
public:

  std::string name;                               // Name of this container
  std::string version;                            // Version of this module
  std::string author;                             // Author of this module (mail included to report bugs)

  std::map<std::string, Operation *>  operations;  // Vector containing operations (map, generate, reduce)
  std::map<std::string, Data *>      datas;  // Vector containing data

  friend class ModulesManager;

  std::string file_name;

  Module() {
  }

  Module(std::string _name, std::string _version, std::string _author) {
    name    = _name;
    version = _version;
    author  = _author;
  }

  ~Module() {
    LM_T(LmtModuleManager, ("Destructor for module:'%s'", name.c_str()));
    clearModule();
  }

public:

  void getInfo(std::ostringstream& output) {
    output << "<module>\n";

    output << "<name><![CDATA[" << name << "]]></name>\n";
    output << "<version><![CDATA[" << version << "]]></version>\n";
    output << "<author><![CDATA[" << author << "]]></author>\n";

    output << "<num_operations>" << operations.size() << "</num_operations>\n";
    output << "<num_datas>" << datas.size() << "</num_datas>\n";

    output << "<operations>\n";
    for (std::map<std::string, Operation *>::iterator o = operations.begin(); o != operations.end(); o++) {
      o->second->getInfo(output);
    }
    output << "</operations>\n";

    output << "<datas>\n";
    for (std::map<std::string, Data *>::iterator d = datas.begin(); d != datas.end(); d++) {
      d->second->getInfo(output);
    }
    output << "</datas>\n";

    output << "</module>\n";
  }

  Operation *getOperation(std::string name) {
    std::map<std::string, Operation *>::iterator i;
    i = operations.find(name);
    if (i == operations.end()) {
      return NULL;
    } else {
      return i->second;
    }
  }

  Operation::Type getOperationType(std::string name) {
    Operation *o =  getOperation(name);

    if (!o) {
      return Operation::unknown;
    }




    return o->getType();
  }

  /**
   * Function to get a new instance
   */

  Data *getData(std::string name) {
    std::map<std::string, Data *>::iterator i = datas.find(name);
    if (i == datas.end()) {
      return NULL;
    } else {
      return i->second;
    }
  }

  bool checkData(std::string name) {
    if (name == "txt") {
      return true;                              // Spetial case
    }
    std::map<std::string, Data *>::iterator i = datas.find(name);
    if (i == datas.end()) {
      return false;
    } else {
      return true;
    }
  }

public:

  /**
   * Add operation to this module
   */

  void add(Operation *operation) {
    Operation *previous_operation  = extractOperation(operation->getName());

    if (previous_operation) {
      delete previous_operation;
    }

    operations.insert(std::pair<std::string, Operation *>(operation->getName(), operation));
    LM_T(LmtModuleManager, ("Module operation inserted: '%s' at operation:%p", operation->getName().c_str(), operation));
  }

  void add(Data *data) {
    Data *previous_data = extractData(data->getName());

    if (previous_data) {
      delete previous_data;
    }

    datas.insert(std::pair<std::string, Data *> (data->getName(), data));
    LM_T(LmtModuleManager, ("Module data inserted: '%s' at data:%p", data->getName().c_str(), data));
  }

  /*
   * Remove operations and datas
   */

  Operation *extractOperation(std::string name) {
    std::map<std::string, Operation *>::iterator iter = operations.find(name);

    if (iter == operations.end()) {
      return NULL;
    } else {
      Operation *operation = iter->second;
      operations.erase(iter);
      return operation;
    }
  }

  Data *extractData(std::string name) {
    std::map<std::string, Data *>::iterator iter = datas.find(name);

    if (iter == datas.end()) {
      return NULL;
    } else {
      Data *data = iter->second;
      datas.erase(iter);
      return data;
    }
  }

  void clearModule() {
    // Remove all operations and datas
    for (std::map<std::string, Operation *>::iterator o = operations.begin(); o != operations.end(); o++) {
      LM_T(LmtModuleManager, ("delete operation:%s with val:%p", o->first.c_str(), o->second));
      delete o->second;
    }

    for (std::map<std::string, Data *>::iterator d = datas.begin(); d != datas.end(); d++) {
      LM_T(LmtModuleManager, ("delete data:%s with val:%p", d->first.c_str(), d->second));
      delete d->second;
    }

    datas.clear();
    operations.clear();
  }
};
}

#endif  // ifndef SAMSON_MODULE_H
