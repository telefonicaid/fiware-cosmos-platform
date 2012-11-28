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
#ifndef _H_AU_TABLE_DATABASE
#define _H_AU_TABLE_DATABASE

#include "au/CommandLine.h"
#include "au/ErrorManager.h"   // au::ErrorManager
#include "au/console/ConsoleAutoComplete.h"
#include "au/containers/map.h"  // au::map
#include "au/mutex/Token.h"
#include "au/string/StringUtilities.h"  // au::str(...)
#include "au/tables/Collection.h"
#include "au/tables/Table.h"
#include "au/tables/Tree.h"
#include "logMsg/logMsg.h"     // LOG_SW
#include <sstream>                     // std::ostringstream
#include <stdio.h>                     /* sprintf */
#include <string>                      // std::string
#include <sys/time.h>                  // struct timeval

namespace au {
namespace tables {
// Ecosystem of tables to work with them

class DataBase {
  au::map<std::string, Table> tables;
  au::map<std::string, TreeItem> trees;
  au::map<std::string, Collection> collections;

  au::Token token;   // Mutex protection

public:

  DataBase();

  ~DataBase() {
    tables.clearMap();
    trees.clearMap();
    collections.clearMap();
  }

  // Add a table, tree or collection to this database
  void addTable(std::string name, Table *table);
  void addTree(std::string name, TreeItem *tree);
  void addCollection(std::string name, Collection *collection);

  // Main execution function
  std::string runCommand(std::string command);

  // Main autocomplete function
  void autoComplete(au::console::ConsoleAutoComplete *info);

  // Get duplicate of tables, trees or collections
  Table *getTable(std::string name);
  TreeItem *getTree(std::string name);
  Collection *getCollection(std::string name);

  // Get select table
  Table *getTable(std::string name, SelectTableInformation *select);

  // Get all values of a particular column
  StringVector getValuesFromColumn(std::string table_name, std::string column_name);
  StringVector getValuesFromColumn(std::string table_name, std::string column_name, TableSelectCondition *condition);

  // Replace node in a tree
  void replaceNodeInTree(std::string tree_name, TreeItem *node);

private:

  // Non mutex protected methods
  void _addTable(std::string name, Table *table);
  void _addTree(std::string name, TreeItem *tree);
  void _addCollection(std::string name, Collection *collection);

  // Specific Autocomplete
  void autoCompleteTables(au::console::ConsoleAutoComplete *info);
  void autoCompleteTrees(au::console::ConsoleAutoComplete *info);
  void autoCompleteCollections(au::console::ConsoleAutoComplete *info);
  void autoCompleteFieldsOfTable(std::string table_name, au::console::ConsoleAutoComplete *info);


  // Get spetial tables
  Table *getTableOfTables();
  Table *getTableOfTrees();
  Table *getTableOfCollections();
  Table *getErrorTable(int error_code, std::string error_message);
};
}
}

#endif  // ifndef _H_AU_TABLE_DATABASE
