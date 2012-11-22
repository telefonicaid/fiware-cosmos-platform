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
#include <string>

#include "au/string/S.h"

#include "engine/Engine.h"                  // engine::Engine
#include "engine/ProcessItem.h"             // engine::ProcessItem
#include "engine/ProcessManager.h"          // engine::ProcessManager

#include "samson/common/KVHeader.h"
#include "samson/network/NetworkInterface.h"
#include "samson/network/Packet.h"
#include "samson/stream/ProcessIsolated.h"  // Own interface
#include "samson/stream/ProcessWriter.h"
#include "samson/stream/SharedMemoryManager.h"
#include "samson/worker/SamsonWorker.h"

namespace samson {
ProcessIsolated::ProcessIsolated(SamsonWorker *samson_worker
                                 , size_t worker_task_id
                                 , const std::string& operation
                                 , const std::string& concept
                                 , ProcessBaseType _type) :
  ProcessItemIsolated(samson_worker, worker_task_id, operation, concept) {
  num_outputs = 0;   // Outputs are defined calling "addOutput" with the rigth output format
  type = _type;   // Keep the type of operation ( data is generated differently )

  // By default we have no asignation of shared memory
  // This is obtained when executing
  shm_id = -1;
  item = NULL;

  // Output ranges based on cluster information
  output_ranges_ = samson_worker->worker_controller()->GetKVRanges();

  writer = NULL;
  txtWriter = NULL;
}

ProcessIsolated::~ProcessIsolated() {
  if (writer) {
    delete writer;
  }
  if (txtWriter) {
    delete txtWriter;
  }
}

// Get the writers to emit key-values
ProcessWriter *ProcessIsolated::getWriter() {
  if (type != key_value) {
    LM_X(1, ("Internal error: Not possible to get a writer if type!=key_value"));
  }
  if (!writer) {
    writer = new ProcessWriter(this);
  }
  return writer;
}

ProcessTXTWriter *ProcessIsolated::getTXTWriter() {
  if (type != txt) {
    LM_X(1, ("Internal error: Not possible to get a txtWriter if type!=txt"));
  }
  if (!txtWriter) {
    txtWriter = new ProcessTXTWriter(this);
  }
  return txtWriter;
}

void ProcessIsolated::runCode(int c) {
  // LM_M(("Isolated process Running code %d",c));

  switch (c) {
    case WORKER_TASK_ITEM_CODE_FLUSH_BUFFER:
      flushBuffer(false);   // Flush the generated buffer with new key-values
      return;

      break;
    case WORKER_TASK_ITEM_CODE_FLUSH_BUFFER_FINISH:
      flushBuffer(true);   // Flush the generated buffer with new key-values
      return;

      break;
    default:
      error_.set("System error: Unknown code in the isolated process communication");
      break;
  }

  // LM_M(("Finish Isolated process Running code %d",c));
}

void ProcessIsolated::flushBuffer(bool finish) {
  au::Cronometer cronometer;

  LM_T(LmtIsolatedOutputs,
       ("Flush buffer starts ( shared memory id %d ) for operation %s ", shm_id, process_item_description().c_str()));

  switch (type) {
    case key_value:
      flushKVBuffer(finish);
      break;
    case txt:
      flushTXTBuffer(finish);
      break;
  }

  LM_T(LmtIsolatedOutputs, ("Flush buffer finished ( shared memory id %d ) for operation %s atfer %s "
                            , shm_id, process_item_description().c_str(), au::S(cronometer).str().c_str()));
}

void ProcessIsolated::flushKVBuffer(bool finish) {
#pragma mark ---

  // General output buffer
  char *buffer = item->data;
  size_t size = item->size;

  // Make sure everything is correct
  if (!buffer) {
    LM_X(1, ("Internal error: Missing buffer in ProcessBase"));
  }
  if (size == 0) {
    LM_X(1, ("Internal error: Wrong size for ProcessBase"));   // Outputs structures placed at the begining of the buffer
  }
  OutputChannel *channel = reinterpret_cast<OutputChannel *>(buffer);

  // NodeBuffers ( inodes in the shared memory buffer )
  NodeBuffer *node = reinterpret_cast<NodeBuffer *>(buffer + sizeof(OutputChannel) * num_outputs );

#pragma mark ---

  // size_t task_id = task->workerTask.task_id();

  for (int o = 0; o < num_outputs; o++) {
    OutputChannel *_channel = &channel[o];

    if (_channel->info.size == 0) {
      continue;
    }

    // For each output range, create an output buffer
    for (size_t r = 0; r < output_ranges_.size(); r++) {
      // Selected range
      KVRange range = output_ranges_[r];

      // Compute size for the buffer
      KVInfo range_info;
      for (int hg = range.hg_begin_; hg < range.hg_end_; hg++) {
        range_info.append(_channel->hg[hg].info);
      }

      if (range_info.size == 0) {
        continue;
      }

      engine::BufferPointer buffer = engine::Buffer::Create("Output of an isolated process [" + concept() + "]"
                                                            , sizeof(KVHeader) + range_info.size);

      if (buffer == NULL) {
        LM_X(1, ("Internal error: Not possible to create a buffer"));
      }
      // Pointer to the header
      KVHeader *header = reinterpret_cast<KVHeader *> (buffer->data());

      // Initial offset for the buffer to write data
      buffer->SkipWrite(sizeof(KVHeader));

      // KVFormat format = KVFormat( output_queue.format().keyformat() , output_queue.format().valueformat() );
      if (outputFormats.size() > (size_t)o) {
        header->Init(outputFormats[o], range_info);
      } else {
        error_.set(au::str("No output format for output %d", o));
        return;
      }

      for (int i = range.hg_begin_; i <  range.hg_end_; i++) {
        // Current hash-group output
        HashGroupOutput *_hgOutput = &_channel->hg[i];

        // Write data following nodes
        uint32 node_id = _hgOutput->first_node;
        while (node_id != KV_NODE_UNASIGNED) {
          if (node_id > _hgOutput->last_node) {
            LM_W(("Warning, we have passed through the end of hashgroup(%u,%u), node_id:%u",
                  _hgOutput->first_node, _hgOutput->last_node, node_id));
          }
          bool ans = buffer->Write(reinterpret_cast<char *>(node[node_id].data), node[node_id].size);
          if (!ans) {
            LM_X(1, ("Error writing key-values into a temporal Buffer ( size %lu ) ", node[node_id].size));   // Go to the next node
          }
          node_id = node[node_id].next;
        }
      }

      if (buffer->size() != buffer->max_size()) {
        LM_X(1, ("Internal error"));   // Set the hash-group limits of the header
      }

      int min_hg = range.hg_begin_;
      int max_hg = range.hg_end_;

      while (_channel->hg[min_hg].info.size == 0) {
        min_hg++;
      }
      while (_channel->hg[max_hg - 1].info.size == 0) {
        max_hg--;
      }

      // Compute the range of valid data
      header->range = KVRange(min_hg, max_hg);

      // Process the output buffer
      processOutputBuffer(buffer, o);
    }
  }
}

void ProcessIsolated::flushTXTBuffer(bool finish) {
#pragma mark ---

  // Size if the firt thing in the buffer
  size_t size = *(reinterpret_cast<size_t *>(item->data));

  // Init the data buffer used here
  char *data = item->data + sizeof(size_t);

#pragma mark ---

  if (size > 0) {
    engine::BufferPointer buffer = engine::Buffer::Create("Output of an isolated process [" + concept() + "]",
                                                          sizeof(KVHeader) + size);

    if (buffer == NULL) {
      LM_X(1, ("Internal error"));
    }
    KVHeader *header = reinterpret_cast<KVHeader *>(buffer->data());
    header->InitForTxt(size);

    // copy the entire buffer to here
    memcpy(buffer->data() + sizeof(KVHeader), data, size);
    buffer->set_size(sizeof(KVHeader) + size);

    processOutputBuffer(buffer, 0);   // Output is always channel "0"
  }
}

void ProcessIsolated::initProcessItemIsolated() {
  setActivity("process");

  initProcessIsolated();   // Init function in the foreground-process

  if (!CheckCompleteKVRanges(output_ranges_)) {
    error_.set("Output ranges are not complete");
  }

  if (error_.IsActivated()) {
    return;
  }

  // Init the shared memory segment
  shm_id = engine::SharedMemoryManager::shared()->RetainSharedMemoryArea();
  if (shm_id != -1) {
    item = engine::SharedMemoryManager::shared()->getSharedMemoryPlatform(shm_id);
  } else {
    error_.set(au::str("Error getting shared memory for %s", process_item_description().c_str()));
  }
}

void ProcessIsolated::finishProcessItemIsolated() {
  if (shm_id != -1) {
    engine::SharedMemoryManager::shared()->ReleaseSharedMemoryArea(shm_id);
    item = NULL;
    shm_id = -1;
  }

  setActivity("finishing");
}

void ProcessIsolated::runIsolated() {
  switch (type) {
    case key_value:
      generateKeyValues(getWriter());
      getWriter()->flushBuffer(true);
      break;

    case txt:
      // Generate TXT content using the entire buffer
      generateTXT(getTXTWriter());
      getTXTWriter()->flushBuffer(true);
      break;
  }
}

void ProcessIsolated::addOutput(KVFormat format) {
  num_outputs++;
  outputFormats.push_back(format);
}

void ProcessIsolated::addOutputsForOperation(Operation *op) {
  for (int i = 0; i < op->getNumOutputs(); i++) {
    addOutput(op->getOutputFormat(i));
  }

  // Add an additional output for log
  addOutput(KVFormat("system.Value", "system.Value"));
}

void ProcessIsolated::setProcessBaseMode(ProcessBaseType _type) {
  type = _type;
}
}
