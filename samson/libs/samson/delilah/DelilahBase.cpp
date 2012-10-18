
/* ****************************************************************************
 *
 * FILE                     DelilahBase.cpp
 *
 * DESCRIPTION			   Basic stuff for delilah client
 *
 */

#include "au/containers/StringVector.h"
#include "au/mutex/TokenTaker.h"      // au::TokenTaker
#include "au/string/xml.h"             // au::xml...

#include "au/tables/Select.h"

#include "samson/common/coding.h"

#include "DelilahBase.h"        // Own interface

#define CREATE_TABLE_QUEUES     \
  "table_from_tree workers /*//stream_manager/queues/queue -save queues "

#define CREATE_TABLE_OPERATIONS \
  "table_from_tree workers /*//modules_manager/modules/module/operations/operation -save operations"

#define CREATE_TABLE_MODULES    \
  "table_from_tree workers /*//modules_manager/modules/module -save modules"

#define CREATE_TABLE_ENGINES    \
  "table_from_tree workers /*/engine_system -save engines"

#define CREATE_TABLE_ENGINE_DELILAH    \
  "table_from_tree delilah /engine_system -save engine_delilah"

#define CREATE_TABLE_STREAM_OPERATIONS \
  "table_from_tree workers /*/stream_manager/stream_operations/stream_operation -save stream_operations"

#define CREATE_TABLE_BLOCKS \
  "table_from_tree  workers /*//block_manager/blocks/block -save blocks"

#define CREATE_TABLE_QUEUE_TASKS \
  "table_from_tree workers /*/stream_manager/queue_tasks/queue_tasks/queue_task -save tasks"

namespace samson {
DelilahBase::DelilahBase() {
}

DelilahBase::~DelilahBase() {
  workers.clearMap();
}

std::vector<std::string> DelilahBase::getOperationNames() {
  au::StringVector values = database.getValuesFromColumn("operations", "name");

  values.RemoveDuplicated();
  return values;
}

std::vector<std::string> DelilahBase::getOperationNames(std::string type) {
  au::tables::TableSelectCondition condition("type", type);
  au::StringVector values = database.getValuesFromColumn("operations", "name", &condition);

  values.RemoveDuplicated();
  return values;
}

std::vector<std::string> DelilahBase::getQueueNames() {
  au::StringVector values = database.getValuesFromColumn("queues", "name");

  values.RemoveDuplicated();
  return values;
}

std::vector<std::string> DelilahBase::getQueueNames(KVFormat format) {
  if (format.isTxt()) {
  }

  // Get all the values
  au::StringVector values = database.getValuesFromColumn("queues", "name");

  values.RemoveDuplicated();
  return values;
}

void DelilahBase::updateWorkerXMLString(size_t worker_id, std::string txt) {
  // Find the xml holder
  XMLStringHolder *xmlStringHolder = workers.findOrCreate(worker_id);

  au::tables::TreeItem *tree = xmlStringHolder->update(txt);

  // Set the main name of this tree
  tree->setValue(au::str("worker_%lu", worker_id));

  // Add some markers
  tree->add("tag_name", "worker_id");
  tree->add("tag_value", au::str("%lu", worker_id));

  // Replace node int he workers tree
  database.replaceNodeInTree("workers", tree);

  // Create the list of queues from tree
  database.runCommand(CREATE_TABLE_QUEUES);
}

void DelilahBase::updateDelilahXMLString(std::string txt) {
  au::tables::TreeItem *tree = delilah->update(txt);

  database.addTree("delilah", tree);
}

std::string DelilahBase::runDatabaseCommand(std::string command) {
  return database.runCommand(command);
}

// Autocomplete for database mode
void DelilahBase::autoCompleteForDatabaseCommand(au::ConsoleAutoComplete *info) {
  database.autoComplete(info);
}

std::string DelilahBase::updateTimeString() {
  std::ostringstream output;

  output << "Update times from SAMSON elements\n";
  output << "-------------------------------------------------\n\n";


  au::map<size_t, XMLStringHolder >::iterator it_workers;
  for (it_workers = workers.begin(); it_workers != workers.end(); it_workers++) {
    output << "Worker " << au::str("%lu", it_workers->first);
    output << " updated " << au::str_time(it_workers->second->getTime()) << "\n";
  }

  output << "-------------------------------------------------\n";

  return output.str();
}
}


