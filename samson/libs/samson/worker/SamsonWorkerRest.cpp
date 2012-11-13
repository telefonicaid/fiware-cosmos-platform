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
  LM_T(LmtCleanup, ("Calling ~SamsonWorkerRest this:%p, rest_service->StopService()", this));
  rest_service_->StopService();
  if (delilah_) {
    delete delilah_;
  }
  if (rest_service_) {
    delete rest_service_;
  }
}

void SamsonWorkerRest::StopRestService() {
  LM_T(LmtCleanup, ("Calling rest_service->StopService()"));
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
      LM_W(("rest_connection notification without a command. This is probably an error..."));
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
  // ---------------------------------------------------
  if (command->format() == "xml") {
    command->Append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    command->Append("<!-- SAMSON Rest interface -->\n");
    command->Append("<samson>\n");
  } else if (command->format() == "html") {
    command->Append("<html><body>");
  } else if (command->format() == "json") {
  }

  // Internal process of the command
  ProcessIntern(command);

  // Close data content
  // ---------------------------------------------------
  if (command->format() == "xml") {
    command->Append("\n</samson>\n");
  } else if (command->format() == "html") {
    command->Append("</body></html>");
  }
}

void SamsonWorkerRest::Append(au::SharedPointer<au::network::RESTServiceCommand> command,
                              au::SharedPointer<gpb::Collection> collection) {
  au::SharedPointer<au::tables::Table> table = GetTableFromCollection(collection);

  if (table == NULL) {
    command->AppendFormatedError("No data");
  } else {
    command->Append(table->strFormatted(command->format()));
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
    command->AppendFormatedElement("version", au::str("SAMSON v %s", SAMSON_VERSION));
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

  if (main_command == "ilogging") {
    process_ilogging(command);
    return;
  }

  if (( main_command == "state" ) || ( main_command == "queue" )) {
    /* /samson/state/queue/key */
    if (components < 4) {
      command->AppendFormatedError(400, "Only /samson/state/queue/key paths are valid");
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
    command->AppendFormatedElement("data_size", au::str("%lu", command->data_size()));

    if (command->data_size() == 0) {
      command->AppendFormatedElement("Data", "No data provided in the REST request");
    } else {
      // Return with provided data
      std::string data;
      data.append(command->data(), command->data_size());
      command->AppendFormatedElement("data", data);
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

    if (error.IsActivated()) {
      delete delilah_;
      delilah_ = NULL;
      command->AppendFormatedError(500, "Not possible to connect internal delilah client");
    }
  }

  LM_T(LmtDelilahCommand, ("Sending delilah command: '%s'", delilah_command.c_str()));
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

  LM_T(LmtRest, ("appending delilah output to command: '%s'", table->str().c_str()));
  command->Append(table->strFormatted(command->format()));
  delete table;
}

void SamsonWorkerRest::process_ilogging(au::SharedPointer<au::network::RESTServiceCommand> command) {
  std::ostringstream logdata;
  std::string logCommand = "";
  std::string sub = "";
  std::string arg = "";

  command->set_http_state(200);

  if (command->path_components().size() > 2) {
    logCommand = command->path_components()[2];
  }
  if (command->path_components().size() > 3) {
    sub = command->path_components()[3];
  }
  if (command->path_components().size() > 4) {
    arg = command->path_components()[4];   //
  }
  // Treat all possible errors
  //

  if (logCommand == "") {
    command->set_http_state(400);
    command->AppendFormatedElement("message", au::str("no ilogging subcommand"));
  } else if ((logCommand != "reads") && (logCommand != "writes") && (logCommand != "traces") && (logCommand
                                                                                                 != "verbose") &&
             (logCommand != "debug"))
  {
    command->set_http_state(400);
    command->AppendFormatedElement("message", au::str("bad ilogging command: '%s'", logCommand.c_str()));
  } else if (((logCommand == "reads") || (logCommand == "writes") || (logCommand == "debug")) && (sub != "on") && (sub
                                                                                                                   !=
                                                                                                                   "off"))
  {
    command->set_http_state(400);
    command->AppendFormatedElement("message",
                                   au::str("bad ilogging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
  } else if ((logCommand == "verbose") && (sub != "get") && (sub != "set") && (sub != "off") && (sub != "")) {
    command->set_http_state(400);
    command->AppendFormatedElement("message",
                                   au::str("bad ilogging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
  } else if ((logCommand == "verbose") && (sub == "set") && (arg != "0") && (arg != "1") && (arg != "2")
             && (arg != "3") && (arg != "4") && (arg != "5"))
  {
    command->set_http_state(400);
    command->AppendFormatedElement("message", au::str("bad ilogging argument for 'verbose': %s", arg.c_str()));
  } else if ((logCommand == "traces") && (sub != "get") && (sub != "set") && (sub != "add") && (sub != "remove")
             && (sub != "off") && (sub != ""))
  {
    command->set_http_state(400);
    command->AppendFormatedElement("message",
                                   au::str("bad ilogging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
  } else if ((logCommand == "traces") && ((sub != "set") || (sub != "add") || (sub != "remove"))) {
    if (strspn(arg.c_str(), "0123456789-,") != strlen(arg.c_str())) {
      command->set_http_state(400);
      command->AppendFormatedElement("message",
                                     au::str("bad ilogging parameter '%s' for 'trace/%s'", arg.c_str(), sub.c_str()));
    }
  }

  //
  // Checking the VERB
  //
  std::string verb = command->command();
  std::string path = logCommand;

  if (sub != "") {
    path += '/' + sub;
  }
  if ((path == "debug/on") && (verb == "POST")) {
    ;
  } else if ((path == "debug/off") && (verb == "POST")) {
    ;
  } else if ((path == "reads/on") && (verb == "POST")) {
    ;
  } else if ((path == "reads/off") && (verb == "POST")) {
    ;
  } else if ((path == "writes/on") && (verb == "POST")) {
    ;
  } else if ((path == "writes/off") && (verb == "POST")) {
    ;
  } else if ((path == "traces") && (verb == "GET")) {
    ;
  } else if ((path == "traces/off") && (verb == "POST")) {
    ;
  } else if ((path == "traces/get") && (verb == "GET")) {
    ;
  } else if ((path == "traces/set") && (verb == "POST")) {
    ;
  } else if ((path == "traces/add") && (verb == "POST")) {
    ;
  } else if ((path == "traces/remove") && (verb == "DELETE")) {
    ;
  } else if ((path == "verbose") && (verb == "GET")) {
    ;
  } else if ((path == "verbose/off") && (verb == "POST")) {
    ;
  } else if ((path == "verbose/set") && (verb == "POST")) {
    ;
  } else {
    command->set_http_state(404);
    command->AppendFormatedElement("error", "BAD VERB");
    return;
  }

  if (command->http_state() != 200) {
    return;
  }

  //
  // Treat the request
  //
  if (logCommand == "reads") {
    if (sub == "on") {
      lmReads = true;
      command->AppendFormatedElement("reads", au::str("reads turned ON"));
      LM_F(("Turned on READS for this node only"));
    } else if (sub == "off") {
      lmReads = false;
      command->AppendFormatedElement("reads", au::str("reads turned OFF"));
      LM_F(("Turned off READS for this node only"));
    }
  } else if (logCommand == "writes") {
    if (sub == "on") {
      lmWrites = true;
      command->AppendFormatedElement("writes", au::str("writes turned ON"));
      LM_F(("Turned on WRITES for this node only"));
    } else if (sub == "off") {
      lmWrites = false;
      command->AppendFormatedElement("writes", au::str("writes turned OFF"));
      LM_F(("Turned off WRITES for this node only"));
    }
  } else if (logCommand == "debug") {
    if (sub == "on") {
      lmDebug = true;
      command->AppendFormatedElement("debug", au::str("debug turned ON"));
      LM_F(("Turned on DEBUG for this node only"));
      LM_D(("Turned on DEBUG for this node only"));
    } else if (sub == "off") {
      lmDebug = false;
      command->AppendFormatedElement("debug", au::str("debug turned OFF"));
      LM_F(("Turned off DEBUG for this node only"));
      LM_D(("This line should not be seen ..."));
    }
  } else if (logCommand == "verbose") {
    // /samson/ilogging/verbose
    if (sub == "") {
      sub = "get";
    }
    if ((sub == "set") && (arg == "0")) {
      sub = "off";
    }
    if (sub == "get") {
      int vLevel;

      if (lmVerbose5 == true) {
        vLevel = 5;
      } else if (lmVerbose4 == true) {
        vLevel = 4;
      } else if (lmVerbose3 == true) {
        vLevel = 3;
      } else if (lmVerbose2 == true) {
        vLevel = 2;
      } else if (lmVerbose == true) {
        vLevel = 1;
      } else {
        vLevel = 0;
      }
      command->AppendFormatedElement("verbose", au::str("verbosity level: %d", vLevel));
    } else {
      // Turn all verbose levels OFF
      lmVerbose = false;
      lmVerbose2 = false;
      lmVerbose3 = false;
      lmVerbose4 = false;
      lmVerbose5 = false;

      if (sub == "off") {
        arg = "0";
      }
      int verboseLevel = arg[0] - '0';

      // Turn on the desired verbose levels
      switch (verboseLevel) {
        case 5:
          lmVerbose5 = true;
        case 4:
          lmVerbose4 = true;
        case 3:
          lmVerbose3 = true;
        case 2:
          lmVerbose2 = true;
        case 1:
          lmVerbose = true;
      }

      command->AppendFormatedElement("verbose", au::str("verbosity level: %d", verboseLevel));
      LM_F(("New verbose level for this node only: %d", verboseLevel));
    }
  } else if (logCommand == "traces") {
    if (sub == "") {
      sub = "get";
    }
    if (sub == "set") {
      lmTraceSet(arg.c_str());
      command->AppendFormatedElement("trace", au::str("trace level: %s", arg.c_str()));
      LM_F(("Set trace levels to '%s' for this node only", arg.c_str()));
    } else if (sub == "get") {
      // /samson/ilogging/trace/get
      char traceLevels[1024];
      lmTraceGet(traceLevels);

      command->AppendFormatedElement("trace", au::str("trace level: %s", traceLevels));
    } else if (sub == "off") {
      // /samson/ilogging/trace/off
      lmTraceSet(NULL);
      command->AppendFormatedElement("trace", au::str("all trace levels turned off"));
      LM_F(("Turned off trace levels for this node only"));
    } else if (sub == "add") {
      // /samson/ilogging/trace/add
      lmTraceAdd(arg.c_str());
      command->AppendFormatedElement("trace", au::str("added level(s) %s", arg.c_str()));
      LM_F(("Added trace levels '%s' for this node only", arg.c_str()));
    } else if (sub == "remove") {
      // /samson/ilogging/trace/remove
      lmTraceSub(arg.c_str());
      command->AppendFormatedElement("trace", au::str("removed level(s) %s", arg.c_str()));
      LM_F(("Removed trace levels '%s' for this node only", arg.c_str()));
    }
  }
}

void SamsonWorkerRest::ProcessLookupSynchronized(au::SharedPointer<au::network::RESTServiceCommand> command) {
  // Get queue and key specified
  std::string queue_name = command->path_components()[2];
  std::string key = command->path_components()[3];

  LM_T(LmtRest, ("looking up key '%s' in queue '%s'", key.c_str(), queue_name.c_str()));

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
  Data *key_data = au::Singleton<ModulesManager>::shared()->getData(format.keyFormat);
  Data *value_data = au::Singleton<ModulesManager>::shared()->getData(format.valueFormat);

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
  reference_key_data_instance->setFromString(key.c_str());
  LM_T(LmtRest, ("Recovered key: '%s' --> '%s'", key.c_str(), reference_key_data_instance->str().c_str()));

  // Append key in all cases
  command->AppendFormatedElement("key", reference_key_data_instance->strFormatted(command->format()));

  // Get hashgroup
  int hg = reference_key_data_instance->hash(KVFILE_NUM_HASHGROUPS);
  LM_T(LmtRest, ("Hash group: %d", hg));

  // Get the worker for this hash.group
  size_t worker_id = samson_worker_->worker_controller()->GetMainWorkerForHashGroup(hg);
  size_t my_worker_id = samson_worker_->worker_controller()->worker_id();

  if (my_worker_id != worker_id) {
    LM_X(1, ("Redirection not implemented"));
    /*
     * std::string  host = worker->network->getHostForWorker( worker_id );
     * //unsigned short  port = worker->network->getPortForWorker( worker_id );
     * unsigned short  port = web_port;   // We have to use to REST port, not the connections port
     *
     * LM_T(LmtRest, ("Redirect to the right server (%s:%d)", host.c_str(), port));
     * command->set_redirect( au::str("http://%s:%d%s", host.c_str(), port , command->resource.c_str() ) );
     * return;
     */
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
        command->AppendFormatedError(au::str("Block %lu not in memory for queue %s", block_id, queue_name.c_str()));
        return;
      }

      block_ptr->lookup(key.c_str(), command);
      return;
    }
  }
  command->AppendFormatedError(au::str("Key ´%s´ not found in the queue %s", key.c_str(), queue_name.c_str()));
}
}

