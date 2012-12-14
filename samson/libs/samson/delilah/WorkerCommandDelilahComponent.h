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

#ifndef _H_WORKER_COMMAND_DELILAH_COMPONENT
#define _H_WORKER_COMMAND_DELILAH_COMPONENT

#include <cstring>
#include <map>

#include "au/ErrorManager.h"          // au::ErrorManager
#include "au/console/CommandCatalogue.h"
#include "au/containers/SharedPointer.h"
#include "au/statistics/Cronometer.h"  // au::CronometerSystem


#include "samson/delilah/DelilahComponent.h"
#include "samson/network/Message.h"  // Message::MessageCode
#include "samson/network/Packet.h"  // samson::Packet

namespace engine {
class Buffer;
}

namespace samson {
class Delilah;


/**
 * Simple component created when a command is send to the controller ( waiting for answeres )
 */

class WorkerResponese {
public:

  WorkerResponese(size_t _worker_id) {
    worker_id_ = _worker_id;
  }

  WorkerResponese(size_t _worker_id, std::string error_message) {
    worker_id_ = _worker_id;
    error_.AddError(error_message);
  }

  size_t worker_id() {
    return worker_id_;
  }

  au::ErrorManager& error() {
    return error_;
  }

private:

  size_t worker_id_;
  au::ErrorManager error_;
};

class WorkerCommandDelilahComponent : public DelilahComponent {
public:

  WorkerCommandDelilahComponent(std::string _command, engine::BufferPointer buffer);
  ~WorkerCommandDelilahComponent();

  // Function to receive packets ( thougth Delilah )
  void receive(const PacketPointer& packet);

  // Main command to start this component
  void run();

  // DelilahComponent virtual methods
  virtual std::string getStatus();
  virtual std::string getExtraStatus();

  // Get main table of result ( to show on screen )
  au::tables::Table *getMainTable();

private:

  // Internal function to print content of received collection
  void print_content(au::SharedPointer<gpb::Collection> collection);

  // Transform a collection into a table
  au::tables::Table *getTable(au::SharedPointer<gpb::Collection> collection);

  std::string command;

  // Instance of the command parsed from input command
  au::SharedPointer<au::console::CommandInstance> command_instance_;

  engine::BufferPointer buffer_;

  std::set<size_t> workers;                         // Ids of the workers involved in this command
  au::map<size_t, WorkerResponese > responses;      // Map with all the responses from workers

  // Collections reported by workers
  std::map<std::string, au::SharedPointer<gpb::Collection> > collections;

  std::string main_command;

  size_t worker_id;                 // if != -1 --> worker to sent this command
  bool send_to_all_workers;         // -a
  bool save_in_database;            // -save
  std::string group_field;          // -group
  std::string filter_field;         // -filter
  std::string sort_field;           // -sort
  bool connected_workers;           // -connected
  int limit;                        // -limit
};
}

#endif  // ifndef _H_WORKER_COMMAND_DELILAH_COMPONENT
