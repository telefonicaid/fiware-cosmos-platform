#ifndef DELILAH_H
#define DELILAH_H

/* ****************************************************************************
 *
 * FILE                     Delilah.h
 *
 * DESCRIPTION			   Client application for Samson
 *
 */

#include <iostream>                          // std::cout
#include <set>                               // std::set
#include <string>
#include <vector>

#include "logMsg/logMsg.h"                   // lmInit, LM_*

#include "au/CommandLine.h"                  // au::CommandLine
#include "au/CounterCollection.h"            // au::CounterCollection
#include "au/Cronometer.h"      // au::Cronometer
#include "au/containers/map.h"               // au::map
#include "au/mutex/Token.h"                  // au::Token
#include "au/mutex/TokenTaker.h"             // au::TokenTaker
#include "au/string.h"                       // au::Table

#include "au/tables/pugi.h"                  // pugi::...

#include "engine/Buffer.h"
#include "engine/MemoryManager.h"
#include "engine/NotificationListener.h"     // engine::NotificationListener

#include "samson/common/Macros.h"            // EXIT, ...
#include "samson/common/samson.pb.h"         // samson::network::..

#include "samson/module/Environment.h"  // samson::Environment

#include "samson/network/DelilahNetwork.h"
#include "samson/network/Message.h"          // Message::MessageCode
#include "samson/network/NetworkInterface.h"  // NetworkInterface

#include "DelilahBase.h"                     // Monitorization information for delilah
#include "DelilahBaseConnection.h"
#include "PushManager.h"

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
  // Random identifier for this delilah
  size_t delilah_id_;

  // Id counter of all internal DelilahComponents
  size_t id;

  // Private token to protect the local list of components
  au::Token token;

  // last commit observed
  int last_commit_version_;

  friend class SamsonClient;

protected:

  // Map of components that intercept messages
  au::map<size_t, DelilahComponent> components_;

  // Manager of the push items
  au::SharedPointer<PushManager> push_manager;

public:

  // Interface to receive live data
  DelilahLiveDataReceiverInterface *data_receiver_interface;

  // Network connection ( with workers )
  DelilahNetwork *network;

  Environment environment;                      // Environment properties to be sent in the next job

  Delilah(std::string connection_type, size_t delilah_id = (size_t)-1);
  ~Delilah();

  // Stop all threads ( just before calling delete )
  void stop() {
    network->ClearConnections();
  }

  // Connect and disconnect to a cluster
  bool connect(std::string host, au::ErrorManager *error);      // Return true if it was possible to connect
  void disconnect();


  // Connect to a queue
  size_t connect_to_queue(const std::string queue) {
    return AddPopComponent(queue, "", false, false);
  }

  // Check if we are really connected to a cluster
  bool isConnected();

  // Get a a line with information about connection
  std::string getClusterConnectionSummary();

  // Notification system
  void notify(engine::Notification *notification);

  // PacketReceiverInterface
  void receive(const PacketPointer& packet);

  // PacketSenderInterface
  virtual void notificationSent(size_t id, bool success);

  // Add particular process that will take input parameters
  size_t push_txt(engine::BufferPointer buffer, const std::string& queues);
  size_t push_txt(engine::BufferPointer buffer, const std::vector<std::string>& queues);
  size_t push(engine::BufferPointer buffer, const std::vector<std::string>& queues);
  size_t get_num_push_items();

  // Add a push component
  size_t add_push_component(const std::vector<std::string>& file_names, const std::vector<std::string>& queues);
  size_t add_push_component(DataSource *data_source, const std::vector<std::string>& queues, bool module = false);

  // Add a push module component
  size_t add_push_module_component(const std::vector<std::string>& file_names);

  size_t AddPopComponent(std::string queue_name, std::string fileName, bool force_flag, bool show_flag);

  // Add a worker command
  size_t sendWorkerCommand(std::string command, engine::BufferPointer buffer  = engine::BufferPointer(NULL));

  // Get delilah_id

  size_t get_delilah_id() {
    return delilah_id_;
  }

  // Check a particular id
  bool isActive(size_t id);
  bool hasError(size_t id);
  std::string errorMessage(size_t id);
  std::string getDescription(size_t id);

public:

  /**
   * Methonds implemented by subclasses
   */

  // Function to be implemented by sub-classes to process packets ( not handled by this class )
  virtual int _receive(const PacketPointer& packet);

  // Notification form a delilah component
  virtual void delilahComponentStartNotification(DelilahComponent *component) {
    if (component == NULL) {
      return;
    }
  };
  virtual void delilahComponentFinishNotification(DelilahComponent *component) {
    if (component == NULL) {
      return;
    }
  };

  // Write something on screen
  virtual void showMessage(std::string message) {
    LM_D(("not implemented (%s)", message.c_str()));
  };
  virtual void showWarningMessage(std::string message) {
    LM_D(("not implemented (%s)", message.c_str()));
  };
  virtual void showErrorMessage(std::string message) {
    LM_D(("not implemented (%s)", message.c_str()));
  };

  // Show traces  ( by default it does nothing )
  virtual void showTrace(std::string message) {
    LM_D(("not implemented (%s)", message.c_str()));
  };

  // Callback to notify that a particular operation has finished
  virtual void notifyFinishOperation(size_t) {
  }

  void PublishBufferFromQueue(std::string queue, engine::BufferPointer buffer) {
    if (data_receiver_interface) {
      data_receiver_interface->receive_buffer_from_queue(queue, buffer);
    } else {
      receive_buffer_from_queue(queue, buffer);
    }
  }

  virtual void receive_buffer_from_queue(std::string queue, engine::BufferPointer buffer) {
    showWarningMessage(au::str("Buffer with %s recevied for queue %s. Ignored..", buffer->str().c_str(), queue.c_str()));
  }

  // Get info about the list of loads
  std::string getListOfComponents();

  // Recover a particular component
  DelilahComponent *getComponent(size_t delilah_id);

public:

  void clearComponents();
  void clearAllComponents();    // Force all of them to be removed

  /*
   * Status stop_repeat( size_t id );
   * Status stop_all_repeat(  );
   */

  // Get a list of local directory
  std::string getLsLocal(std::string pattern, bool only_queues);

  // Generate XML monitorization data
  void getInfo(std::ostringstream& output);

  bool checkXMLInfoUpdate();


  // Cancel a particuarl delilah_id
  void cancelComponent(size_t id);
  void setBackgroundComponent(size_t id);
  std::string getOutputForComponent(size_t id);

protected:

  size_t addComponent(DelilahComponent *component);
};
}

#endif  // ifndef DELILAH_H
