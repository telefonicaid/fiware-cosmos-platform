
#include "samson/common/coding.h"

#include "Block.h"              // samson::Stream::Block
#include "BlockManager.h"       // samson::stream::BlockManager

#include "StreamOperationInfo.h"
#include "samson/common/KVInputVector.h"
#include "samson/isolated/ProcessWriter.h"
#include "samson/worker/SamsonWorker.h"
#include "samson/zoo/CommitCommand.h"

#include "WorkerTask.h"      // Own interface

namespace samson {
namespace stream {
ProcessIsolated::ProcessBaseType get_type(Operation *operation) {
  switch (operation->getType()) {
    case Operation::parserOut:
    case Operation::parserOutReduce:
      return ProcessIsolated::txt;

    default:
      return ProcessIsolated::key_value;
  }
}

WorkerTask::WorkerTask(SamsonWorker *samson_worker
                       , size_t id
                       , const gpb::StreamOperation& stream_operation
                       , Operation *operation
                       , KVRange range)
  : ProcessIsolated(au::str("WorkerTask %lu: %s", id, stream_operation.operation().c_str()), get_type(operation))
    , WorkerTaskBase(id) {
  // Keep a pointer to samson_worker to create output blocks
  samson_worker_ = samson_worker;

  // Some environment variables
  environment_.set("system.task_id", id);
  set_process_item_description(stream_operation.operation());

  // Set output formats for this operation
  addOutputsForOperation(operation);

  // Keep a pointer to the operation
  operation_ = operation;

  // keep the working range
  range_ = range;

  // Copy information about stream operation to be executed
  stream_operation_ = new gpb::StreamOperation();
  stream_operation_->CopyFrom(stream_operation);

  finished_ = false;     // Default value for this flag
}

WorkerTask::~WorkerTask() {
  delete stream_operation_;
}

void WorkerTask::fill(samson::gpb::CollectionRecord *record, const Visualization& visualization) {
  if (visualization.get_flag("data")) {
    add(record, "id", get_id(), "left,different");

    for (int i = 0; i < 3; i++) {
      BlockList *block_list = block_list_container_.getBlockList(au::str("input_%d", i));
      BlockInfo block_info = block_list->getBlockInfo();
      add(record, au::str("Input %d", i), block_info.strShort(), "left,different");
    }
    for (int i = 0; i < 3; i++) {
      BlockList *block_list = block_list_container_.getBlockList(au::str("output_%d", i));
      BlockInfo block_info = block_list->getBlockInfo();
      add(record, au::str("Output %d", i), block_info.strShort(), "left,different");
    }

    return;
  }

  add(record, "id", get_id(), "left,different");
  add(record, "worker_command_id", environment_.get("worker_command_id", "?"), "left,different");
  add(record, "creation", creation_cronometer_.seconds(), "f=time,different");
  add(record, "running ", cronometer().seconds(), "f=time,different");
  add(record, "progress ", progress(), "f=percentadge,different");

  if (ProcessItem::running())
    add(record, "state", "running", "left,different"); else
    add(record, "state", task_state(), "left,different"); add(record, "operation", stream_operation_->operation(), "left,different");


  /*
   * // Collect information from inputs / outputs
   * add( record , "input_0" , getBlockList("input_0")->getBlockInfo().strShort() , "different");
   * add( record , "all_inputs" , getUniqueBlockInfo().strShort()  , "different");
   */
}

void WorkerTask::processOutputBuffer(engine::BufferPointer buffer, int output, int hg_division, bool finish) {
  // Information for generated block
  KVHeader *header = (KVHeader *)buffer->getData();

  // Create a block ( and distribute it )
  size_t block_id = samson_worker_->distribution_blocks_manager->CreateBlock(buffer);
  BlockPointer block = BlockManager::shared()->getBlock(block_id);

  // Add output to this operation
  add_output(output, block, header->range, header->info);
}

void WorkerTask::processOutputTXTBuffer(engine::BufferPointer buffer, bool finish) {
  // Information for generated block
  KVHeader *header = (KVHeader *)buffer->getData();

  // Create a block ( and distribute it )
  size_t block_id = samson_worker_->distribution_blocks_manager->CreateBlock(buffer);
  BlockPointer block = BlockManager::shared()->getBlock(block_id);

  // Add output to this operation
  // Always on channel 0
  add_output(0, block, header->range, header->info);
}

void WorkerTask::sendBufferToQueue(engine::BufferPointer buffer, int outputWorker, std::string queue_name) {
  LM_X(1, ("Unimplemented"));
  /*
   *
   * if(!buffer)
   * return;
   *
   * if( queue_name == "null")
   * return;
   *
   * // Select the target worker_id
   * // ------------------------------------------------------------------------------------
   * size_t target_worker_id;
   * if( outputWorker == -1 )
   * {
   * // To my self
   * target_worker_id = distribution_information.get_my_worker_id();
   * }
   * else
   * {
   *
   * if ( ( outputWorker < 0 ) || ( outputWorker > (int)distribution_information.workers.size() ) )
   * LM_X(1, ("Non valid worker %d (#workers %lu) when seding buffer to queue %s" , outputWorker , distribution_information.workers.size() , queue_name.c_str() ) );
   * target_worker_id = distribution_information.workers[ outputWorker ];
   * }
   *
   * // ------------------------------------------------------------------------------------
   * // Sent the packet
   * // ------------------------------------------------------------------------------------
   *
   * Packet* packet = new Packet( Message::PushBlock );
   * packet->set_buffer( buffer );    // Set the buffer of data
   * packet->message->set_delilah_component_id( 0 );
   *
   * network::PushBlock* pb =  packet->message->mutable_push_block();
   * pb->set_size( buffer->getSize() );
   *
   * std::vector<std::string> queue_names = au::split( queue_name , ',' );
   * for ( size_t i = 0 ; i < queue_names.size() ; i++)
   * {
   * pb->add_queue( queue_names[i] );
   * }
   *
   *
   * // Direction to send packet
   * packet->to.node_type = WorkerNode;
   * //packet->to.id = target_worker_id;
   *
   * // Send packet
   * //distribution_information.network->send( packet );
   *
   * // Release created packet
   * packet->Release();
   *
   */
}

std::string WorkerTask::commit_command() {
  // Get the commit command for this stream operation
  CommitCommand commit_command;

  // Input elements
  for (int c = 0; c < stream_operation_->inputs_size(); c++) {
    BlockList *block_list = block_list_container_.getBlockList(au::str("input_%d", c));
    au::list< BlockRef >::iterator it;
    for (it = block_list->blocks.begin(); it != block_list->blocks.end(); it++) {
      BlockRef *block_ref = *it;

      commit_command.RemoveBlock(stream_operation_->inputs(c)
                                 , block_ref->block_id()
                                 , operation_->inputFormats[c]
                                 , block_ref->range()
                                 , block_ref->info());
    }
  }

  // Output elements
  for (int c = 0; c < stream_operation_->outputs_size(); c++) {
    BlockList *block_list = block_list_container_.getBlockList(au::str("output_%d", c));
    au::list< BlockRef >::iterator it;
    for (it = block_list->blocks.begin(); it != block_list->blocks.end(); it++) {
      BlockRef *block_ref = *it;

      commit_command.AddBlock(stream_operation_->outputs(c)
                              , block_ref->block_id()
                              , operation_->outputFormats[c]
                              , block_ref->range()
                              , block_ref->info());
    }
  }


  // Gerate the commit command
  return commit_command.GetCommitCommand();
}

void WorkerTask::initProcessIsolated() {
  LM_W(("Init worker task %lu", get_id()));

  // Review input blocks to count key-values
  for (int i = 0; i < operation_->getNumInputs(); i++) {
    BlockList *list = block_list_container_.getBlockList(au::str("input_%d", i));
    list->ReviewBlockReferences(error_);
  }

  LM_W(("Finish Init worker task %lu", get_id()));
}

void WorkerTask::generateKeyValues(samson::ProcessWriter *writer) {
  switch (operation_->getType()) {
    case Operation::parser:
      generateKeyValues_parser(writer);
      break;

    case Operation::map:
      generateKeyValues_map(writer);
      break;

    case Operation::reduce:
      generateKeyValues_reduce(writer);
      break;

    default:
      LM_W(("Operation still not implemented"));
      break;
  }
}

void WorkerTask::generateTXT(TXTWriter *writer) {
  // Handy class to emit traces
  // OperationTraces operation_traces( au::str( "[%lu] Parserout %s" , id,  operation_name.c_str() ) , getUniqueBlockInfo().size );
  OperationTraces operation_traces(au::str("[%lu] Parserout %s", get_id(), process_item_description().c_str()), 100);

  // Type of inputs ( for selecting key-values )
  std::vector<KVFormat> inputFormats =  operation_->getInputFormats();

  if (inputFormats.size() != 1) {
    setUserError("Parser with more that one input... that is not valid!");
    return;
  }

  // Run the generator over the ProcessWriter to emit all key-values
  ParserOut *parserOut = (ParserOut *)operation_->getInstance();

  if (!parserOut) {
    setUserError("Error getting an instance of this operation");
    return;
  }

  KVInputVector inputVector(operation_);


  parserOut->environment = &operation_environment;
  parserOut->tracer = this;
  parserOut->operationController = this;

  parserOut->init(writer);

  BlockList *list = block_list_container_.getBlockList("input_0");

  au::list< BlockRef >::iterator bi;
  for (bi = list->blocks.begin(); bi != list->blocks.end(); bi++) {
    BlockRef *block_ref = *bi;
    BlockPointer block = block_ref->block();
    engine::BufferPointer buffer = block->buffer();

    // KV File to access content of this block
    au::SharedPointer<KVFile> file = block_ref->file();
    if (file == NULL) {
      setUserError("Error getting information about this block");
      return;
    }

    for (int hg = range_.hg_begin; hg < range_.hg_end; hg++) {
      KVInfo info = file->info[hg];

      if (info.size > 0) {
        inputVector.prepareInput(info.kvs);
        inputVector.addKVs(0, info, file->kvs_for_hg(hg));

        KVSetStruct inputStruct;
        inputStruct.num_kvs = inputVector.num_kvs;
        inputStruct.kvs = inputVector._kv;

        parserOut->run(&inputStruct, writer);
      }
    }
  }

  parserOut->finish(writer);

  // Detele the created instance
  delete parserOut;
}

void WorkerTask::generateKeyValues_map(samson::ProcessWriter *writer) {
  // Handy class to emit traces
  OperationTraces operation_traces(au::str("[%lu] Map %s", get_id(),  process_item_description().c_str()), 100);

  // Type of inputs ( for selecting key-values )
  std::vector<KVFormat> inputFormats =  operation_->getInputFormats();

  if (inputFormats.size() != 1) {
    setUserError("Map with more that one input.");
    return;
  }

  // Input vector of key-values
  KVInputVector inputVector(operation_);

  // Get an instance of this operation
  Map *map = (Map *)operation_->getInstance();

  if (!map) {
    setUserError("Error getting an instance of this operation");
    return;
  }

  // Prepare opertion to be executed
  map->environment = &operation_environment;
  map->tracer = this;
  map->operationController = this;

  // Call the init function
  map->init(writer);

  BlockList *list = block_list_container_.getBlockList("input_0");

  au::list< BlockRef >::iterator bi;
  for (bi = list->blocks.begin(); bi != list->blocks.end(); bi++) {
    BlockRef *block_ref = *bi;
    BlockPointer block = block_ref->block();
    engine::BufferPointer buffer = block->buffer();

    // Check header for valid block
    KVHeader *header = (KVHeader *)buffer->getData();
    if (!header->check()) {
      setUserError(("Not valid header in block refernce"));
      return;
    }

    // Analyse all key-values and hashgroups
    au::SharedPointer<KVFile> file = block_ref->file();
    if (file == NULL) {
      setUserError("Error getting information about this block");
      return;
    }

    // Read key values
    inputVector.prepareInput(header->info.kvs);

    for (int hg = range_.hg_begin; hg < range_.hg_end; hg++) {
      inputVector.addKVs(0, file->info[hg], &file->kvs[ file->kvs_index[hg] ]);
    }

    // inputVector.addKVs( 0 , header->info ,  file->dataForHashGroup( range.hg_begin ) );

    KVSetStruct inputStruct;
    inputStruct.num_kvs = inputVector.num_kvs;
    inputStruct.kvs = inputVector._kv;

    map->run(&inputStruct, writer);
  }

  map->finish(writer);

  // Detele the created instance
  delete map;
}

#pragma mark BlockReaderList

// Class to process a block as input for a reduce operation

class BlockReader {
public:

  static au::SharedPointer<BlockReader> create(BlockRef *block_ref, int channel, au::ErrorManager& error) {
    au::SharedPointer<BlockReader> block_reader(new BlockReader());       // Candidate instance

    block_reader->channel_ = channel;

    // Get pointer to the block
    BlockPointer block = block_ref->block();

    if (!block->is_content_in_memory()) {
      error.set("Content of block is not in memory");
      return au::SharedPointer<BlockReader>(NULL);
    }

    // Create the kv_file
    block_reader->kv_file_ = KVFile::create(block->buffer(), error);

    if (error.IsActivated())
      return au::SharedPointer<BlockReader>(NULL);

    return block_reader;
  }

  au::SharedPointer<KVFile> kv_file() {
    return kv_file_;
  }

  int channel() {
    return channel_;
  }

private:

  // Private constructor since instances are create with create method
  BlockReader() {
  }

  au::SharedPointer<KVFile> kv_file_;       // Internal management of data
  int channel_;                             // Input channel associated to this block
};


class BlockReaderCollection {
public:

  BlockReaderCollection(Operation *operation) : kvVector_(operation) {
    // Keep operation for sorting
    operation_ = operation;
  }

  ~BlockReaderCollection() {
    // Not necessary to clear vector since they are shared pointers
  }

  void AddInputBlocks(BlockRef *block_ref, int channel) {
    if (( channel < 0 ) || ( channel > ( operation_->getNumInputs() - 1 )))
      LM_X(1, ("Internal error")); au::ErrorManager error;
    au::SharedPointer<BlockReader> block_reader = BlockReader::create(block_ref, channel, error);

    if (error.IsActivated())
      // Still not supported how to handle this error nicely
      LM_X(1, ("Error crearing a block reader: %s", error.GetMessage().c_str()));
    input_block_readers_.push_back(block_reader);
  }

  void AddStateBlocks(BlockRef *block_ref, int channel) {
    if (( channel < 0 ) || ( channel > ( operation_->getNumInputs() - 1 )))
      LM_X(1, ("Internal error")); au::ErrorManager error;
    au::SharedPointer<BlockReader> block_reader = BlockReader::create(block_ref, channel, error);

    if (error.IsActivated())
      // Still not supported how to handle this error nicely
      LM_X(1, ("Error crearing a block reader: %s", error.GetMessage().c_str()));
    state_block_readers_.push_back(block_reader);
  }

  size_t prepare(int hg) {
    // Getting the number of key-values for input
    input_num_kvs_ = 0;
    for (size_t i = 0; i < input_block_readers_.size(); i++) {
      input_num_kvs_ += input_block_readers_[i]->kv_file()->info[hg].kvs;
    }

    // Getting the number of key-values for state
    state_num_kvs_ = 0;
    for (size_t i = 0; i < state_block_readers_.size(); i++) {
      input_num_kvs_ += state_block_readers_[i]->kv_file()->info[hg].kvs;
    }

    // Total number of key-values
    size_t total_kvs = input_num_kvs_ + state_num_kvs_;

    // Prepare KV Vector with the total number of kvs ( from input and state )
    kvVector_.prepareInput(total_kvs);

    // Add key values for all the inputs
    if (input_num_kvs_ > 0)
      for (size_t i = 0; i < input_block_readers_.size(); i++) {
        kvVector_.addKVs(input_block_readers_[i]->channel()
                         , input_block_readers_[i]->kv_file()->info[hg]
                         , input_block_readers_[i]->kv_file()->kvs_for_hg(hg)
                         );
      }
    // Add key values for the state
    if (state_num_kvs_ > 0)
      for (size_t i = 0; i < state_block_readers_.size(); i++) {
        kvVector_.addKVs(state_block_readers_[i]->channel()
                         , state_block_readers_[i]->kv_file()->info[hg]
                         , state_block_readers_[i]->kv_file()->kvs_for_hg(hg)
                         );
      }
    return total_kvs;
  }

  void sortAndInit() {
    if (state_num_kvs_ == 0) // No state

      // LM_M(("Sorting %lu key-values" , input_num_kvs));
      kvVector_.sort(); else if (input_num_kvs_ > 0)
      // LM_M(("Merge-Sorting %lu / %lu key-values" , input_num_kvs , state_num_kvs));
      kvVector_.sortAndMerge(input_num_kvs_);
    kvVector_.init();
  }

  KVSetStruct *getNext() {
    return kvVector_.getNext();
  }

private:

  // Collection of BlockReader's ( each for every block used as input for this operation )
  au::Vector<BlockReader> input_block_readers_;

  // Collection of BlockReader's ( each for every block used in the state )
  // Note: Only one state at the moment
  au::Vector< BlockReader> state_block_readers_;

  // Common structure to give data to 3rd party software
  KVInputVector kvVector_;

  size_t input_num_kvs_;
  size_t state_num_kvs_;

  // Operation used ( necessary for sorting )
  Operation *operation_;
};


#pragma mark



void WorkerTask::generateKeyValues_reduce(samson::ProcessWriter *writer) {
  bool update_only = stream_operation_->has_reduce_update_only() && stream_operation_->reduce_update_only();
  bool reduce_forward = stream_operation_->has_reduce_forward() && stream_operation_->reduce_forward();

  if (update_only)
    LM_T(LmtReduceOperation, ("Reduce %lu - %s begin.... (update_only)", get_id(), process_item_description().c_str())); else
    LM_T(LmtReduceOperation,
         ("Reduce %lu - %s begin", get_id(),
          process_item_description().c_str())); LM_T(LmtReduceOperation,
                                                     ("Reduce %lu - %s KVRange %s", get_id(), process_item_description().c_str(),
                                                    range_.str().c_str()));

  // Handy class to emit traces
  // OperationTraces operation_traces( au::str( "[%lu] reduce %s" , id,  operation_name.c_str() ) , getUniqueBlockInfo().size );
  OperationTraces operation_traces(au::str("[%lu] reduce %s", get_id(),  process_item_description().c_str()), 100);

  // Get the operation instance
  Reduce *reduce = (Reduce *)operation_->getInstance();

  // Prepare the operation
  reduce->environment = &operation_environment;
  reduce->tracer = this;
  reduce->operationController = this;

  // Init function
  reduce->init(writer);


  // Get the block reader list to prepare inputs for operation
  BlockReaderCollection blockreaderCollection(operation_);

  // Insert all the blocks involved in this operation
  for (int i = 0; i < operation_->getNumInputs(); i++) {
    BlockList *list = block_list_container_.getBlockList(au::str("input_%d", i));

    au::list< BlockRef >::iterator bi;
    for (bi = list->blocks.begin(); bi != list->blocks.end(); bi++) {
      BlockRef *block_ref = *bi;
      // BlockPointer block = block_ref->block();
      // engine::BufferPointer buffer = block->buffer();

      if (!reduce_forward && (i == (operation_->getNumInputs() - 1)))
        blockreaderCollection.AddStateBlocks(block_ref, i); else
        blockreaderCollection.AddInputBlocks(block_ref, i);
    }
  }

  // Counters for statistics
  int total_hgs =  range_.hg_end - range_.hg_begin;
  int processed_hds = 0;

  int transfered_states = 0;
  int processed_states = 0;

  LM_T(LmtReduceOperation, ("Reduce %lu - %s Data ready! Running hash-groups", get_id(), process_item_description().c_str()));

  for (int hg = 0; hg < KVFILE_NUM_HASHGROUPS; hg++) {
    // Check if this is inside the range we are interested in processing
    if (( hg >= range_.hg_begin ) && (hg < range_.hg_end)) {
      // Report progress to show in the lists
      processed_hds++;
      reportProgress((double)processed_hds / (double)total_hgs);

      // Prepare the blockReaderCollection for thi hash-group
      size_t num_kvs = blockreaderCollection.prepare(hg);

      // sort input key-values and init the kvVector structure
      blockreaderCollection.sortAndInit();

      if (num_kvs > 0) {
        // Get data for the next reduce operation
        KVSetStruct *inputStructs = blockreaderCollection.getNext();

        while (inputStructs) {
          if (update_only && (inputStructs[0].num_kvs == 0)) {
            // Just copy state to the output...
            if (inputStructs[1].num_kvs > 0) {    // Check we really have state
              // Satte out
              int state_output = operation_->getNumOutputs() - 1;

              // Data pointer
              char *key_data = inputStructs[1].kvs[0]->key;
              // char * value_data = inputStructs[1].kvs[0]->value;

              // Total size
              size_t key_size = inputStructs[1].kvs[0]->key_size;
              size_t value_size = inputStructs[1].kvs[0]->value_size;

              // inputStructs[1].kvs[0]->key
              writer->internal_emit(state_output, hg, key_data, key_size + value_size);
              transfered_states++;
            } else {
              LM_T(LmtReduceOperation, ("Reduce %lu - %s ERROR since no data at input nor state"
                                        , get_id()
                                        , process_item_description().c_str()
                                        ));
            }
          } else {
            // Call the reduce operation
            reduce->run(inputStructs, writer);
            processed_states++;
          }

          // Get the next one
          inputStructs = blockreaderCollection.getNext();
        }
      }
    }
  }

  reduce->finish(writer);

  // Detele the created instance
  delete reduce;

  // LM_M(("Finish ReduceWorkerTask for range %s  [%s]" , range.str().c_str() , block_info.str().c_str() ));
  LM_T(LmtReduceOperation, ("Reduce %lu - %s finish [ Different key Processed: %d DirectState: %d ]"
                            , get_id()
                            , process_item_description().c_str()
                            , processed_states
                            , transfered_states
                            ));
}

void WorkerTask::generateKeyValues_parser(samson::ProcessWriter *writer) {
  size_t id = get_id();

  // Handy class to emit traces
  OperationTraces operation_traces(au::str("[%lu] Parser %s", id,  stream_operation_->operation().c_str()), 100);

  // Run the generator over the ProcessWriter to emit all key-values
  Parser *parser = (Parser *)operation_->getInstance();

  if (!parser) {
    setUserError("Error getting an instance of this operation");
    return;
  }

  parser->environment = &operation_environment;
  parser->tracer = this;
  parser->operationController = this;

  parser->init(writer);

  // Recover the input list "input_0"
  BlockList *list = block_list_container_.getBlockList("input_0");

  au::list< BlockRef >::iterator bi;
  for (bi = list->blocks.begin(); bi != list->blocks.end(); bi++) {
    BlockRef *block_ref = *bi;
    BlockPointer block = block_ref->block();
    engine::BufferPointer buffer = block->buffer();

    // Pointer to the internal data in the buffer
    char *data = buffer->getData() + sizeof( KVHeader );
    size_t size = buffer->getSize() - sizeof( KVHeader );

    // Trace
    operation_traces.trace_block(buffer->getSize());

    // Run parser with this data
    parser->run(data, size,  writer);
  }

  parser->finish(writer);

  // Detele the created instance
  delete parser;
}

std::string WorkerTask::getStatus() {
  std::ostringstream output;

  output << "[" << get_id() << "] ";
  output << "StreamOperation " << stream_operation_->name();
  output << " Operation " << stream_operation_->operation();
  return output.str();
}

void WorkerTask::set_finished() {
  finished_ = true;
}

void WorkerTask::set_finished_with_error(const std::string& error_message) {
  error_.set(error_message);
  finished_ = true;
}

bool WorkerTask::is_finished() {
  return finished_;
}

const au::ErrorManager& WorkerTask::error() {
  return error_;
}

std::string WorkerTask::str() {
  return au::str("Task %lu : %s"
                 , get_id()
                 , stream_operation_->name().c_str());
}
}
}
