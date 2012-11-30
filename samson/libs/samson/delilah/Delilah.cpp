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
#include "samson/delilah/Delilah.h"    // Own interfce

#include <dirent.h>                    // DIR directory header
#include <fnmatch.h>
#include <iomanip>
#include <iostream>                    // std::cout ...
#include <map>
#include <sys/stat.h>                  // stat(.)

#include "au/CommandLine.h"            // CommandLine
#include "au/log/LogMain.h"
#include "au/mutex/TokenTaker.h"       // au::TokenTake
#include "au/statistics/Cronometer.h"      // au::Cronometer
#include "au/tables/Table.h"
#include "engine/Buffer.h"      // engine::Buffer
#include "engine/DiskManager.h"
#include "engine/DiskOperation.h"
#include "engine/Notification.h"    // engine::Notificaiton
#include "logMsg/logMsg.h"             // lmInit, LM_*
#include "samson/common/EnvironmentOperations.h"
#include "samson/common/Logs.h"
#include "samson/common/Macros.h"      // EXIT, ...
#include "samson/common/NotificationMessages.h"  // notification_network_diconnected
#include "samson/common/SamsonDataSet.h"       // samson::SamsonDataSet
#include "samson/common/SamsonSetup.h"  // samson::SamsonSetup
#include "samson/delilah/DataSource.h"
#include "samson/delilah/DelilahConsole.h"      // samson::DelilahConsole
#include "samson/delilah/PopDelilahComponent.h"
#include "samson/delilah/PushDelilahComponent.h"
#include "samson/delilah/WorkerCommandDelilahComponent.h"      // samson::WorkerCommandDelilahComponent
#include "samson/module/ModulesManager.h"       // samson::ModulesManager
#include "samson/network/DelilahNetwork.h"
#include "samson/network/Message.h"           // Message::MessageCode, ...
#include "samson/network/NetworkInterface.h"  // NetworkInterface
#include "samson/network/Packet.h"            // samson::Packet

namespace samson {
/* ****************************************************************************
 *
 * Delilah::Delilah
 */
Delilah::Delilah(std::string connection_type, size_t delilah_id) :
  token("Delilah_token") {
  // Random identifier for this delilah
  if (delilah_id == static_cast<size_t>(-1)) {
    delilah_id_ = au::code64_rand();
  } else {
    delilah_id_ = delilah_id;   // Network interface for all the workers ( included in the cluster selected )
  }

  au::log_central->set_node(au::str("D%s", au::code64_str(delilah_id_).c_str()));

  network = new DelilahNetwork(connection_type, delilah_id_);

  // we start with process 2 because 0 is no process & 1 is global_update messages
  id = 2;

  // Init the push & pop manager
  push_manager.Reset(new PushManager(this));

  // Listen notification about netowrk disconnection
  listen(notification_network_diconnected);
  listen("notification_cluster_info_changed");
  listen("delilah_components_review");
  listen(notification_packet_received);

  // Review components
  engine::Engine::shared()->notify(new engine::Notification("delilah_components_review"), 1);

  // No receiver to deal with live data from queues by default ( used in samsonClient library )
  data_receiver_interface = NULL;
}

Delilah::~Delilah() {
  network->ClearConnections();
  clearAllComponents();
}

std::string Delilah::getClusterConnectionSummary() {
  return network->getClusterConnectionStr();
}

bool Delilah::connect(std::string host, au::ErrorManager *error) {
  // Remove all internal state in this delilah....
  if (isConnected()) {
    disconnect();
  }
  LM_V(("Connecting to %s", host.c_str()));

  // Get host and port
  std::vector<std::string> components = au::split(host, ':');

  std::string host_name;
  int port = -1;
  if (components.size() == 2) {
    host_name = components[0];
    port = atoi(components[1].c_str());
  } else if (components.size() == 1) {
    host_name = host;
    port = SAMSON_WORKER_PORT;   // Default port for worker
  }

  // Open a socket connection
  au::SocketConnection *socket_connection = NULL;
  au::Status s = au::SocketConnection::Create(host_name, port, &socket_connection);

  if (s != au::OK) {
    error->AddError(au::str("Error creating socket: '%s'", au::status(s)));
    if (socket_connection) {
      delete socket_connection;
    }
    return false;
  }

  // Special node just to retreive cluster information
  NodeIdentifier node_identifier(DelilahNode, au::code64_rand());

  // Send Hello message
  PacketPointer hello_packet(new Packet(Message::Hello));
  gpb::Hello *hello_hello = hello_packet->message->mutable_hello();
  node_identifier.fill(hello_hello->mutable_node_identifier());
  size_t size;
  s = hello_packet->write(socket_connection, &size);

  if (s != au::OK) {
    error->AddError(au::str("Error sending hello packet %s", au::status(s)));
    if (socket_connection) {
      delete socket_connection;
    }
    return false;
  }

  PacketPointer packet(new Packet());
  size_t total_read = 0;
  s = packet->read(socket_connection, &total_read);

  if (s != au::OK) {
    error->AddError(au::str("Error receiving cluster information %s", au::status(s)));
    if (socket_connection) {
      delete socket_connection;
    }
    return false;
  }

  if (packet->msgCode != Message::ClusterInfoUpdate) {
    error->AddError(
      au::str("Error receiving cluster information. Received %s instead",
              Message::messageCode(packet->msgCode)));
    if (socket_connection) {
      delete socket_connection;
    }
    return false;
  }

  // Update cluster indo

  au::SharedPointer<gpb::ClusterInfo> cluster_info(new gpb::ClusterInfo());
  cluster_info->CopyFrom(packet->message->cluster_info());

  LM_V(("ClusterSetup retreived correctly from %s ( version %lu )"
        , host.c_str()
        , cluster_info->version()));

  network->set_cluster_information(cluster_info);

  if (socket_connection) {
    delete socket_connection;
  }
  return true;
}

void Delilah::disconnect() {
  network->remove_cluster_information();
}

bool Delilah::isConnected() {
  // Check if have received an update from any worker
  return (network->cluster_information_version() != static_cast<size_t>(-1));
}

void Delilah::notify(engine::Notification *notification) {
  if (notification->isName(notification_packet_received)) {
    au::SharedPointer<Packet> packet = notification->dictionary().Get<Packet> ("packet");
    if (packet == NULL) {
      LOG_SW(("Received a notification to receive a packet without a packet"));
    }
    receive(packet);
    return;
  }

  if (notification->isName("delilah_components_review")) {
    // Review all co

    std::map<size_t, DelilahComponent *>::iterator iter;
    for (iter = components_.begin(); iter != components_.end(); iter++) {
      DelilahComponent *component = iter->second;
      component->review();
    }

    return;
  }

  if (notification->isName(notification_network_diconnected)) {
    std::string type = notification->environment().Get("type", "unknown");
    size_t id = notification->environment().Get("id", -1);

    // At the moment only a warning
    showWarningMessage(au::str("Disconnected (%s %lu )", type.c_str(), id));
    return;
  }

  if (notification->isName(notification_disk_operation_request_response)) {
    // Nothing to do here...
    return;
  }

  if (notification->isName("notification_cluster_info_changed")) {
    std::string message = au::str("Cluster setup has changed ( current version %lu )",
                                  network->cluster_information_version());
    showWarningMessage(message);
    return;
  }

  LM_X(1, ("Delilah received an unexpected notification %s", notification->name()));
}

/* ****************************************************************************
 *
 * receive -
 */
void Delilah::receive(const PacketPointer& packet) {
  LM_T(logs.in_messages, ("Delilah received packet type:%s", packet->str().c_str()));

  // Message received
  Message::MessageCode msgCode = packet->msgCode;

  DelilahComponent *component = NULL;

  // --------------------------------------------------------------------
  // Message::Message
  // --------------------------------------------------------------------

  if (msgCode == Message::Message) {
    showWarningMessage(packet->message->message());
    return;
  }

  // --------------------------------------------------------------------
  // ClusterInfoUpdate
  // --------------------------------------------------------------------

  if (packet->msgCode == Message::ClusterInfoUpdate) {
    if (!packet->message->has_cluster_info()) {
      LOG_SW(("Received a cluster info update message without cluster information from connection %s. Ignoring..."
              , packet->from.str().c_str()));
      return;
    }

    // Get a copy of the cluster information
    au::SharedPointer<gpb::ClusterInfo> cluster_info(new gpb::ClusterInfo());
    cluster_info->CopyFrom(packet->message->cluster_info());

    network->set_cluster_information(cluster_info);

    return;
  }

  // --------------------------------------------------------------------
  // StatusReport messages
  // --------------------------------------------------------------------

  if (msgCode == Message::StatusReport) {
    int worker_id = packet->from.id;
    if (worker_id != -1) {
      updateWorkerXMLString(worker_id, packet->message->info());
    } else {
      LOG_SW(("Status report received from an unknown endpoint"));
      return;
    }

    return;
  }

  // --------------------------------------------------------------------
  // PushBlockResponse
  // PushBlockConfirmation
  // --------------------------------------------------------------------
  if ((msgCode == Message::PushBlockResponse) || (msgCode == Message::PushBlockConfirmation)) {
    if (!packet->message->has_push_id()) {
      LOG_SW(("Received a %s without a push_id", Message::messageCode(msgCode)));
      return;
    }
    if (packet->from.node_type != WorkerNode) {
      LOG_SW(("Received a %s from a non-worker nodeid", Message::messageCode(msgCode)));
      return;
    }

    size_t worker_id = packet->from.id;
    size_t push_id = packet->message->push_id();

    // Redirect this message to push_manager
    au::ErrorManager error;
    if (packet->message->has_error()) {
      error.AddError(packet->message->error().message());
    }
    push_manager->receive(msgCode, worker_id, push_id, error);
    return;
  }

  // --------------------------------------------------------------------


  {
    au::TokenTaker tk(&token);

    size_t delilah_component_id = packet->message->delilah_component_id();
    component = components_.findInMap(delilah_component_id);
    if (component) {
      component->receive(packet);
      return;   // If process by component, not process anywhere else
    }
  }

  // Forward the reception of this message to the client
  _receive(packet);
}

/* ****************************************************************************
 *
 * notificationSent -
 */
void Delilah::notificationSent(size_t id, bool success) {
  // Do something
  if ((id == 0) || (success == false)) {
    return;
  }
}

#pragma mark Load data process

/* ****************************************************************************
 *
 * pushData -
 */

size_t Delilah::add_push_component(const std::vector<std::string>&file_names, const std::vector<std::string>& queues) {
  // Data source with these files
  AgregatedFilesDataSource *data_source = new AgregatedFilesDataSource(file_names);

  if (data_source->getTotalSize() == 0) {
    std::ostringstream message;

    if (file_names.size() == 0) {
      message << "No valid files provided";
    } else {
      message << "No content at ";
      for (size_t i = 0; i < file_names.size(); i++) {
        message << file_names[i] << " ";
      }
    }
    showErrorMessage(message.str());
    return 0;
  }

  return add_push_component(data_source, queues);
}

size_t Delilah::add_push_component(DataSource *data_source, const std::vector<std::string>& queues, bool modules) {
  PushDelilahComponent *d = new PushDelilahComponent(data_source, queues);

  if (modules) {
    d->SetUploadModule();
  }
  size_t tmp_id = addComponent(d);
  d->run();
  return tmp_id;
}

size_t Delilah::add_push_module_component(const std::vector<std::string>& file_names) {
  // Spetial one-buffer data source
  IndividualFilesDataSources *data_source = new IndividualFilesDataSources(file_names);

  std::vector<std::string> queues;
  queues.push_back(".modules");

  return add_push_component(data_source, queues, true);
}

size_t Delilah::AddPopComponent(std::string queue_name, std::string fileName, bool force_flag, bool show_flag) {
  PopDelilahComponent *d = new PopDelilahComponent(queue_name, fileName, force_flag, show_flag);
  size_t tmp_id = addComponent(d);

  if (!d->error.HasErrors()) {
    d->run();
  }
  return tmp_id;
}

size_t Delilah::push_txt(engine::BufferPointer buffer, const std::string& queue) {
  std::vector<std::string> queues;
  queues.push_back(queue);
  return push_txt(buffer, queues);
}

size_t Delilah::push_txt(engine::BufferPointer buffer, const std::vector<std::string>& queues) {
  if (buffer == NULL) {
    return static_cast<size_t>(-1);
  }

  // Create a new buffer containing a header
  size_t new_buffer_size = buffer->size() + sizeof(KVHeader);
  engine::BufferPointer new_buffer = engine::Buffer::Create("Push txt data", new_buffer_size);
  // new_buffer->set_size(new_buffer_size);

  // Set the header
  KVHeader *header = reinterpret_cast<KVHeader *>(new_buffer->data());
  header->InitForTxt(buffer->size());
  new_buffer->SkipWrite(sizeof(KVHeader));

  // Copy content of the original buffer
  new_buffer->Write(buffer->data(), buffer->size());

  return push(new_buffer, queues);
}

size_t Delilah::push(engine::BufferPointer buffer, const std::vector<std::string>& queues) {
  // Insert int the push manager
  return push_manager->Push(buffer, queues);
}

size_t Delilah::get_num_push_items() {
  return push_manager->get_num_items();
}

size_t Delilah::addComponent(DelilahComponent *component) {
  au::TokenTaker tk(&token);

  size_t tmp_id = id++;

  component->setId(this, tmp_id);
  components_.insertInMap(tmp_id, component);

  // Call the notifier
  delilahComponentStartNotification(component);

  return tmp_id;
}

void Delilah::cancelComponent(size_t _id) {
  au::TokenTaker tk(&token);

  DelilahComponent *component = components_.findInMap(_id);

  if (component) {
    component->setComponentFinishedWithError("Canceled by user in delilah console");
  } else {
    showWarningMessage(au::str("Not possible to cancel delilah process %lu.", _id));
  }
}

std::string Delilah::getOutputForComponent(size_t _id) {
  au::TokenTaker tk(&token);

  DelilahComponent *component = components_.findInMap(_id);

  if (!component) {
    return au::str("Delilah component %lu not found", _id);
  }
  return component->output_component.str();
}

void Delilah::setBackgroundComponent(size_t _id) {
  au::TokenTaker tk(&token);

  DelilahComponent *component = components_.findInMap(_id);

  if (component) {
    component->set_print_output_at_finish();
  } else {
    showWarningMessage(au::str("Not possible to set delilah process %lu in background.", _id));
  }
}

void Delilah::clearComponents() {
  au::TokenTaker tk(&token);

  std::vector<size_t> components_to_remove;

  for (au::map<size_t, DelilahComponent>::iterator c = components_.begin(); c != components_.end(); c++) {
    if (c->second->isComponentFinished()) {
      components_to_remove.push_back(c->first);
    }
  }

  for (size_t i = 0; i < components_to_remove.size(); i++) {
    DelilahComponent *component = components_.extractFromMap(components_to_remove[i]);
    if (component) {
      delete component;
    }
  }
}

void Delilah::clearAllComponents() {
  au::TokenTaker tk(&token);

  std::vector<size_t> components_to_remove;

  au::map<size_t, DelilahComponent>::iterator c;
  for (c = components_.begin(); c != components_.end(); c++) {
    components_to_remove.push_back(c->first);
  }

  for (size_t i = 0; i < components_to_remove.size(); i++) {
    DelilahComponent *component = components_.extractFromMap(components_to_remove[i]);
    if (component) {
      delete component;
    }
  }
}

std::string Delilah::getListOfComponents() {
  au::tables::Table table("id|type,left|status,left|time,left|completion,left|concept,left");

  table.setTitle("List of delilah processes");

  std::map<size_t, DelilahComponent *>::iterator iter;
  for (iter = components_.begin(); iter != components_.end(); iter++) {
    DelilahComponent *component = iter->second;

    if (component->hidden) {
      continue;
    }
    au::StringVector values;
    values.push_back(au::str("%s_%lu", au::code64_str(delilah_id_).c_str(), component->getId()));
    values.push_back(component->getTypeName());
    values.push_back(component->getStatusDescription());

    values.push_back(au::str_time(component->cronometer.seconds()));
    values.push_back(au::str_percentage(component->progress));

    values.push_back(component->concept);

    // cronometer
    table.addRow(values);
  }
  return table.str();
}

size_t Delilah::sendWorkerCommand(std::string command, engine::BufferPointer buffer) {
  // Add a components for the reception
  WorkerCommandDelilahComponent *c = new WorkerCommandDelilahComponent(command, buffer);

  // Get the id of this operation
  size_t tmp_id = addComponent(c);

  // Send the packet to necessary workers
  c->run();

  return tmp_id;
}

bool Delilah::isActive(size_t id) {
  au::TokenTaker tk(&token);

  DelilahComponent *c = components_.findInMap(id);

  if (!c) {
    LOG_SW(("Unknown delilah component for id:%lu", id));
    return false;
  }
  return (!c->isComponentFinished());
}

bool Delilah::hasError(size_t id) {
  au::TokenTaker tk(&token);

  DelilahComponent *c = components_.findInMap(id);

  // No process, no error ;)
  if (!c) {
    LOG_SW(("Unknown delilah component for id:%lu", id));
    return false;
  }

  return c->error.HasErrors();
}

std::string Delilah::errorMessage(size_t id) {
  au::TokenTaker tk(&token);

  DelilahComponent *c = components_.findInMap(id);

  // No process, no error ;)
  if (!c) {
    return "Invalid delilah process";
  }

  return c->error.GetLastError();
}

std::string Delilah::getDescription(size_t id) {
  au::TokenTaker tk(&token);

  DelilahComponent *c = components_.findInMap(id);

  if (!c) {
    return "No process with this id";
  } else {
    return c->getStatus();
  }
}

int Delilah::_receive(const PacketPointer& packet) {
  if (packet->msgCode != Message::Alert) {
    LM_V(("Unused packet %s", packet->str().c_str()));
  }
  return 0;
}

// Get information for monitorization
void Delilah::getInfo(std::ostringstream&  /* output */) {
  // Engine
  // engine::Engine::shared()->getInfo( output );

  // Engine system
  // samson::getInfoEngineSystem(output, network);

  // Modules manager
  // au::Singleton<ModulesManager>::shared()->getInfo( output );

  // Network
  // network->getInfo( output , "main" );
}

std::string Delilah::getLsLocal(std::string pattern, bool only_queues) {
  au::tables::Table table("Name,left|Type,left|Size|Format,left|Error,left");

  // first off, we need to create a pointer to a directory
  DIR *pdir = opendir(".");   // "." will refer to the current directory
  struct dirent *pent = NULL;

  if (pdir != NULL) {   // if pdir wasn't initialised correctly
    while ((pent = readdir(pdir))) {   // while there is still something in the directory to list
      if (pent != NULL) {
        std::string fileName = pent->d_name;

        if ((fileName != ".") && (fileName != "..")) {
          struct stat buf2;
          stat(pent->d_name, &buf2);

          if (::fnmatch(pattern.c_str(), pent->d_name, 0) != 0) {
            continue;
          }
          if (S_ISREG(buf2.st_mode)) {
            size_t size = buf2.st_size;
            if (!only_queues) {
              table.addRow(au::StringVector(pent->d_name, "FILE", au::str(size, "bytes"), "-", "-"));
            }
          }
          if (S_ISDIR(buf2.st_mode)) {
            au::ErrorManager error;
            au::SharedPointer<SamsonDataSet> samson_data_set = SamsonDataSet::create(pent->d_name, error);

            if (error.HasErrors()) {
              if (!only_queues) {
                table.addRow(au::StringVector(pent->d_name, "DIR", "", "", error.GetLastError()));
              }
            } else {
              table.addRow(
                au::StringVector(pent->d_name, "SAMSON queue", samson_data_set->info().strDetailed(),
                                 samson_data_set->format().str(), "-"));
            }
          }
        }
      }
    }
    // finally, let's close the directory
    closedir(pdir);
  }

  if (only_queues) {
    table.setTitle(au::str("Local queues ( %s )", pattern.c_str()));
  } else {
    table.setTitle(au::str("Local files ( %s )", pattern.c_str()));
  }
  return table.str();
}

DelilahComponent *Delilah::getComponent(size_t id) {
  return components_.findInMap(id);
}
}

