
#include "samson/common/gpb_operations.h"
#include "samson/network/Packet.h"

#include "DataModel.h"  // Own interface

namespace samson {
DataModel::DataModel(zoo::Connection *zoo_connection)
  : ZooNodeCommiter<gpb::Data>(zoo_connection, "/samson/data") {
}

DataModel::~DataModel() {
}

void DataModel::PerformCommit(au::SharedPointer<gpb::Data> data, std::string command, int version,
                              au::ErrorManager *error) {
  au::CommandLine cmd;

  // Input output definition of queues
  cmd.SetFlagString("input", "");
  cmd.SetFlagString("output", "");

  // Forward flag to indicate that this is a reduce forward operation ( no update if state )
  cmd.SetFlagBoolean("forward");

  // Number of divisions in state operations
  cmd.SetFlagInt("divisions", au::Singleton<SamsonSetup>::shared()->getInt("general.num_processess"));

  // Use third party software only for state with new input
  cmd.SetFlagBoolean("update_only");

  // Prefix used to change names of queues and operations
  cmd.SetFlagString("prefix", "");

  cmd.SetFlagUint64("delilah_id", (size_t)-1);
  cmd.SetFlagUint64("delilah_component_id", (size_t)-1);

  cmd.Parse(command);

  // Recover prefix
  std::string prefix = cmd.GetFlagString("prefix");
  bool forward       = cmd.GetFlagBool("forward");
  bool update_only   = cmd.GetFlagBool("update_only");

  if (cmd.get_num_arguments() == 0) {
    error->set("No command specified");
    return;
  }

  cmd.Parse(command);

  if (cmd.get_num_arguments() == 0) {
    error->set("No command provided");
    return;
  }

  // Get main command
  std::string main_command = cmd.get_argument(0);

  if (main_command == "remove_all_stream_operations") {
    reset_stream_operations(data.shared_object());
    return;
  }
  if (main_command == "remove_all_data") {
    reset_data(data.shared_object());
    return;
  }

  if (main_command == "remove_all") {
    reset_stream_operations(data.shared_object());
    reset_data(data.shared_object());
    return;
  }

  if (main_command == "add_queue_connection") {
    if (cmd.get_num_arguments() < 3) {
      error->set("Usage: add_queue_connections source_queue target_queue");
      return;
    }

    std::string queue_source = cmd.get_argument(1);
    std::vector<std::string> target_queues = au::split(cmd.get_argument(2), ' ');
    for (size_t i = 0; i < target_queues.size(); i++) {
      // Check if the connection exist
      if (data_exist_queue_connection(data.shared_object(),  queue_source, target_queues[i])) {
        continue;
      } else {
        data_create_queue_connection(data.shared_object(), queue_source, target_queues[i]);
      }
    }
    return;
  }

  if (main_command == "rm_queue_connection") {
    if (cmd.get_num_arguments() < 3) {
      error->set("Usage: rm_queue_connections source_queue target_queue");
      return;
    }

    std::vector<std::string> source_queues = au::split(cmd.get_argument(1), ' ');
    std::vector<std::string> target_queues = au::split(cmd.get_argument(2), ' ');

    for (size_t i = 0; i < source_queues.size(); i++) {
      for (size_t j = 0; j < target_queues.size(); j++) {
        data_remove_queue_connection(data.shared_object(), source_queues[i], target_queues[j]);
      }
    }

    return;
  }

  if (main_command == "batch") {
    if (cmd.get_num_arguments() < 2) {
      error->set("Usage: batch operation -input \"input1 input2\" -output \"outputs1 outputs2 output3\"");
      return;
    }

    std::string operation       = cmd.get_argument(1);
    std::string inputs          = cmd.GetFlagString("input");
    std::string outputs         = cmd.GetFlagString("output");

    size_t delilah_id = cmd.GetFlagUint64("delilah_id");
    size_t delilah_component_id = cmd.GetFlagUint64("delilah_component_id");

    std::vector<std::string> input_queues = au::split(inputs, ' ');
    std::vector<std::string> output_queues = au::split(outputs, ' ');

    std::string prefix = au::str(".%s_%lu_", au::code64_str(delilah_id).c_str(), delilah_component_id);

    // Push content to paralel queues
    for (size_t i = 0; i < input_queues.size(); i++) {
      std::string new_command = au::str("push_queue %s %s%s", input_queues[i].c_str(),
                                        prefix.c_str(), input_queues[i].c_str());
      PerformCommit(data, new_command, version, error);
      if (error->IsActivated()) {
        return;
      }
    }

    // Schedule stream operations
    std::string operation_name = au::str(".run_%s_%lu", au::code64_str(delilah_id).c_str());
    std::ostringstream new_command;

    new_command << "add_stream_operation " << operation_name << " " << operation << " ";
    new_command << "-input \"";
    for (size_t i = 0; i < input_queues.size(); i++) {
      new_command << prefix << input_queues[i];
    }
    new_command << "\"";
    new_command << "-output \"";
    for (size_t i = 0; i < input_queues.size(); i++) {
      new_command << prefix << output_queues[i];
    }
    new_command << "\"";

    PerformCommit(data, new_command.str(), version, error);
    if (error->IsActivated()) {
      return;
    }

    // Add the operation itself in the list
    gpb::BatchOperation *batch_operation = data->add_batch_operations();
    batch_operation->set_delilah_id(delilah_id);
    batch_operation->set_delilah_component_id(delilah_component_id);
    batch_operation->set_finished(false);
    batch_operation->set_operation(operation);

    for (size_t i = 0; i < input_queues.size(); i++) {
      batch_operation->add_inputs(input_queues[i]);
      gpb::KVInfo *info = batch_operation->add_input_info();

      gpb::Queue *queue =  get_queue(data.shared_object(), input_queues[i]);
      if (queue) {
        size_t num_blocks;
        size_t kvs;
        size_t size;
        getQueueInfo(*queue, &num_blocks, &kvs, &size);
        info->set_kvs(kvs);
        info->set_size(size);
      } else {
        info->set_kvs(0);
        info->set_size(0);
      }
    }


    for (size_t i = 0; i < output_queues.size(); i++) {
      batch_operation->add_outputs(output_queues[i]);
    }


    return;
  }

  if (main_command == "add_stream_operation") {
    if (cmd.get_num_arguments() < 3) {
      error->set(
        "Usage: add_stream_operation name operation -input \"input1 input2\" -output \"outputs1 outputs2 output3\"");
      return;
    }

    std::string name            = prefix + cmd.get_argument(1);
    std::string operation       = cmd.get_argument(2);
    std::string inputs          = cmd.GetFlagString("input");
    std::string outputs         = cmd.GetFlagString("output");

    // Get a new stream operation and increse the global counter
    size_t stream_operation_id = data->next_stream_operation_id();
    data->set_next_stream_operation_id(stream_operation_id + 1);

    // Create the new stream operation
    gpb::StreamOperation *stream_operation = gpb::getStreamOperation(data.shared_object(), name);

    if (stream_operation) {
      error->set(au::str("Stream operation %s already exist", name.c_str()));
      return;
    }

    // Create a new stream operation
    gpb::StreamOperation *so = data->add_operations();
    so->set_stream_operation_id(stream_operation_id);
    so->set_name(name);
    so->set_operation(operation);
    so->set_paused(false);

    // Add input queues
    au::CommandLine cmd_inputs(inputs);
    for (int i = 0; i < cmd_inputs.get_num_arguments(); i++) {
      so->add_inputs(cmd_inputs.get_argument(i));
    }

    // Add output queues
    au::CommandLine cmd_outputs(outputs);
    for (int i = 0; i < cmd_outputs.get_num_arguments(); i++) {
      so->add_outputs(cmd_outputs.get_argument(i));
    }

    so->set_reduce_forward(forward);
    so->set_reduce_update_only(update_only);

    error->AddMessage(au::str("StreamOperation %s added correctly", name.c_str()));
    return;
  }

  if (main_command == "rm_stream_operation") {
    std::string name            = prefix + cmd.get_argument(1);

    gpb::StreamOperation *stream_operation = gpb::getStreamOperation(data.shared_object(), name);
    if (!stream_operation) {
      error->set(au::str("Stream operation %s not found", name.c_str()));
      return;
    }

    gpb::removeStreamOperation(data.shared_object(), name);
    error->AddMessage(au::str("Stream operation %s removed correctly", name.c_str()));
    return;
  }


  if (main_command == "set_stream_operation_property") {
    if (cmd.get_num_arguments() < 4) {
      error->set("Usage: set_stream_operation_property name property value");
      return;
    }

    std::string name      = prefix + cmd.get_argument(1);
    std::string property  = cmd.get_argument(2);
    std::string value     = cmd.get_argument(3);


    gpb::StreamOperation *stream_operation = gpb::getStreamOperation(data.shared_object(), name);
    if (!stream_operation) {
      error->set(au::str("Stream operation %s not found", name.c_str()));
      return;
    }

    setProperty(stream_operation->mutable_environment(), property, value);
    error->AddMessage(au::str("Stream operation %s has been updated correctly", name.c_str()));
    return;
  }

  if (main_command == "unset_stream_operation_property") {
    if (cmd.get_num_arguments() < 3) {
      error->set("Usage: unset_stream_operation_property name property");
      return;
    }

    std::string name      = prefix + cmd.get_argument(1);
    std::string property  = cmd.get_argument(2);


    gpb::StreamOperation *stream_operation = gpb::getStreamOperation(data.shared_object(), name);
    if (!stream_operation) {
      error->set(au::str("Stream operation %s not found", name.c_str()));
      return;
    }

    unsetProperty(stream_operation->mutable_environment(), property);
    error->AddMessage(au::str("Stream operation %s has been updated correctly", name.c_str()));
    return;
  }

  if (main_command == "block") {
    // Main command to add or remove blocks to queues

    CommitCommand commit_command;
    commit_command.ParseCommitCommand(command, error);
    if (error->IsActivated()) {
      return;
    }

    // Perform all changes
    const au::vector<CommitCommandItem>& items = commit_command.items();
    for (size_t i = 0; i < items.size(); i++) {
      CommitCommandItem *item = items[i];

      if (item->command() == "add") {
        add_block(data.shared_object()
                  , item->queue()
                  , item->block_id()
                  , item->format()
                  , item->range()
                  , item->info()
                  , version
                  , error);

        if (error->IsActivated()) {
          return;
        }

        // add also to the connected queues
        au::StringVector connected_queues = data_get_queues_connected(data.shared_object(), item->queue());
        for (size_t i = 0; i < connected_queues.size(); i++) {
          add_block(data.shared_object()
                    , connected_queues[i]
                    , item->block_id()
                    , item->format()
                    , item->range()
                    , item->info()
                    , version
                    , error);
        }
      }

      if (item->command() == "rm") {
        rm_block(data.shared_object()
                 , item->queue()
                 , item->block_id()
                 , item->format()
                 , item->range()
                 , item->info()
                 , error);

        if (error->IsActivated()) {
          return;
        }
      }
    }

    // Blocks have been added or removed... review batch operation
    ReviewBatchOperations(data, version, error);
    if (error->IsActivated()) {
      return;
    }
  } else if (main_command == "add") {
    if (cmd.get_num_arguments() < 3) {
      error->set("Usage: add queue_name key_format value_format");
      return;
    }

    if (( cmd.get_num_arguments() == 3 ) && (cmd.get_argument(2) != "txt")) {
      error->set("Usage: add queue_name key_format value_format ( add queue_name txt )");
      return;
    }

    std::string name = cmd.get_argument(1);
    std::string key_format = cmd.get_argument(2);
    std::string value_format;
    if (cmd.get_num_arguments() == 3) {
      value_format = key_format;
    } else {
      value_format = cmd.get_argument(3);  // Get or create this queue
    }
    samson::gpb::get_or_create_queue(data.shared_object(), name, KVFormat(key_format, value_format), error);
    if (!error->IsActivated()) {
      error->AddMessage(au::str("Queue %s added correctly", name.c_str()));
    }
    return;
  } else if (main_command == "rm") {
    if (cmd.get_num_arguments() < 2) {
      error->set("Usage: rm queue_name queue_name2 ....");
      return;
    }

    for (int i = 1; i < cmd.get_num_arguments(); i++) {
      std::string name = cmd.get_argument(i);

      samson::gpb::Queue *queue = get_queue(data.shared_object(), name);
      if (!queue) {
        error->AddError(au::str("Queue %s does not exist already exists", name.c_str()));
      } else {
        removeQueue(data.shared_object(), name);
        error->AddMessage(au::str("Queue %s has been removed correctly", name.c_str()));
      }
    }
    return;
  } else if (main_command == "push_queue") {
    if (cmd.get_num_arguments() < 3) {
      error->set("Usage: push_queue source_queue target_queues");
      return;
    }

    samson::gpb::Queue *queue = get_queue(data.shared_object(), cmd.get_argument(1));
    if (!queue) {
      return;   // nothing to do
    }
    KVFormat format(queue->key_format(), queue->value_format());
    samson::gpb::Queue *target_queue = get_or_create_queue(data.shared_object(), cmd.get_argument(2), format, error);

    if (error->IsActivated()) {
      return;
    }

    // Copy all the conten
    for (int i  = 0; i  < queue->blocks_size(); i++) {
      target_queue->add_blocks()->CopyFrom(queue->blocks(i));
    }

    return;
  } else if (main_command == "set_queue_property") {
    error->set("Still not implemented");
    return;
  } else if (main_command == "set_queue_property") {
    error->set("Still not implemented");
    return;
  } else {
    LM_W(("Unknown command (%s) in the commit to data model", main_command.c_str()));
  }
}

bool DataModel::isValidCommand(const std::string& main_command) {
  if (main_command == "add") {
    return true;
  }
  if (main_command == "rm") {
    return true;
  }
  if (main_command == "push_queue") {
    return true;
  }
  if (main_command == "remove_all_data") {
    return true;
  }
  if (main_command == "remove_all") {
    return true;
  }

  if (main_command == "add_stream_operation") {
    return true;
  }
  if (main_command == "rm_stream_operation") {
    return true;
  }
  if (main_command == "set_stream_operation_property") {
    return true;
  }
  if (main_command == "unset_stream_operation_property") {
    return true;
  }
  if (main_command == "remove_all_stream_operations") {
    return true;
  }
  if (main_command == "add_queue_connection") {
    return true;
  }
  if (main_command == "rm_queue_connection") {
    return true;
  }

  if (main_command == "batch") {
    return true;
  }

  return false;
}

// Get collections for all the operations
gpb::Collection *DataModel::getCollectionForStreamOperations(const Visualization& visualization) {
  // Get a zopy of the current version
  au::SharedPointer<gpb::Data> data = getCurrentModel();

  bool all_flag = visualization.get_flag("a");

  gpb::Collection *collection = new gpb::Collection();
  collection->set_name("stream_operations");

  for (size_t i = 0; i < (size_t)data->operations_size(); i++) {
    const gpb::StreamOperation & stream_operation = data->operations(i);

    std::string name = stream_operation.name();

    if (name.length() == 0) {
      continue;
    }
    if (!all_flag && name[0] == '.') {
      continue;
    }
    gpb::CollectionRecord *record = collection->add_record();

    ::samson::add(record, "id", stream_operation.stream_operation_id(), "different");
    ::samson::add(record, "name", stream_operation.name(), "different");
    ::samson::add(record, "operation", stream_operation.operation(), "different");

    std::ostringstream inputs;
    for (int j = 0; j < stream_operation.inputs_size(); j++) {
      inputs << stream_operation.inputs(j) << " ";
    }

    std::ostringstream outputs;
    for (int j = 0; j < stream_operation.outputs_size(); j++) {
      outputs << stream_operation.outputs(j) << " ";
    }

    ::samson::add(record, "inputs", inputs.str(), "different");
    ::samson::add(record, "outputs", outputs.str(), "different");

    ::samson::add(record, "environment", str(stream_operation.environment()), "different");
  }

  return collection;
}

// Get collections for all the operations
gpb::Collection *DataModel::getCollectionForBatchOperations(const Visualization& visualization) {
  // Get a zopy of the current version
  au::SharedPointer<gpb::Data> data = getCurrentModel();

  gpb::Collection *collection = new gpb::Collection();
  collection->set_name("batch_operations");

  for (size_t i = 0; i < (size_t)data->batch_operations_size(); i++) {
    const gpb::BatchOperation & batch_operation = data->batch_operations(i);

    gpb::CollectionRecord *record = collection->add_record();


    size_t delilah_id = batch_operation.delilah_id();
    size_t delilah_component_id = batch_operation.delilah_component_id();
    std::string name = au::str("%s %lu"
                               , au::code64_str(delilah_id).c_str()
                               , delilah_component_id
                               );

    ::samson::add(record, "delilah", name, "different");
    ::samson::add(record, "operation", batch_operation.operation(), "different");

    if (batch_operation.finished()) {
      ::samson::add(record, "finish", "YES", "different");
    } else {
      ::samson::add(record, "finish", "NO", "different");
    } std::ostringstream inputs;
    std::ostringstream inputs_data;
    for (int j = 0; j < batch_operation.inputs_size(); j++) {
      inputs << batch_operation.inputs(j) << " ";
      inputs_data << KVInfo(batch_operation.input_info(i).size(), batch_operation.input_info(i).kvs()).str() << " ";
    }

    // Prefix for all queues
    std::string prefix = au::str(".%s_%lu_", au::code64_str(delilah_id).c_str(), delilah_component_id);

    std::ostringstream outputs;
    std::ostringstream outputs_data;
    for (int j = 0; j < batch_operation.outputs_size(); j++) {
      outputs << batch_operation.outputs(j) << " ";

      std::string queue_name = prefix + batch_operation.outputs(j);

      gpb::Queue *queue =  get_queue(data.shared_object(), queue_name);
      if (queue) {
        size_t num_blocks;
        size_t kvs;
        size_t size;
        getQueueInfo(*queue, &num_blocks, &kvs, &size);

        outputs_data << KVInfo(size, kvs).str() << " ";
      }
    }

    ::samson::add(record, "inputs", inputs.str(), "different");
    ::samson::add(record, "outputs", outputs.str(), "different");

    // Data information
    ::samson::add(record, "Input data", inputs_data.str(), "different");
    ::samson::add(record, "Output data", outputs_data.str(), "different");

    // Data for the output
  }

  return collection;
}

gpb::Collection *DataModel::getCollectionForQueuesWithBlocks(const Visualization& visualization) {
  // Get a copy of the current node
  au::SharedPointer<gpb::Data> data = getCurrentModel();

  bool all_flag = visualization.get_flag("a");

  gpb::Collection *collection = new gpb::Collection();
  collection->set_name("block_queues");

  for (int q = 0; q < data->queue_size(); q++) {
    const gpb::Queue& queue = data->queue(q);

    for (int b = 0; b < queue.blocks_size(); b++) {
      const gpb::Block& block = queue.blocks(b);

      if (queue.name().length() == 0) {
        continue;
      }
      if (!all_flag && queue.name()[0] == '.') {
        continue;
      }
      gpb::CollectionRecord *record = collection->add_record();

      ::samson::add(record, "queue", queue.name(), "different");
      ::samson::add(record, "block", block.block_id(), "different");

      KVRanges ranges = block.ranges();   // Implicit conversion

      ::samson::add(record, "ranges", ranges.str(), "different");

      KVInfo info(block.size(), block.kvs());

      ::samson::add(record, "info", info.str(), "different");
      ::samson::add(record, "commit", block.commit_id(), "different");
      ::samson::add(record, "time", block.time(), "f=timestamp");
    }
  }
  return collection;
}

gpb::Collection *DataModel::getCollectionForQueueConnections(const Visualization& visualization) {
  // Get a copy of the current node
  au::SharedPointer<gpb::Data> data = getCurrentModel();

  bool all_flag = visualization.get_flag("a");

  // Get all source queues
  std::set<std::string> source_queues;
  for (size_t i = 0; i < (size_t)data->queue_connections_size(); i++) {
    std::string queue_name = data->queue_connections(i).queue_source();

    if (queue_name.length() == 0) {
      continue;
    }
    if (!all_flag && queue_name[0] == '.') {
      continue;
    }
    source_queues.insert(queue_name);
  }

  gpb::Collection *collection = new gpb::Collection();
  collection->set_name("queues_connections");

  std::set<std::string>::iterator it;
  for (it = source_queues.begin(); it != source_queues.end(); it++) {
    // Target queues for this source quuee
    au::StringVector target_queues = data_get_queues_connected(data.shared_object(), *it);

    gpb::CollectionRecord *record = collection->add_record();
    ::samson::add(record, "source", *it, "different");
    ::samson::add(record, "target", target_queues.str(), "different");
  }
  return collection;
}

gpb::Collection *DataModel::getCollectionForQueues(const Visualization& visualization) {
  // Get a copy of the current node
  au::SharedPointer<gpb::Data> data = getCurrentModel();

  bool all_flag = visualization.get_flag("a");

  gpb::Collection *collection = new gpb::Collection();
  collection->set_name("queues");

  for (size_t q = 0; q < (size_t)data->queue_size(); q++) {
    const gpb::Queue& queue = data->queue(q);

    std::string name = queue.name();

    if (name.length() == 0) {
      continue;
    }
    if (!all_flag && name[0] == '.') {
      continue;
    }
    size_t kvs;
    size_t size;
    size_t num_blocks;

    getQueueInfo(queue, &num_blocks, &kvs, &size);


    gpb::CollectionRecord *record = collection->add_record();
    ::samson::add(record, "name", queue.name(), "different");

    std::string format = au::str("%s-%s"
                                 , queue.key_format().c_str()
                                 , queue.value_format().c_str());

    ::samson::add(record, "format", format, "different");

    ::samson::add(record, "#blocks", num_blocks, "different,f=uint64");
    ::samson::add(record, "#kvs", kvs, "different,f=uint64");
    ::samson::add(record, "size", size, "different,f=uint64");
  }

  return collection;
}

std::set<size_t> DataModel::get_block_ids() {
  // Prepare list of ids to be returned
  std::set<size_t> block_ids;

  // Get a copy of the data
  au::SharedPointer<gpb::Data> my_data = getCurrentModel();

  for (int q = 0; q < my_data->queue_size(); q++) {
    const gpb::Queue& queue = my_data->queue(q);
    for (int b = 0; b < queue.blocks_size(); b++) {
      block_ids.insert(queue.blocks(b).block_id());
    }
  }

  return block_ids;
}

std::set<size_t> DataModel::get_my_block_ids(const KVRanges& hg_ranges) {
  // Prepare list of ids to be returned
  std::set<size_t> block_ids;

  // Get a copy of the data
  au::SharedPointer<gpb::Data> my_data = getCurrentModel();

  // Loop all the queues
  for (int q = 0; q < my_data->queue_size(); q++) {
    const gpb::Queue& queue = my_data->queue(q);
    for (int b = 0; b < queue.blocks_size(); b++) {
      const gpb::Block& block = queue.blocks(b);

      // Implicit convesion to C++ type
      KVRanges ranges = block.ranges();

      if (ranges.IsOverlapped(hg_ranges)) {
        block_ids.insert(block.block_id());
      }
    }
  }

  return block_ids;
}

void DataModel::ReviewBatchOperations(au::SharedPointer<gpb::Data> data, int version,  au::ErrorManager *error) {
  for (int i = 0; i < data->batch_operations_size(); i++) {
    gpb::BatchOperation *batch_operation = data->mutable_batch_operations(i);

    if (!batch_operation->finished()) {
      batch_operation->set_finished(true);

      size_t delilah_id = batch_operation->delilah_id();
      size_t delilah_component_id = batch_operation->delilah_component_id();
      std::string prefix = au::str(".%s_%lu_", au::code64_str(delilah_id).c_str(), delilah_component_id);

      // Perform all push for output queues
      for (int j = 0; j < batch_operation->outputs_size(); j++) {
        std::string queue_name = batch_operation->outputs(j);
        std::string final_queue_name = prefix + queue_name;
        std::string command = au::str("push_queue %s %s", final_queue_name.c_str(), queue_name.c_str());
        PerformCommit(data, command, version, error);
        if (error->IsActivated()) {
          return;
        }
      }
    }
  }
}
}  // namespace samson
