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

class Delilah : public engine::NotificationListener {
public:

  Delilah(std::string connection_type, size_t delilah_id = static_cast<size_t>(-1));
  ~Delilah();

  /**
   * \brief Try to connect with a SAMSON cluster. Error is reported if not possible
   */
  bool Connect(std::string host, au::ErrorManager *error);
  
  /**
   * \brief Disconnect from SAMSON cluster
   */
  void Disconnect();
  
  /**
   * \brief Check if this delilah is already connected
   */
  bool IsConnected() const;

  /**
   * \brief Connect to a queue to receive live data from this queue
   */
  size_t ConnectToQueue(const std::string queue) {
    return AddPopComponent(queue, "", false, false);
  }

  /**
   * \brief Get a string with information about connection ( used in prompt for DelilahConsole)
   */
  std::string GetClusterConnectionSummary();

  /**
   * \brief Process a notification from engine system
   */
  virtual void notify(engine::Notification *notification);

  /**
   * \brief Process a packet received from other nodes ( SAMSON workers )
   */
  void ProcessIncomePacket(const PacketPointer& packet);

  /**
   * \brief Push a buffer with plain data to a queue ( expected to be a txt-txt queue )
   */
  size_t PushPlainData(engine::BufferPointer buffer, const std::string& queues);

  /**
   * \brief Push a buffer with plain data to a set of queue ( expected all of them to be txt-txt queues )
   */
  size_t PushPlainData(engine::BufferPointer buffer, const std::vector<std::string>& queues);
  
  /**
   * \brief Push a SAMSON block ( KVHeader + data ) to some queues
   */
  size_t PushSamsonBlock(engine::BufferPointer buffer, const std::vector<std::string>& queues);

  /**
   * \brief Get pending data size to be "pushed"
   */
  size_t GetPendingSizeToPush();

  /**
   * \brief Add a PushDelilahComponent to push some files to a queue
   */
  size_t AddPushComponent(const std::vector<std::string>& file_names,
                          const std::vector<std::string>& queues,
                          au::ErrorManager& error);
  
  /**
   * \brief Add a PushDelilahComponent to push data to some queues from a generic "data source"
   */
  size_t AddPushComponent(DataSource *data_source,
                          const std::vector<std::string>& queues,
                          bool module,
                          au::ErrorManager& error);

  /**
   * \brief Add a PushDelilahComponent to push a module from a file
   */
  size_t AddPushModuleComponent(const std::vector<std::string>& file_names, au::ErrorManager& error);

  /**
   * \brief Add a PopDelilahComponent to pop data deom a queue and push content to a file
   */
  size_t AddPopComponent(std::string queue_name, std::string fileName, bool force_flag, bool show_flag);

  /**
   * \brief Add a WorkerCommandDelilahComponent to send a command to SAMSON cluster ( one or all workers )
   */
  size_t SendWorkerCommand(std::string command, engine::BufferPointer buffer  = engine::BufferPointer(NULL));

  /**
   * \brief Get a string with the list of all components in this delilah
   */
  std::string GetListOfComponents();

  /**
   * \brief Get a particular delilah component from its id
   */
  DelilahComponent *GetComponent(size_t delilah_id);

  /**
   * \brief Remove finished delilah components ( with or without error )
   */
  void ClearFinishedComponents();
  
  /**
   * \brief Remove all delilah components ( finished or not )
   */
  void ClearComponents();    // Force all of them to be removed

  /**
   * \brief Get internal delilah identifier
   */
  size_t delilah_id() const {
    return delilah_id_;
  }

  /**
   * \brief Check if a particular delilah component is still active ( not finished )
   */
  bool DelilahComponentIsActive(size_t id);
  
  /**
   * \brief Check if a particular delilah component has finihsed with error
   */
  bool DelilahComponentHasError(size_t id);

  /**
   * \brief Get output generated by a particular delilah component
   */
  std::string GetOutputForComponent(size_t id);
  
  /**
   * \brief Get the error for a particular delilah component
   */
  std::string GetErrorForDelilahComponent(size_t id);
  
  /**
   * \brief Get a description for a particular delilah component
   */
  std::string GetDescriptionForDelilahComponent(size_t id);

  /**
   * \brief Nofication that a delilah component has started
   */
  virtual void DelilahComponentStartNotification(DelilahComponent *component) {};

  /**
   * \brief Nofication that a delilah component has finished
   */
  virtual void DelilahComponentFinishNotification(DelilahComponent *component) {};

  /**
   * \brief Handle packets not managed by delilah components
   */
  virtual int _receive(const PacketPointer& packet);
  
  
  /**
   * \brief Write something
   */
  virtual void WriteOnDelilah(const std::string& message) {
    LOG_M(logs.delilah, ("%s", message.c_str()));
  }

  /**
   * \brief Write a warning
   */
  virtual void WriteWarningOnDelilah(const std::string& message) {
    LOG_W(logs.delilah, ("%s", message.c_str()));
  }

  /**
   * \brief Write an error
   */
  virtual void WriteErrorOnDelilah(const std::string& message) {
    LOG_E(logs.delilah, ("%s", message.c_str()));
  }

  /**
   * \brief Default implementation to handle live data from SAMSON cluter
   */
  virtual void receive_buffer_from_queue(std::string queue, engine::BufferPointer buffer) {
    LOG_W(logs.delilah, ("Buffer with %s recevied for queue %s. Ignored..", buffer->str().c_str(), queue.c_str()));
  }

  /**
   * \brief Get a list of local directory
   */
  std::string GetLsLocal(std::string pattern, bool only_queues);

  /**
   * \brief Cancel a particuarl delilah_id
   */
  void CancelDelilahComponent(size_t id, au::ErrorManager& error);

  DelilahNetwork *network_;   /**< Network connection ( with workers ) */
  Environment environment_;   /**< Environment properties to be sent in the next job */

protected:

  /**
   * \brief Add a component to this delilah
   */
  size_t AddComponent(DelilahComponent *component);

  DelilahLiveDataReceiverInterface *data_receiver_interface;  /**< Interface to receive live data */
  au::map<size_t, DelilahComponent> components_;              /**< Map of components that intercept messages */
  au::SharedPointer<PushManager> push_manager_;               /**< Manager of data-blocks being pushed to SAMSON  */

private:

  friend class SamsonClient;
  friend class PopDelilahComponent;
  
  /**
   * \brief Publish data received from a queue ( in a pop component )
   */
  void PublishBufferFromQueue(std::string queue, engine::BufferPointer buffer) {
    if (data_receiver_interface) {
      data_receiver_interface->receive_buffer_from_queue(queue, buffer);
    } else {
      receive_buffer_from_queue(queue, buffer);
    }
  }

  
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
