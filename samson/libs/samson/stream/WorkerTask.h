#ifndef _H_SAMSON_QUEUE_TASK
#define _H_SAMSON_QUEUE_TASK

#include <set>
#include <set>                            // std::set
#include <sstream>

#include "au/S.h"
#include "au/string.h"                    // au::Format

#include "engine/ProcessItem.h"           // engine::ProcessItem

#include "samson/common/MessagesOperations.h"
#include "samson/isolated/ProcessIsolated.h"
#include "samson/stream/Block.h"          // samson::Stream::Block
#include "samson/stream/BlockInfo.h"      // struct BlockInfo
#include "samson/stream/BlockList.h"      // stream::BlockList
#include "samson/stream/WorkerTaskBase.h"  // parent class samson::stream::WorkerTaskBase

namespace samson {
class SamsonWorker;
namespace stream {
class Block;


// Base class for all the stream tasks ( parser , map , reduce , parseOut )
class WorkerTask : public ::samson::ProcessIsolated, public WorkerTaskBase {
public:

  // Constructor
  WorkerTask(SamsonWorker *samson_worker
             , size_t id
             , const gpb::StreamOperation& stream_operation
             , Operation *operation
             , KVRange range);

  ~WorkerTask();

  // Virtual methods of samson::ProcessIsolated
  virtual void initProcessIsolated();
  virtual void generateKeyValues(samson::ProcessWriter *writer);
  virtual void generateTXT(TXTWriter *writer);
  virtual void processOutputBuffer(engine::BufferPointer buffer, int output, int hg_division, bool finish);
  virtual void processOutputTXTBuffer(engine::BufferPointer buffer, bool finish);


  // Commit command to use when this operation finish
  std::string commit_command();

  // Get information of the current process
  std::string getStatus();

  // Finish methids
  void set_finished();
  void set_finished_with_error(const std::string& error_message);

  // Check status
  const au::ErrorManager& error();
  bool is_finished();

  // Virtual methods from WorkerTaskBase
  virtual std::string str();
  virtual void fill(samson::gpb::CollectionRecord *record, const Visualization& visualization);

private:

  // Specific function to execute map, reduce, parser operations
  void generateKeyValues_parser(samson::ProcessWriter *writer);
  void generateKeyValues_map(samson::ProcessWriter *writer);
  void generateKeyValues_reduce(samson::ProcessWriter *writer);

  // Information about the operation to run
  gpb::StreamOperation *stream_operation_;

  // Operation to be used here ( form ModulesManager )
  Operation *operation_;

  // Range to apply this operation
  KVRange range_;

  // Falg to indicate that this operation is finished
  bool finished_;

  // Error container if something happen suring execution
  au::ErrorManager error_;

  // Pointer to samson worker to create new blocks
  SamsonWorker *samson_worker_;
};


class OperationTraces {
  std::string name;
  au::Cronometer cronometer;
  size_t input_size;

public:

  OperationTraces(std::string _name, size_t _input_size) {
    name = _name;
    input_size = _input_size;
    LM_T(LmtIsolatedOperations, ("%s starts with input %s", name.c_str(), au::str(input_size, "B").c_str()));
  }

  ~OperationTraces() {
    size_t time = cronometer.seconds();
    double rate = 0;

    if (time > 0) {
      rate = input_size / time;
    }

    LM_T(LmtIsolatedOperations, ("%s ( input size %s ) finish atfer %s. Aprox rate %s"
                                 , name.c_str()
                                 , au::str(input_size, "B").c_str()
                                 , au::S(cronometer).str().c_str()
                                 , au::str(rate, "B/s").c_str()
                                 ));
  }

  void trace_block(size_t block_size) {
    LM_T(LmtIsolatedOperations,
         ("%s running a block of %s. Time since start %s", name.c_str(),
          au::str(block_size, "B").c_str(), au::S(cronometer).str().c_str()));
  }
};
}
}

#endif  // ifndef _H_SAMSON_QUEUE_TASK
