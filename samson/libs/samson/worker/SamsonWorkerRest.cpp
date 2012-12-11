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
#include "samson/worker/SamsonWorkerRest.h"  // Own interface

#include "samson/common/Logs.h"
#include "samson/common/ports.h"          // SAMSON_WORKER_PORT
#include "samson/common/samsonVersion.h"
#include "samson/delilah/WorkerCommandDelilahComponent.h"
#include "samson/worker/SamsonWorker.h"

namespace samson {
SamsonWorkerRest::SamsonWorkerRest(SamsonWorker *samson_worker, int web_port) :
  samson_worker_samples_(samson_worker) {
  // Keep pointer to samson worker
  samson_worker_ = samson_worker;

  // No auto-client at the moment
  delilah_ = NULL;   // Still testing cluster setup.... ( disabling temporary )
  // delilah = new Delilah( "rest" );
  // delilah->connect( au::str("localhost:%d" , port ) );   // Connect to myself

  // Run REST interface
  rest_service_ = new au::network::RESTService(web_port, this);
  rest_service_->InitService();

  // Listen synchronized rest commands
  listen("notification_process_lookup_synchronized");   // Notification with rest commands

  // Take samples for the REST interface
  listen(notification_samson_worker_take_sample);
  {
    engine::Notification *notification = new engine::Notification(notification_samson_worker_take_sample);
    engine::Engine::shared()->notify(notification, 1);
  }
}

SamsonWorkerRest::~SamsonWorkerRest() {
  LOG_M(logs.cleanup, ("Calling ~SamsonWorkerRest this:%p, rest_service->StopService()", this));
  rest_service_->StopService();
  if (delilah_) {
    delete delilah_;
  }
  if (rest_service_) {
    delete rest_service_;
  }
}

void SamsonWorkerRest::StopRestService() {
  LOG_M(logs.cleanup, ("Calling rest_service->StopService()"));
  rest_service_->StopService();
}

void SamsonWorkerRest::notify(engine::Notification *notification) {
  if (notification->isName(notification_samson_worker_take_sample)) {
    samson_worker_samples_.take_samples();    // Take samples every second to check current status
    return;
  }

  if (notification->isName("notification_process_lookup_synchronized")) {
    // Get RestServiceCommand from the notification
    au::SharedPointer<au::network::RESTServiceCommand> command;
    command = notification->dictionary().Get<au::network::RESTServiceCommand> ("command");

    if (command == NULL) {
      LOG_SW(("rest_connection notification without a command. This is probably an error..."));
      return;
    }

    // Process this command ( now it is synchronized with engine )
    ProcessLookupSynchronized(command);

    // Mark as finish and wake up thread to answer this connection
    command->NotifyFinish();
    return;
  }
}

void SamsonWorkerRest::process(au::SharedPointer<au::network::RESTServiceCommand> command) {
  // Set XML ( default format ) if no format is specified
  if (command->format() == "") {
    command->set_format("xml");
  }

  // Init message
  if (command->format() == "xml") {
    command->Append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    command->Append("<!-- SAMSON Rest interface -->\n");
    command->Append("<samson>\n");
  } else if (command->format() == "html") {
    command->Append("<html><body>");
  } else if (command->format() == "json") {
    command->Append("{");  // Returning always a json-object
  }

  // Internal process of the command
  ProcessIntern(command);

  // Close data content
  if (command->format() == "xml") {
    command->Append("\n</samson>\n");
  } else if (command->format() == "html") {
    command->Append("</body></html>");
  } else if (command->format() == "json") {
    command->Append("}");  // Returning an object
  }
}

void SamsonWorkerRest::Append(au::SharedPointer<au::network::RESTServiceCommand> command,
                              au::SharedPointer<gpb::Collection> collection) {
  au::SharedPointer<au::tables::Table> table = GetTableFromCollection(collection);

  if (table == NULL) {
    command->AppendFormatedError("No data");
  } else {
    if (command->format() == "json") {
      // Spetial case since json global element is always an object
      command->Append(std::string("\"table\":") + table->strFormatted(command->format()) + ",");
    } else {
      command->Append(table->strFormatted(command->format()));
    }
  }
}

std::string GetCommandAndLink(const std::string& command) {
  return au::str("%s <a href=\"%s.html\">link</a>", command.c_str(), command.c_str());
}

void SamsonWorkerRest::ProcessIntern(au::SharedPointer<au::network::RESTServiceCommand> command) {
  Visualization v;   // Common visualitzation options to generate all collections

  // Get and check number of components
  size_t components = command->path_components().size();

  if (components < 2) {
    command->AppendFormatedError(400, "Only /samson/option paths are valid");
    return;
  }

  // Check main component to start with /samson/*
  if (command->path_components()[0] != "samson") {
    command->AppendFormatedError(400, "Only /samson/option paths are valid");
    return;
  }

  std::string main_command = command->path_components()[1];
  std::string verb = command->command();
  std::string path = command->path();


  if ((main_command == "queues") && (verb == "DELETE")) {
    // Remove a queue
    if (command->path_components().size() < 3) {
      command->AppendFormatedError("Only /samson/queues/queue_name paths accepted");
      return;
    }

    std::string queue_name = command->path_components()[2];
    ProcessCommitToDatamodel(au::str("rm %s", queue_name.c_str()), command);
    return;
  }

  if ((path == "/samson/help") && (verb == "GET")) {
    au::tables::Table table("command|description");
    table.setTitle("REST commands");
    table.addRow(au::StringVector(GetCommandAndLink("/samson/help"), "Show this help message"));
    table.addRow(au::StringVector(GetCommandAndLink("/samson/version"), "Get SAMSON version for this worker"));
    table.addRow(au::StringVector(GetCommandAndLink("/samson/cluster"),
                                  "Show information about all workers in this SAMSON cluster"));
    table.addRow(au::StringVector(GetCommandAndLink("/samson/workers"),
                                  "Show live information about workers in the current cluster"));
    table.addRow(au::StringVector(GetCommandAndLink("/samson/kv_ranges"), "Show information about different KVRanges"));
    table.addRow(au::StringVector(GetCommandAndLink("/samson/queues"), "Show current queues"));
    table.addRow(au::StringVector(GetCommandAndLink("/samson/modules"), "Show loaded modules"));
    table.addRow(au::StringVector(GetCommandAndLink("/samson/operations"), "Show operations for loaded modules"));
    table.addRow(au::StringVector(GetCommandAndLink("/samson/datas"), "Show data-types for loaded modules"));
    table.addRow(au::StringVector(GetCommandAndLink("/samson/stream_operations"), "Show stream operations"));
    table.addRow(au::StringVector(GetCommandAndLink("/samson/tasks"),
                                  "Show tasks scheduled and executing in this worker"));
    command->Append(table.strFormatted(command->format()));
    return;
  }

  if ((path == "/samson/version") && (verb == "GET")) {
    command->AppendFormatedLiteral("version", au::str("SAMSON v %s", SAMSON_VERSION));
    return;
  }

  if ((path == "/samson/kv_ranges") && (verb == "GET")) {
    Append(command, samson_worker_->GetKVRangesCollection(v));
    return;
  }

  if ((path == "/samson/status") && (verb == "GET")) {
    command->Append("TBC");
    return;
  }

  if ((path == "/samson/workers") && (verb == "GET")) {
    ProcessDelilahCommand("ls_workers", command);
    return;
  }

  if ((path == "/samson/queues") && (verb == "GET")) {
    Append(command, samson_worker_->data_model()->GetCollectionForQueues(v));
    return;
  }

  if ((path == "/samson/tasks") && (verb == "GET")) {
    Append(command, samson_worker_->task_manager()->GetCollection(v));
    return;
  }

  if ((path == "/samson/modules") && (verb == "GET")) {
    Append(command, au::Singleton<ModulesManager>::shared()->GetModulesCollection(v));
    return;
  }

  if ((path == "/samson/operations") && (verb == "GET")) {
    Append(command, au::Singleton<ModulesManager>::shared()->GetOperationsCollection(v));
    return;
  }

  if ((path == "/samson/datas") && (verb == "GET")) {
    Append(command, au::Singleton<ModulesManager>::shared()->GetDatasCollection(v));
    return;
  }

  if ((path == "/samson/stream_operations") && (verb == "GET")) {
    Append(command, samson_worker_->data_model()->GetCollectionForStreamOperations(v));
    return;
  }

  // General command
  // This command is for debugging only and should be removed in official releases
  if (main_command == "command") {
    if (components < 3) {
      command->AppendFormatedError(400, "Only /samson/command/X paths are valid");
      return;
    }
    std::string delilah_command = command->path_components()[2];
    ProcessDelilahCommand(delilah_command, command);
    return;
  }

  if ((path == "/samson/cluster") && (verb == "GET")) {
    au::SharedPointer<samson::gpb::ClusterInfo> cluster_info =
      samson_worker_->worker_controller()->GetCurrentClusterInfo();
    if (cluster_info == NULL) {
      command->AppendFormatedError("No cluster information found");
      return;
    }
    au::tables::Table table("worker_id|host|port|port rest|cores|memory,f=uint64");
    table.setTitle(au::str("Cluster information (v. %lu)", cluster_info->version()));

    for (int i = 0; i < cluster_info->workers_size(); i++) {
      au::StringVector values;
      values.Push(cluster_info->workers(i).worker_id());
      values.Push(cluster_info->workers(i).worker_info().host());
      values.Push(cluster_info->workers(i).worker_info().port());
      values.Push(cluster_info->workers(i).worker_info().port_web());
      values.Push(cluster_info->workers(i).worker_info().cores());
      values.Push(cluster_info->workers(i).worker_info().memory());
      table.addRow(values);
    }
    command->Append(table.strFormatted(command->format()));
    return;
  }

  if (main_command == "queues") {
    /* /samson/state/queue/key */
    if (components < 5) {
      command->AppendFormatedError(400, "Only /samson/queues/queue_name/key/key paths are valid");
    } else {
      engine::Notification *notification = new engine::Notification("notification_process_lookup_synchronized");
      notification->dictionary().Set<au::network::RESTServiceCommand> ("command", command);
      engine::Engine::shared()->notify(notification);

      // Wait until completed
      command->WaitUntilFinished();
      return;
    }
  }

  // Test to verify uploaded data
  if (main_command == "data_test") {
    command->AppendFormatedLiteral("data_size", au::str("%lu", command->data_size()));

    if (command->data_size() == 0) {
      command->AppendFormatedLiteral("Data", "No data provided in the REST request");
    } else {
      // Return with provided data
      std::string data;
      data.append(command->data(), command->data_size());
      command->AppendFormatedLiteral("data", data);
    }
  }


  // Unknown command so far
  command->AppendFormatedError(404, au::str("Bad VERB (%s) or PATH (%s)", verb.c_str(), path.c_str()));
}

void SamsonWorkerRest::ProcessDelilahCommand(std::string delilah_command,
                                             au::SharedPointer<au::network::RESTServiceCommand> command) {
  // Create client if not created
  if (!delilah_) {
    delilah_ = new Delilah("rest");
    au::ErrorManager error;
    delilah_->connect(au::str("localhost:%d", samson_worker_->port()), &error);

    if (error.HasErrors()) {
      delete delilah_;
      delilah_ = NULL;
      command->AppendFormatedError(500, "Not possible to connect internal delilah client");
    }
  }

  LOG_M(logs.delilah, ("Sending delilah command: '%s'", delilah_command.c_str()));
  size_t command_id = delilah_->sendWorkerCommand(delilah_command);

  // Wait for the command to finish
  {
    au::Cronometer c;
    while (delilah_->isActive(command_id)) {
      usleep(10000);
      if (c.seconds() > 2) {
        command->AppendFormatedError(500, au::str("Timeout awaiting response from REST client (task %lu)", command_id));
        LM_E(("Timeout awaiting response from REST client"));
        return;
      }
    }
  }

  // Recover information
  WorkerCommandDelilahComponent *component =
    reinterpret_cast<WorkerCommandDelilahComponent *>(delilah_->getComponent(command_id));
  if (!component) {
    command->AppendFormatedError(500, "Internal error recovering answer from REST client");
    LM_E(("Internal error recovering answer from REST client"));
    return;
  }

  // Recover table from component
  au::tables::Table *table = component->getMainTable();

  if (!table) {
    // command->AppendFormatedError(500, "No content in answer from REST client" );
    LM_E(("No content in answer from REST client"));
    return;
  }

  LOG_M(logs.rest, ("appending delilah output to command: '%s'", table->str().c_str()));
  command->Append(table->strFormatted(command->format()));
  delete table;
}

void SamsonWorkerRest::ProcessCommitToDatamodel(const std::string& c
                                                , au::SharedPointer<au::network::RESTServiceCommand> command) {
  au::ErrorManager error;

  samson_worker_->data_model()->Commit("REST", c, error);
  command->AppendFormatedLiteral("command", c);

  if (error.HasErrors()) {
    command->AppendFormatedError(error.GetLastError());
  } else {
    command->AppendFormatedLiteral("result", "OK");
  }
}

void SamsonWorkerRest::ProcessLookupSynchronized(au::SharedPointer<au::network::RESTServiceCommand> command) {
  // Get queue and key specified
  std::string queue_name = command->path_components()[2];
  std::string key = command->path_components()[4];

  // If debug is specified ( instead of "key" ) show more messages
  bool debug = (command->path_components()[3] == "debug");

  LOG_M(logs.rest, ("looking up key '%s' in queue '%s'", key.c_str(), queue_name.c_str()));

  // Get  copy of the current data model
  au::SharedPointer<gpb::DataModel> data_model = samson_worker_->data_model()->getCurrentModel();
  gpb::Data *data =  data_model->mutable_current_data();

  // Get queue
  gpb::Queue *queue = gpb::get_queue(data, queue_name);

  if (!queue) {
    LM_E(("Queue '%s' not found for REST query", queue_name.c_str()));
    command->AppendFormatedError(au::str("Queue '%s' not found", queue_name.c_str()));
    return;
  }

  // Get the format of this queue
  KVFormat format(queue->key_format(), queue->value_format());

  if (format.isGenericKVFormat()) {
    // Unknown format
    command->AppendFormatedError(au::str("Queue '%s' format %s not valid for quering", format.str().c_str()));
    return;
  }

  // Data instances
  Data *key_data = au::Singleton<ModulesManager>::shared()->GetData(format.keyFormat);
  Data *value_data = au::Singleton<ModulesManager>::shared()->GetData(format.valueFormat);

  if (!key_data || !value_data) {
    LM_E(("Queue '%s' has wrong format for REST query", queue_name.c_str()));
    std::string m = au::str("Queue '%s' has wrong format (%s)", queue_name.c_str(), format.str().c_str());
    command->AppendFormatedError(m);
    return;
  }

  au::SharedPointer<DataInstance> reference_key_data_instance(reinterpret_cast<DataInstance *>(key_data->getInstance()));
  au::SharedPointer<DataInstance> key_data_instance(reinterpret_cast<DataInstance *>(key_data->getInstance()));
  au::SharedPointer<DataInstance> value_data_instance(reinterpret_cast<DataInstance *>(value_data->getInstance()));

  // Get all the information from the reference key
  std::string error_message;
  if (!reference_key_data_instance->setContentFromString(key.c_str(), error_message)) {
    // Error decoding key
    command->AppendFormatedError(au::str("Not possible to convert %s into a valid %s (%s)"
                                         , key.c_str(), key_data->getName().c_str(), error_message.c_str()));
    return;
  }
  LOG_M(logs.rest, ("Recovered key: '%s' --> '%s'", key.c_str(), reference_key_data_instance->str().c_str()));


  // Get hashgroup
  int hg = reference_key_data_instance->hash(KVFILE_NUM_HASHGROUPS);
  LOG_M(logs.rest, ("Hash group: %d", hg));

  // Get the worker for this hash.group
  size_t worker_id = samson_worker_->worker_controller()->GetMainWorkerForHashGroup(hg);
  size_t my_worker_id = samson_worker_->worker_controller()->worker_id();

  if (my_worker_id != worker_id) {
    std::string host = samson_worker_->worker_controller()->getHostForWorker(worker_id);
    unsigned short port = samson_worker_->worker_controller()->getWebPortForWorker(worker_id);

    command->SetRedirect(au::str("http://%s:%d%s", host.c_str(), port, command->resource().c_str()));
  }

  if (debug) {
    command->AppendFormatedLiteral("search_key", key);
    command->AppendFormatedLiteral("search_queue", queue_name);
    command->AppendFormatedLiteral("hash_group", au::str("%d", hg));    // Inform about the hash-group
    command->AppendFormatedLiteral("worker", au::str("%lu", worker_id));    // Inform about the worker
  }


  // Search for the correct block
  for (int b = 0; b < queue->blocks_size(); ++b) {
    const gpb::Block& block = queue->blocks(b);

    size_t block_id = block.block_id();  // Identifier of this block
    KVRange range = block.range();       // Implicit conversion

    if (range.Contains(hg)) {
      // This is the block
      stream::BlockPointer block_ptr = stream::BlockManager::shared()->GetBlock(block_id);

      // I am suppouse to have the block
      if (block_ptr == NULL) {
        command->AppendFormatedError(au::str("Interal error. Block %lu not present in this worker", block_id));
        return;
      }

      if (!block_ptr->is_content_in_memory()) {
        command->AppendFormatedError(au::str("Block %s not in memory for queue %s"
                                             , str_block_id(block_id).c_str()
                                             , queue_name.c_str()));
        return;
      }

      if (debug) {
        command->AppendFormatedLiteral("block", str_block_id(block_id));
        command->AppendFormatedLiteral("block_range", range.str());
      }

      block_ptr->lookup(key.c_str(), command);
      return;
    }
  }
  command->AppendFormatedError("Key not found");
}
}

