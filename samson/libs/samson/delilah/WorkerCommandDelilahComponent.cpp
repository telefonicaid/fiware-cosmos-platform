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
#include "samson/delilah/WorkerCommandDelilahComponent.h"      // Own interface

#include <utility>                               // std::pair

#include "au/CommandLine.h"                      // samson::CommandLine
#include "au/containers/Uint64Vector.h"

#include "engine/Buffer.h"                       // engine::Buffer

#include "samson/common/EnvironmentOperations.h"  // copyEnviroment()
#include "samson/common/MessagesOperations.h"
#include "samson/delilah/Delilah.h"              // samson::Delilah
#include "samson/delilah/DelilahCommandCatalogue.h"
#include "samson/delilah/DelilahComponent.h"
#include "samson/network/Packet.h"               // samson::Packet

namespace samson {
WorkerCommandDelilahComponent::WorkerCommandDelilahComponent(std::string _command, engine::BufferPointer buffer)
  : DelilahComponent(DelilahComponent::worker_command) {
  command = _command;
  buffer_ = buffer;

  // Remove the last return
  while (command.substr(command.size() - 1) == "\n") {
    command.erase(command.size() - 1, 1);
  }

  setConcept(command);        // The command is the description itself

  DelilahCommandCatalogue catalogue;

  au::ErrorManager error;
  command_instance_ = catalogue.parse(command, error);

  send_to_all_workers = command_instance_->command()->tag("send_to_all_workers");

  if (error.IsActivated()) {
    setComponentFinishedWithError(error.GetMessage());
    return;
  }


  // Old parsing stuff
  au::CommandLine cmdLine;
  cmdLine.SetFlagBoolean("hidden");
  cmdLine.SetFlagBoolean("save");       // Flag to identify if is necessary to save it locally
  cmdLine.SetFlagBoolean("connected_workers");      // Flag to run the operation only with connected workers
  cmdLine.SetFlagUint64("w", static_cast<size_t>(-1));
  cmdLine.SetFlagString("group", "");
  cmdLine.SetFlagString("filter", "");
  cmdLine.SetFlagString("sort", "");
  cmdLine.SetFlagUint64("limit", 0);
  cmdLine.Parse(command);

  worker_id         = cmdLine.GetFlagUint64("w");
  hidden            = cmdLine.GetFlagBool("hidden");
  save_in_database  = cmdLine.GetFlagBool("save");
  group_field       = cmdLine.GetFlagString("group");
  filter_field      = cmdLine.GetFlagString("filter");
  sort_field        = cmdLine.GetFlagString("sort");
  connected_workers = cmdLine.GetFlagBool("connected_workers");
  limit             = cmdLine.GetFlagUint64("limit");


  if (cmdLine.get_num_arguments() > 0) {
    main_command = cmdLine.get_argument(0);
  }
}

WorkerCommandDelilahComponent::~WorkerCommandDelilahComponent() {
  responses.clearMap();
  collections.clear();
  if (command_instance_) {
    delete command_instance_;
  }
}

void WorkerCommandDelilahComponent::run() {
  // Get a random worker id to send the command (excep command that requires all workers involved )
  if ((worker_id == static_cast<size_t>(-1)) && (!send_to_all_workers)) {
    worker_id = delilah->network->getRandomWorkerId();
    if (worker_id == static_cast<size_t>(-1)) {
      setComponentFinishedWithError("It seems there is no samson worker up in this cluster");
    }
  }

  // Errors during contructor or selecting worker
  if (error.IsActivated()) {
    setComponentFinished();
    return;
  }

  if (!delilah->isConnected()) {
    setComponentFinishedWithError("This delilah is not connected to any SAMSON cluster");
    return;
  }

  // Prepare packet to be sent to all workers
  // ------------------------------------------

  PacketPointer p(new Packet(Message::WorkerCommand));
  gpb::WorkerCommand *c = p->message->mutable_worker_command();
  c->set_command(command);
  p->message->set_delilah_component_id(id);
  copyEnviroment(&delilah->environment, c->mutable_environment());

  // Set buffer to be sent
  p->set_buffer(buffer_);

  if (worker_id != static_cast<size_t>(-1)) {
    p->to = NodeIdentifier(WorkerNode, worker_id);
    workers.insert(worker_id);
    delilah->network->Send(p);
  } else {
    delilah->network->SendToAllWorkers(p, workers);    // Send to all workers
  }
}

void WorkerCommandDelilahComponent::receive(const PacketPointer& packet) {
  if (packet->from.node_type != WorkerNode) {
    return;
  }

  size_t worker_id = packet->from.id;

  if (packet->msgCode == Message::WorkerCommandResponse) {
    if (workers.find(worker_id) == workers.end()) {
      LM_W(("WorkerCommandResponse received from worker %lu not involved in this operation. Ignoring...", worker_id));
      return;
    }

    if (responses.findInMap(worker_id) != NULL) {
      LM_W(("Duplicated WorkerCommandResponse received from worker %lu.Ignoring...", worker_id));
      return;
    }

    // If error is returned, worker_command is automatically canceled
    if (packet->message->worker_command_response().has_error()) {
      std::string error_message = packet->message->worker_command_response().error().message();
      WorkerResponese *response = new WorkerResponese(worker_id, error_message);
      responses.insertInMap(worker_id, response);
    } else {
      WorkerResponese *response = new WorkerResponese(worker_id);
      responses.insertInMap(worker_id, response);
    }
  }

  // Extract collections if included ( adding worker_id field )
  for (int i = 0; i < packet->message->collection_size(); i++) {
    std::string name = packet->message->collection(i).name();
    std::string title = packet->message->collection(i).title();

    au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
    std::map< std::string, au::SharedPointer<gpb::Collection> >::iterator it_collection = collections.find(name);
    if (it_collection != collections.end()) {
      collection = it_collection->second;
    } else {
      collections[name] = collection;
    }
    collection->set_name(name);
    collection->set_title(title);

    for (int j = 0; j < packet->message->collection(i).record_size(); j++) {
      gpb::CollectionRecord *record = collection->add_record();

      // Add worker_id if multiple workers are involved in the message
      if (send_to_all_workers) {
        ::samson::add(record, "worker_id", worker_id, "different");
      }

      // Add all elements in the sent collection item
      for (int k = 0; k < packet->message->collection(i).record(j).item_size(); k++) {
        record->add_item()->CopyFrom(packet->message->collection(i).record(j).item(k));
      }
    }
  }

  if (responses.size() == workers.size()) {
    bool general_error = false;
    std::string general_error_message;
    au::map<size_t, WorkerResponese >::iterator it_responses;
    for (it_responses = responses.begin(); it_responses != responses.end(); it_responses++) {
      size_t worker_id = it_responses->first;
      WorkerResponese *response = it_responses->second;
      if (response->error().IsActivated()) {
        general_error_message.append(au::str("[Worker %lu] ", worker_id) + response->error().GetMessage() + "\n");
        general_error = true;
      }
    }

    // Print the result in a table if necessary
    std::map<std::string, au::SharedPointer<gpb::Collection> >::iterator it;
    for (it = collections.begin(); it != collections.end(); it++) {
      print_content(it->second);
    }

    if (general_error) {
      setComponentFinishedWithError(general_error_message);
    } else {
      setComponentFinished();
    }
  }
}

au::tables::Table *WorkerCommandDelilahComponent::getMainTable() {
  if (collections.size() == 0) {
    return NULL;
  }

  au::SharedPointer<gpb::Collection> collection = collections.begin()->second;

  if (collection->record_size() == 0) {
    return NULL;
  }

  // Get the first one
  return getTable(collection);
}

au::tables::Table *WorkerCommandDelilahComponent::getTable(au::SharedPointer<gpb::Collection> collection) {
  au::SharedPointer<au::tables::Table> table = GetTableFromCollection(collection);

  // Select the table with the common criteria
  std::string title = collection->title();
  au::tables::Table *selected_table = table->selectTable(title, group_field, sort_field, filter_field, limit);

  return selected_table;
}

void WorkerCommandDelilahComponent::print_content(au::SharedPointer<gpb::Collection> collection) {
  if (collection->record_size() == 0) {
    if (!hidden) {
      delilah->showWarningMessage("No records\n");
    }
    return;
  }

  au::tables::Table *table = getTable(collection);

  if (!hidden) {
    std::string title = collection->title();
    output << table->str();
  }

  // Save in the internal database
  if (save_in_database) {
    std::string table_name = collection->name();

    delilah->database.addTable(table_name, table);

    if (!hidden) {
      delilah->showWarningMessage(
        au::str("Table %s has been created locally. Type set_database_mode to check content...\n"
                , table_name.c_str())
        );
    }
  }
}

std::string WorkerCommandDelilahComponent::getStatus() {
  std::ostringstream output;

  output << "Command sent to workers [";
  std::set<size_t>::iterator it;
  for (it = workers.begin(); it != workers.end(); it++) {
    output << *it  << " ";
  }
  output << "] ";

  output << "Responses received from workers [";
  au::map<size_t, WorkerResponese >::iterator it_responses;
  for (it_responses = responses.begin(); it_responses != responses.end(); it_responses++) {
    output << it_responses->first << " ";
  }

  output << "]";

  return output.str();
}

std::string WorkerCommandDelilahComponent::getExtraStatus() {
  au::tables::Table table("Worker,left|Status,left|Message,left");

  std::set<size_t>::iterator it;
  for (it = workers.begin(); it != workers.end(); it++) {
    size_t worker_id = *it;

    au::StringVector values;
    values.push_back(au::str("%lu", worker_id));

    WorkerResponese *response = responses.findInMap(worker_id);
    if (!response) {
      values.push_back("Pending");
      values.push_back("");
    } else {
      if (response->error().IsActivated()) {
        values.push_back("Error");
        values.push_back(response->error().GetMessage());
      } else {
        values.push_back("Finish");
        values.push_back("OK");
      }
    }
    table.addRow(values);
  }

  table.setTitle("Responses from SAMSON workers");
  return table.str();
}
}

