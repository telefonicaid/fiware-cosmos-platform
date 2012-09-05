#ifndef _H_SAMSON_PROCESS_ISOLATED
#define _H_SAMSON_PROCESS_ISOLATED


#include <cstring>                               // size_t
#include <sstream>                               // std::ostringstream
#include <string>                                // std::string

#include "engine/Buffer.h"

#include "samson/common/coding.h"
#include "samson/common/samson.pb.h"             // samson::network::...

#include "samson/isolated/ProcessItemIsolated.h"  // samson:ProcessItemIsolated
#include "samson/module/Environment.h"           // samson::Enviroment

#define WORKER_TASK_ITEM_CODE_FLUSH_BUFFER        1
#define WORKER_TASK_ITEM_CODE_FLUSH_BUFFER_FINISH 2

namespace samson {
class ProcessWriter;
class ProcessTXTWriter;
class Operation;
class KVWriter;
class TXTWriter;
class NetworkInterface;

/**
 *
 * ProcessIsolated is an isolated process that share
 * one or more shared-memory buffers between process and platform
 *
 */

class ProcessIsolated : public ProcessItemIsolated {
public:

  typedef enum {
    key_value,             // Emit key-values to multiple outputs / multiple workers
    txt                    // Emit txt content using the entire buffer
  } ProcessBaseType;

  ProcessBaseType type;

public:

  int shm_id;              // Shared memory area used in this operation
  engine::SharedMemoryItem *item;   // Share memory item

  ProcessWriter *writer;   // Object used to emit key-values if type=key_value
  ProcessTXTWriter *txtWriter;      // Object used to emit txt content if type=txt

public:

  int num_outputs;         // Number of outputs
  static int num_hg_divisions;    // Number of hg divisions

  // Auxiliar information to give correct format to output buffers
  std::vector<KVFormat> outputFormats;

public:

  Environment operation_environment;                  // Environment for this process

public:

  ProcessIsolated(std::string description,  ProcessBaseType _type);
  virtual ~ProcessIsolated();

  // Get the writers to emit key-values
  ProcessWriter *getWriter();

  // Get writer to emit txt content
  ProcessTXTWriter *getTXTWriter();

  // Flush the buffer ( front process ) in key-value and txt mode
  void flushBuffer(bool finish);
  void flushKVBuffer(bool finish);
  void flushTXTBuffer(bool finish);

  // Function executed at this process side when a code is sent from the background process
  void runCode(int c);

  // Virtual methods of ProcessItemIsolated
  virtual void initProcessItemIsolated();
  virtual void runIsolated();
  virtual void finishProcessItemIsolated();

  // --------------------------------------------------------------------------
  // Methods implemented by subclases to generate content ( in the background process )
  // Not pure virtual
  // --------------------------------------------------------------------------
  virtual void generateKeyValues(ProcessWriter *writer) {
    if (writer == NULL) {
      return;
    }
  };
  virtual void generateTXT(TXTWriter *writer) {
    if (writer == NULL) {
      return;
    }
  };

  // --------------------------------------------------------------------------
  // Methods implemented by subclases to do something with the output buffers
  // at front-process
  // --------------------------------------------------------------------------

  virtual void initProcessIsolated() = 0;
  virtual void processOutputBuffer(engine::BufferPointer buffer, int output, int hg_division, bool finish) = 0;
  virtual void processOutputTXTBuffer(engine::BufferPointer buffer, bool finish) = 0;

  // ---------------------------------------------------------------
  // SETUP ProcessIsolated
  // ---------------------------------------------------------------

  // Generic function to add outputs to this ProcesIsolated
  void addOutput(KVFormat format);
  void addOutputsForOperation(Operation *op);

  // Chage the type of usage
  void setProcessBaseMode(ProcessBaseType _type);

  // Send traces using distribution information
  // void sendAlert( samson::gpb::Alert& alert );
};
}


#endif  // ifndef _H_SAMSON_PROCESS_ISOLATED
