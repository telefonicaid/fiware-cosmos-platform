

#include "au/mutex/TokenTaker.h"
#include "au/string/string.h"

#include "DataBase.h"  // Own interface
#include "Select.h"

namespace au {
namespace tables {
DataBase::DataBase() : token("DataBase") {
}

// Add a table to the collection
void DataBase::addTable(std::string name, Table *table) {
  au::TokenTaker tt(&token);   // Mutex protection for public methods

  _addTable(name, table);
}

// Add a table to the collection
void DataBase::addTree(std::string name, TreeItem *tree) {
  au::TokenTaker tt(&token);   // Mutex protection for public methods

  _addTree(name, tree);
}

void DataBase::addCollection(std::string name, Collection *collection) {
  au::TokenTaker tt(&token);   // Mutex protection for public methods

  _addCollection(name, collection);
}

Table *DataBase::getTable(std::string name) {
  au::TokenTaker tt(&token);   // Mutex protection for public methods
  Table *table = tables.findInMap(name);

  if (!table) {
    return NULL;
  } else {
    return new Table(table);
  }
}

Table *DataBase::getTable(std::string name, SelectTableInformation *select) {
  au::TokenTaker tt(&token);   // Mutex protection for public methods
  Table *table = tables.findInMap(name);

  if (!table) {
    return NULL;
  } else {
    return table->selectTable(select);
  }
}

TreeItem *DataBase::getTree(std::string name) {
  au::TokenTaker tt(&token);   // Mutex protection for public methods
  TreeItem *tree = trees.findInMap(name);

  if (!tree) {
    return NULL;
  } else {
    return new TreeItem(tree);
  }
}

Collection *DataBase::getCollection(std::string name) {
  au::TokenTaker tt(&token);   // Mutex protection for public methods
  Collection *collection = collections.findInMap(name);

  if (!collection) {
    return NULL;
  } else {
    return new Collection(collection);
  }
}

void DataBase::autoComplete(au::ConsoleAutoComplete *info) {
  au::TokenTaker tt(&token);   // Mutex protection for public methods

  if (info->completingFirstWord()) {
    // Add console commands
    info->add("show");
    info->add("show_tables");
    info->add("show_trees");
    info->add("show_collections");

    info->add("info");

    info->add("print_table");
    info->add("print_tree");
    info->add("print_collection");

    info->add("save");
    info->add("table_from_tree");
    info->add("select");
    info->add("select_tree");
    info->add("select_collection");

    return;
  }

  if (info->completingSecondWord("info")) {
    autoCompleteTables(info);
  }
  if (info->completingSecondWord("print_table")) {
    autoCompleteTables(info);
  }
  if (info->completingSecondWord("print_tree")) {
    autoCompleteTrees(info);
  }
  if (info->completingSecondWord("table_from_tree")) {
    autoCompleteTrees(info);
  }
  if (info->completingSecondWord("print_collection")) {
    autoCompleteCollections(info);
  }
  if (info->completingWord() > 1) {
    if (info->firstWord() == "print_table") {
      std::string table_name = info->secondWord();
      autoCompleteFieldsOfTable(table_name, info);
    }

    if (info->firstWord() == "select_collection") {
      info->setHelpMessage("Usage: select_collection <collection> <new_collection> field=value field2=value2 ... ");
    }
  }
}

std::string DataBase::runCommand(std::string command) {
  au::TokenTaker tt(&token);   // Mutex protection for public methods

  std::ostringstream output;

  au::CommandLine cmdLine;

  cmdLine.SetFlagInt("limit", 0);
  cmdLine.SetFlagString("save", "no_save");
  cmdLine.SetFlagString("group", "");
  cmdLine.SetFlagString("divide", "");
  cmdLine.SetFlagString("sort", "");
  cmdLine.SetFlagString("where", "");
  cmdLine.SetFlagString("title", "");
  cmdLine.SetFlagBoolean("first");
  cmdLine.Parse(command);

  int limit = cmdLine.GetFlagInt("limit");
  std::string save = cmdLine.GetFlagString("save");
  std::string group = cmdLine.GetFlagString("group");
  std::string divide = cmdLine.GetFlagString("divide");
  std::string sort = cmdLine.GetFlagString("sort");
  std::string where = cmdLine.GetFlagString("where");
  std::string title = cmdLine.GetFlagString("title");
  bool first = cmdLine.GetFlagBool("first");

  if (cmdLine.get_num_arguments() == 0) {
    return "No command";
  }

  std::string mainCommand = cmdLine.get_argument(0);

  if (mainCommand == "show_tables") {
    Table *table = getTableOfTables();
    _addTable("result",  table);
    return table->str();
  }

  if (mainCommand == "show_trees") {
    Table *table = getTableOfTrees();
    _addTable("result",  table);
    return table->str();
  }

  if (mainCommand == "show_collections") {
    Table *table = getTableOfCollections();
    _addTable("result",  table);
    return table->str();
  }

  if (mainCommand == "show") {
    std::ostringstream output;


    Table *table_tables = getTableOfTables();
    output << table_tables->str();

    output << "\n";

    Table *table_trees = getTableOfTrees();
    output << table_trees->str();

    output << "\n";

    Table *table_collection = getTableOfCollections();
    output << table_collection->str();

    return output.str();
  }

  if (mainCommand == "info") {
    if (cmdLine.get_num_arguments() < 2) {
      return au::string_in_color(
               "Usage: info <table> ( run show_tables to get a list of tables )"
               , "red"
               );
    }

    std::string table_name = cmdLine.get_argument(1);
    Table *table = tables.findInMap(table_name);
    if (!table) {
      return au::str("Unknown table %s", table_name.c_str());
    }

    Table *result = table->getColumnDescriptionTable();
    if (save != "no_save") {
      _addTable(save,  result);
      return au::str("Created table %s", save.c_str());
    } else {
      _addTable("result",  result);
      return result->str();
    }
  }

  if (mainCommand == "print_table") {
    if (cmdLine.get_num_arguments() < 2) {
      return au::string_in_color(
               "Usage: print_table table <fields> [-group fields] [-divide fields] [-sort fields] [-limit X] ( run show_tables to get a list of tables )"
               , "red"
               );
    }

    std::string table_name = cmdLine.get_argument(1);
    Table *table = tables.findInMap(table_name);
    if (!table) {
      return au::str("Unknown table %s", table_name.c_str());
    }

    SelectTableInformation select;
    if (cmdLine.get_num_arguments() < 3) {
      select.addColumns(table->getColumnNames());
    } else {
      for (int i = 2; i < cmdLine.get_num_arguments(); i++) {
        select.columns.push_back(cmdLine.get_argument(i));
      }  // Set the limit of rows to display...
    }
    select.limit = limit;

    if (title != "") {
      select.title = title;
    } else {
      select.title = au::str("Table %s", table_name.c_str());
    } select.group_columns = StringVector::ParseFromString(
      group, ',');
    select.sort_columns = StringVector::ParseFromString(sort, ',');
    select.divide_columns = StringVector::ParseFromString(divide, ',');

    // Where clause
    StringVector where_clauses = StringVector::ParseFromString(where, ',');
    for (size_t w = 0; w < where_clauses.size(); w++) {
      StringVector where_clause_parts = StringVector::ParseFromString(where_clauses[w], '=');
      if (where_clause_parts.size() == 2) {
        std::string name = where_clause_parts[0];
        std::string value = where_clause_parts[1];
        select.conditions.push_back(TableSelectCondition(name, value));
      }
    }

    if (first) {
      Table *_table = table->selectTable(&select);

      // Print first record if any ....
      if (_table->getNumRows() == 0) {
        return au::str(au::red, "No records to print");
      }

      Table *record_table = _table->rows[0]->getTable();
      std::string output = record_table->str();
      delete record_table;

      delete _table;

      return output;
    }

    return table->str(&select);
  }

  if (mainCommand == "table_from_table") {
    if (cmdLine.get_num_arguments() < 3) {
      return au::str(au::red, "Usage: table_from_table table <fields> [-save new_table]");
    }

    std::string table_name = cmdLine.get_argument(1);
    Table *table = tables.findInMap(table_name);
    if (!table) {
      return au::str(au::red, "Unknown table %s", table_name.c_str());
    }

    SelectTableInformation select;

    for (int i = 2; i < cmdLine.get_num_arguments(); i++) {
      select.columns.push_back(cmdLine.get_argument(i));
    }
    select.title = au::str("Table %s", table_name.c_str());
    select.group_columns = StringVector::ParseFromString(group, ',');

    Table *table_result = table->selectTable(&select);

    if (save != "no_save") {
      addTable(save, table_result);
      return au::str(au::purple, "Created table %s", save.c_str());
    } else {
      addTable("result", table_result);
      return table_result->str();
    }
  }

  if (mainCommand == "print_tree") {
    if (cmdLine.get_num_arguments() < 2) {
      return au::string_in_color(
               "Usage: print_tree <tree> ( run show_trees to get a list of trees )"
               , "red"
               );
    }

    std::string tree_name = cmdLine.get_argument(1);
    TreeItem *tree = trees.findInMap(tree_name);
    if (!tree) {
      return au::string_in_color(au::str("Unknown tree %s", tree_name.c_str()), "red");
    }

    if (cmdLine.get_num_arguments() > 2) {
      std::string path = cmdLine.get_argument(2);
      TreeCollection *tree_collection = tree->getTreesFromPath(path);

      std::ostringstream output;
      output << tree_collection->str(limit);
      delete tree_collection;
      return output.str();
    } else {
      return tree->str(limit);
    }
  }

  if (mainCommand == "print_collection") {
    if (cmdLine.get_num_arguments() < 2) {
      return au::string_in_color(
               "Usage: print_collection <collection> ( run show_collections to get a list of collections )"
               , "red"
               );
    }

    std::string collection_name = cmdLine.get_argument(1);
    Collection *collection = collections.findInMap(collection_name);
    if (!collection) {
      return au::string_in_color(au::str("Unknown collection %s", collection_name.c_str()), "red");
    }

    return collection->str(collection_name);
  }

  if (mainCommand == "save") {
    if (cmdLine.get_num_arguments() < 2) {
      return au::string_in_color("Usage: save <new_table_name>", "red");
    }

    std::string table_name = "result";
    Table *table = tables.findInMap(table_name);
    if (!table) {
      return au::string_in_color("No table result to save", "red");
    }

    _addTable(cmdLine.get_argument(1),  new Table(table));
    return "Ok";
  }

  if (mainCommand == "table_from_tree") {
    if (cmdLine.get_num_arguments() < 3) {
      return au::string_in_color("Usage: table_from_tree tree <path>", "red");
    }

    std::string tree_name = cmdLine.get_argument(1);

    TreeItem *tree = trees.findInMap(tree_name);
    if (!tree) {
      return au::string_in_color(au::str("Unknown tree %s", tree_name.c_str()), "red");
    }

    std::string path = cmdLine.get_argument(2);
    Table *result = tree->getTableFromPath(path);

    if (save != "no_save") {
      _addTable(save,  result);
      return au::string_in_color(au::str("Created table %s", save.c_str()), "purple");
    } else {
      _addTable("result",  result);
      return result->str();
    }
  }

  if (mainCommand == "select_tree") {
    if (cmdLine.get_num_arguments() < 3) {
      return au::string_in_color("Usage: select_tree <tree> path", "red");
    }

    std::string tree_name = cmdLine.get_argument(1);
    std::string path = cmdLine.get_argument(2);


    TreeItem *tree = trees.findInMap(tree_name);
    if (!tree) {
      return au::string_in_color(au::str("Unknown tree %s", tree_name.c_str()), "red");
    }

    TreeCollection *result = tree->getTreesFromPath(path);

    if (result->getNumTrees() == 0) {
      delete result;
      return au::str(au::red, "No result for path %s over tree %s", tree_name.c_str(), path.c_str());
    }

    TreeItem *result_tree = result->getTree(0);        // Get first tree

    _addTree("result", result_tree);
    return result_tree->str();
  }

  if (mainCommand == "select_collection") {
    if (cmdLine.get_num_arguments() < 4) {
      return au::string_in_color("Usage: select_collection <collection> <new_collection> field=value field=value",
                                 "red");
    }

    std::string collection_name = cmdLine.get_argument(1);
    std::string new_collection_name = cmdLine.get_argument(2);

    Collection *collection = collections.findInMap(collection_name);
    if (!collection) {
      return au::string_in_color(au::str("Unknown collection %s", collection_name.c_str()), "red");
    }

    CollectionItem filter;

    for (int i = 3; i < cmdLine.get_num_arguments(); i++) {
      std::string condition = cmdLine.get_argument(i);
      std::vector<std::string> condition_parts = au::split(condition, '=');
      if (condition_parts.size() != 2) {
        return au::string_in_color(au::str("Wrong condition %s", condition.c_str()), "red");
      }

      filter.add(condition_parts[0], condition_parts[1]);
    }

    Collection *new_collection = collection->getCollection(&filter);
    _addCollection(new_collection_name, new_collection);

    return au::str("Collection %s created with %lu items", new_collection_name.c_str(), new_collection->getNumItems());
  }

  return au::str("Unknown command %s", mainCommand.c_str());
}

StringVector DataBase::getValuesFromColumn(std::string table_name, std::string column_name,
                                           TableSelectCondition *condition) {
  Table *table = tables.findInMap(table_name);

  if (!table) {
    return StringVector();     // empty list since table does not exist
  }
  return table->getValuesFromColumn(column_name, condition);
}

StringVector DataBase::getValuesFromColumn(std::string table_name, std::string column_name) {
  Table *table = tables.findInMap(table_name);

  if (!table) {
    return StringVector();     // empty list since table does not exist
  }
  return table->getValuesFromColumn(column_name);
}

Table *DataBase::getTableOfTables() {
  Table *tmp = new Table(StringVector("name", "#columns", "#rows"));

  tmp->setTitle("Tables");
  au::map<std::string, Table>::iterator it_tables;
  for (it_tables = tables.begin(); it_tables != tables.end(); it_tables++) {
    std::string name = it_tables->first;
    Table *table = it_tables->second;
    tmp->addRow(
      StringVector(name
                   , au::str("%lu", table->getNumColumns())
                   , au::str("%lu", table->getNumRows())
                   )
      );
  }

  return tmp;
}

Table *DataBase::getTableOfTrees() {
  Table *tmp = new Table(StringVector("name", "#nodes", "depth"));

  tmp->setTitle("Trees");
  au::map<std::string, TreeItem>::iterator it_trees;
  for (it_trees = trees.begin(); it_trees != trees.end(); it_trees++) {
    std::string name = it_trees->first;
    TreeItem *tree = it_trees->second;
    tmp->addRow(
      StringVector(name
                   , au::str("%lu", tree->getTotalNumNodes())
                   , au::str("%lu", tree->getMaxDepth())
                   )
      );
  }

  return tmp;
}

Table *DataBase::getTableOfCollections() {
  Table *tmp = new Table(StringVector("name", "#items"));

  tmp->setTitle("Collections");
  au::map<std::string, Collection>::iterator it_collections;
  for (it_collections = collections.begin(); it_collections != collections.end(); it_collections++) {
    std::string name = it_collections->first;
    Collection *collection = it_collections->second;
    tmp->addRow(
      StringVector(name
                   , au::str("%lu", collection->getNumItems())
                   )
      );
  }

  return tmp;
}

Table *DataBase::getErrorTable(int error_code, std::string error_message) {
  Table *tmp = new Table(StringVector("error", "description"));

  tmp->addRow(StringVector(au::str("%d", error_code), au::string_in_color(error_message, "red")));
  return tmp;
}

// Add a table to the collection
void DataBase::_addTable(std::string name, Table *table) {
  tables.removeInMap(name);
  tables.insertInMap(name, table);
}

// Add a table to the collection
void DataBase::_addTree(std::string name, TreeItem *tree) {
  trees.removeInMap(name);
  trees.insertInMap(name, tree);
}

void DataBase::_addCollection(std::string name, Collection *collection) {
  collections.removeInMap(name);
  collections.insertInMap(name, collection);
}

void DataBase::autoCompleteTables(au::ConsoleAutoComplete *info) {
  au::map<std::string, Table>::iterator it_tables;
  for (it_tables = tables.begin(); it_tables != tables.end(); it_tables++) {
    std::string name = it_tables->first;
    info->add(name);
  }
}

void DataBase::autoCompleteTrees(au::ConsoleAutoComplete *info) {
  au::map<std::string, TreeItem>::iterator it_trees;
  for (it_trees = trees.begin(); it_trees != trees.end(); it_trees++) {
    std::string name = it_trees->first;
    info->add(name);
  }
}

void DataBase::autoCompleteCollections(au::ConsoleAutoComplete *info) {
  au::map<std::string, Collection>::iterator it_collections;
  for (it_collections = collections.begin(); it_collections != collections.end(); it_collections++) {
    std::string name = it_collections->first;
    info->add(name);
  }
}

void DataBase::autoCompleteFieldsOfTable(std::string table_name, au::ConsoleAutoComplete *info) {
  Table *table = tables.findInMap(table_name);

  if (table) {
    for (size_t c = 0; c < table->getNumColumns(); c++) {
      info->add(table->getColumn(c));
    }
  }
}

void DataBase::replaceNodeInTree(std::string tree_name, TreeItem *node) {
  TreeItem *tree = trees.findInMap(tree_name);

  if (!tree) {
    tree = new TreeItem("root");
    trees.insertInMap(tree_name, tree);
  }

  tree->replaceNode(node);
}
}
}

