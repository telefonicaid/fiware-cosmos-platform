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
#ifndef DELILAH_H
#define DELILAH_H

/* ****************************************************************************
 *
 * FILE                     Delilah.h
 *
 * DESCRIPTION			   Client application for Samson
 *
 */

#include <iostream>                        // std::cout
#include <set>                             // std::set
#include <string>
#include <vector>

#include "logMsg/logMsg.h"                 // lmInit, LM_*

#include "au/CommandLine.h"                // au::CommandLine
#include "au/containers/map.h"             // au::map
#include "au/mutex/Token.h"                // au::Token
#include "au/mutex/TokenTaker.h"           // au::TokenTaker
#include "au/statistics/CounterCollection.h"  // au::CounterCollection
#include "au/statistics/Cronometer.h"      // au::Cronometer
#include "au/string/StringUtilities.h"       // au::Table
#include "au/tables/pugi.h"                // pugi::...

#include "engine/Buffer.h"
#include "engine/MemoryManager.h"
#include "engine/NotificationListener.h"     // engine::NotificationListener

#include "samson/common/Logs.h"            // EXIT, ...
#include "samson/common/Macros.h"          // EXIT, ...
#include "samson/delilah/DelilahBase.h"    // Monitorization information for delilah
#include "samson/delilah/DelilahBaseConnection.h"
#include "samson/delilah/PushManager.h"
#include "samson/module/Environment.h"  // samson::Environment
#include "samson/network/DelilahNetwork.h"
#include "samson/network/Message.h"        // Message::MessageCode
#include "samson/network/NetworkInterface.h"  // NetworkInterface

namespace  engine {
class Buffer;
}

namespace samson {
class Delilah;
class DelilahClient;
class DelilahComponent;
class PushDelilahComponent;
class PopDelilahComponent;
class DataSource;

// Interface to receive live data
class DelilahLiveDataReceiverInterface {
public:

  virtual void receive_buffer_from_queue(std::string queue, engine::BufferPointer buffer) = 0;
};

/**
 * Main class for the samson client element
 */

class Delilah : public engine::NotificationListener, public DelilahBase {
public:

  Delilah(std::string connection_type, size_t delilah_id = static_cast<size_t>(-1));
  ~Delilah();

  // Connect and disconnect to a cluster
  bool connect(std::string host, au::ErrorManager *error);
  void disconnect();
  bool isConnected();  // Check if we are really connected to a cluster

  // Connect to a queue
  size_t connect_to_queue(const std::string queue) {
    return AddPopComponent(queue, "", false, false);
  }

  // Get a a line with information about connection
  std::string getClusterConnectionSummary();

  // Notification system
  void notify(engine::Notification *notification);

  // PacketReceiverInterface
  void receive(const PacketPointer& packet);

  // Push data to SAMSON
  size_t push_txt(engine::BufferPointer buffer, const std::string& queues);
  size_t push_txt(engine::BufferPointer buffer, const std::vector<std::string>& queues);
  size_t push(engine::BufferPointer buffer, const std::vector<std::string>& queues);

  /**
   * \brief Get pending data size to be "pushed"
   */
  size_t GetPendingSizeToPush();

  // Add a push component
  size_t add_push_component(const std::vector<std::string>& file_names, const std::vector<std::string>& queues,
                            au::ErrorManager& error);
  size_t add_push_component(DataSource *data_source, const std::vector<std::string>& queues, bool module,
                            au::ErrorManager& error);

  // Add a push module component
  size_t add_push_module_component(const std::vector<std::string>& file_names, au::ErrorManager& error);

  size_t AddPopComponent(std::string queue_name, std::string fileName, bool force_flag, bool show_flag);

  // Add a worker command
  size_t sendWorkerCommand(std::string command, engine::BufferPointer buffer  = engine::BufferPointer(NULL));

  // Get info about the list of loads
  std::string getListOfComponents();

  // Recover a particular component
  DelilahComponent *getComponent(size_t delilah_id);

  void clearComponents();
  void clearAllComponents();    // Force all of them to be removed


  /**
   * \brief Get internal delilah identifier
   */
  size_t delilah_id() const {
    return delilah_id_;
  }

  // Check a particular id
  bool isActive(size_t id);
  bool hasError(size_t id);
  std::string errorMessage(size_t id);
  std::string getDescription(size_t id);


  /**
   * \brief Handle packets not managed by delilah components
   */
  virtual int _receive(const PacketPointer& packet);

  /**
   * \brief Nofication that a delilah component has started
   */
  virtual void delilahComponentStartNotification(DelilahComponent *component) {
  };

  /**
   * \brief Nofication that a delilah component has finished
   */
  virtual void delilahComponentFinishNotification(DelilahComponent *component) {
  };

  // Write something on screen
  virtual void WriteOnDelilah(const std::string& message) {
    LOG_M(logs.delilah, ("%s", message.c_str()));
  }

  virtual void WriteWarningOnDelilah(const std::string& message) {
    LOG_W(logs.delilah, ("%s", message.c_str()));
  }

  virtual void WriteErrorOnDelilah(const std::string& message) {
    LOG_E(logs.delilah, ("%s", message.c_str()));
  }

  void PublishBufferFromQueue(std::string queue, engine::BufferPointer buffer) {
    if (data_receiver_interface) {
      data_receiver_interface->receive_buffer_from_queue(queue, buffer);
    } else {
      receive_buffer_from_queue(queue, buffer);
    }
  }

  virtual void receive_buffer_from_queue(std::string queue, engine::BufferPointer buffer) {
    LOG_W(logs.delilah, ("Buffer with %s recevied for queue %s. Ignored..", buffer->str().c_str(), queue.c_str()));
  }

  // Get a list of local directory
  std::string getLsLocal(std::string pattern, bool only_queues);

  // Generate XML monitorization data
  void getInfo(std::ostringstream& output);

  bool checkXMLInfoUpdate();


  // Cancel a particuarl delilah_id
  void cancelComponent(size_t id, au::ErrorManager& error);
  std::string GetOutputForComponent(size_t id);

  DelilahNetwork *network_;   /**< Network connection ( with workers ) */
  Environment environment_;   /**< Environment properties to be sent in the next job */

protected:

  /**
   * \brief Add a component to this delilah
   */
  size_t AddComponent(DelilahComponent *component);

  DelilahLiveDataReceiverInterface *data_receiver_interface;  /**< Interface to receive live data */
  au::map<size_t, DelilahComponent> components_;             /**< Map of components that intercept messages */
  au::SharedPointer<PushManager> push_manager_;              /**< Manager of data-blocks being pushed to SAMSON  */

private:

  friend class SamsonClient;

  // Random identifier for this delilah
  size_t delilah_id_;

  // Id counter of all internal DelilahComponents
  size_t next_delilah_component_id;

  // Private token to protect the local list of components
  au::Token token_;

  // last commit observed
  int last_commit_version_;
};
}

#endif  // ifndef DELILAH_H
