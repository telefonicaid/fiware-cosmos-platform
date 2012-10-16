#ifndef _H_PROCESS_WRITER
#define _H_PROCESS_WRITER

#include <iostream>                     // std::cout

#include "au/ErrorManager.h"            // au::ErrorManager
#include "au/string.h"                  // au::Format

#include "engine/MemoryManager.h"

#include "samson/common/coding.h"       // High level definition of KV_*
#include "samson/module/DataInstance.h"  // samson::DataInstance
#include "samson/module/KVWriter.h"     // samson::KVWriter
#include "samson/module/Operation.h"    // samson::Operation

#include "SharedMemoryItem.h"

#define KV_NODE_SIZE      255
#define KV_NODE_UNASIGNED 4294967295u   // 2^32-1

namespace samson {
class ProcessIsolated;

/**
 * Class to emit key-values ( for maps , reducers, generators, parsers, etc )
 */

class ProcessWriter : public KVWriter {
  ProcessIsolated *processIsolated;     // Pointer to the processIsolated to emit codes through the pipe

  engine::SharedMemoryItem *item;       // Shared memory item used at this side ( fork in the middle )

  char *buffer;                         // General output buffer
  size_t size;                          // General output buffer size

  int num_outputs;                      // Number of global outputs ( channels of output )

  /*
   * Output key-values for a particular output channel will be dividided in "num_hg_divisions" blocks
   * dividing the full range of hash-groups in groups of hash-groups
   */

  // Minibuffer to serialize
  char *miniBuffer;
  size_t miniBufferSize;

  // Collection of output data instances and hash-values to check we are using the rigth DataInstance
  DataInstance **outputKeyDataInstance;
  DataInstance **outputValueDataInstance;
  KeyValueHash *keyValueHash;

  // Structure to divide shared memory buffer to emit key-values for each output
  OutputChannel *channel;

  // Node buffers ( inodes in the shared-memory buffer )
  NodeBuffer *node;                     // Pointer to the entire node set
  uint32 num_nodes;                     // Total number of nodes ( to fit inside the shared memory block)

  uint32 new_node;                      // Identifier of the next free block

private:

  friend class ProcessIsolated;
  ProcessWriter(ProcessIsolated *_processIsolated);

public:

  ~ProcessWriter();

  // KVWriter interface
  virtual void emit(int output, DataInstance *key, DataInstance *value);

  // Function used to flush the content of the buffer ( it sends a code to the "parent" process )
  void flushBuffer(bool finish);

  // Emit data for a particular output channel
  // It is not public since it is used in WorkerTask to bypass key-values
  // not processed in update_only reduce operations
  void internal_emit(int output, int hg, char *data, size_t size);

private:

  // Clear the current buffer to submit new key-values
  void clear();
};

class ProcessTXTWriter : public TXTWriter {
public:

  ~ProcessTXTWriter();

  ProcessIsolated *workerTaskItem;                      // Pointer to the workTaskItem to emit codes through the pipe

  char *data;
  size_t *size;
  size_t max_size;

  engine::SharedMemoryItem *item;

private:

  friend class ProcessIsolated;
  ProcessTXTWriter(ProcessIsolated *_workerTaskItem);

public:

  void emit(const char *_data, size_t _size);

  void flushBuffer(bool finish);
};
}

#endif  // ifndef _H_PROCESS_WRITER
