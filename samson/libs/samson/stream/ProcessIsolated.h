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
#ifndef _H_SAMSON_PROCESS_ISOLATED
#define _H_SAMSON_PROCESS_ISOLATED

#include <cstring>                               // size_t
#include <sstream>                               // std::ostringstream
#include <string>                                // std::string
#include <vector>

#include "au/containers/Queue.h"
#include "au/log/LogCentral.h"
#include "au/log/LogCommon.h"
#include "au/mutex/Token.h"

#include "engine/Buffer.h"

#include "samson/common/coding.h"
// samson::network::...
#include "samson/module/Environment.h"           // samson::Enviroment
#include "samson/stream/ProcessItemIsolated.h"  // samson:ProcessItemIsolated

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
    key_value,     // Emit key-values to multiple outputs / multiple workers
    txt
    // Emit txt content using the entire buffer
  } ProcessBaseType;

  ProcessIsolated(SamsonWorker *samson_worker
                  , size_t worker_task_id
                  , const std::string& operation
                  , const std::string& concept
                  , ProcessBaseType _type
                  );
  virtual ~ProcessIsolated();

  ProcessWriter *getWriter();     // Get writer to emit key-values
  ProcessTXTWriter *getTXTWriter();     // Get writer to emit txt content

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
  // Not pure virtual since only one of both is implemented
  // --------------------------------------------------------------------------
  virtual void generateKeyValues(ProcessWriter *writer) {
    if (writer == NULL) {
      return;
    }
  }

  virtual void generateTXT(TXTWriter *writer) {
    if (writer == NULL) {
      return;
    }
  }

  // --------------------------------------------------------------------------
  // Methods implemented by subclases to do something at init
  // --------------------------------------------------------------------------

  virtual void initProcessIsolated() = 0;

  // ---------------------------------------------------------------
  // SETUP ProcessIsolated
  // ---------------------------------------------------------------

  // Generic function to add outputs to this ProcesIsolated
  void addOutput(KVFormat format);
  void addOutputsForOperation(Operation *op);

  // Chage the type of usage
  void setProcessBaseMode(ProcessBaseType _type);

  // Get next output buffer to be process
  engine::BufferPointer GetNextOutputBuffer() {
    au::TokenTaker tt(&token_);

    return output_buffers_.Pop();
  }

  Environment *get_operation_environment() {
    return &operation_environment;
  }

  std::vector<KVFormat>& get_outputFormats() {
    return outputFormats;
  }

  int get_shm_id() {
    return shm_id;
  }

  int get_num_outputs() {
    return num_outputs;
  }

private:

  /**
   * \brief Process generated buffers ( in a separate thread )
   *
   * Accumulate output buffers in a private output list
   *
   */
  void processOutputBuffer(engine::BufferPointer buffer, int output) {
    au::TokenTaker tt(&token_);

    buffer->environment().Set("output", output);
    output_buffers_.Push(buffer);
  }

  ProcessBaseType type;     // Type of process( to generate key-values or generate txt content )

  std::vector<KVRange> output_ranges_;  // Ranges to emit separated packets

  int shm_id;     // Shared memory area used in this operation
  samson::SharedMemoryItem *item;     // Share memory item

  ProcessWriter *writer;     // Object used to emit key-values if type=key_value
  ProcessTXTWriter *txtWriter;     // Object used to emit txt content if type=txt

  int num_outputs;     // Number of outputs

  std::vector<KVFormat> outputFormats;     // Auxiliar information to give correct format to output buffers
  Environment operation_environment;     // Environment for this process

  // List of generated buffers at the output
  au::Token token_;
  au::Queue<engine::Buffer> output_buffers_;
};
}

#endif  // ifndef _H_SAMSON_PROCESS_ISOLATED
