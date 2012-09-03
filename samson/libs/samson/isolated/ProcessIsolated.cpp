

#include "au/S.h"

#include "engine/Engine.h"                  // engine::Engine
#include "engine/ProcessItem.h"             // engine::ProcessItem
#include "engine/ProcessManager.h"          // engine::ProcessManager

#include "ProcessIsolated.h"                // Own interface
#include "ProcessWriter.h"
#include "SharedMemoryManager.h"

#include "samson/common/KVHeader.h"

#include "samson/network/NetworkInterface.h"
#include "samson/network/Packet.h"

namespace samson {
// Number of blocks generated at the output of each operation
// 5 is an arbitrary name ( should be based on the size of the cluster and redundancy )
int ProcessIsolated::num_hg_divisions = 5;

ProcessIsolated::ProcessIsolated(std::string description, ProcessBaseType _type) : ProcessItemIsolated(description) {
  num_outputs = 0;          // Outputs are defined calling "addOutput" with the rigth output format

  type = _type;

  // By default we have no asignation of shared memory
  // This is obtained when executing
  shm_id = -1;
  item = NULL;

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
      flushBuffer(false);       // Flush the generated buffer with new key-values
      return;

      break;
    case WORKER_TASK_ITEM_CODE_FLUSH_BUFFER_FINISH:
      flushBuffer(true);        // Flush the generated buffer with new key-values
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
       ("Flush buffer starts ( shared memory id %d ) for operation %s ", shm_id,  process_item_description().c_str()));

  switch (type) {
    case key_value:
      flushKVBuffer(finish);
      break;
    case txt:
      flushTXTBuffer(finish);
      break;
  }

  LM_T(LmtIsolatedOutputs, ("Flush buffer finished ( shared memory id %d ) for operation %s atfer %s "
                            , shm_id,   process_item_description().c_str(), au::S(cronometer).str().c_str()));
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
    LM_X(1, ("Internal error: Wrong size for ProcessBase"));  // Outputs structures placed at the begining of the buffer
  }
  OutputChannel *channel = (OutputChannel *)buffer;

  // NodeBuffers ( inodes in the shared memory buffer )
  NodeBuffer *node = (NodeBuffer *)( buffer + sizeof(OutputChannel) * num_outputs * num_hg_divisions );

#pragma mark ---

  // size_t task_id = task->workerTask.task_id();

  for (int o = 0; o < num_outputs; o++) {
    for (size_t s = 0; s < (size_t)num_hg_divisions; s++) {
      OutputChannel *_channel = &channel[ o * num_hg_divisions + s ];

      if (_channel->info.size > 0) {
        engine::BufferPointer buffer = engine::Buffer::Create("Output of " + processItemIsolated_description
                                                              , "ProcessIsolated"
                                                              , sizeof(KVHeader) + _channel->info.size);

        if (buffer == NULL) {
          LM_X(1, ("Internal error: Missing buffer in ProcessBase"));  // Pointer to the header
        }
        KVHeader *header = (KVHeader *)buffer->data();

        // Initial offset for the buffer to write data
        buffer->SkipWrite(sizeof(KVHeader));

        // KVFormat format = KVFormat( output_queue.format().keyformat() , output_queue.format().valueformat() );
        if (outputFormats.size() > (size_t)o) {
          header->Init(outputFormats[o], _channel->info);
        } else {
          header->Init(KVFormat("no-used", "no-used"), _channel->info);  // This buffer is not not sended with the buffer
        }
        KVInfo *info = (KVInfo *)malloc(sizeof(KVInfo) * KVFILE_NUM_HASHGROUPS);

        for (int i = 0; i < KVFILE_NUM_HASHGROUPS; i++) {
          // Current hash-group output
          HashGroupOutput *_hgOutput     = &_channel->hg[i];

          // Set gloal info
          info[i] = _hgOutput->info;

          // Write data followign nodes
          uint32 node_id = _hgOutput->first_node;
          while (node_id != KV_NODE_UNASIGNED) {
            bool ans = buffer->Write((char *)node[node_id].data, node[node_id].size);
            if (!ans) {
              LM_X(1, ("Error writing key-values into a temporal Buffer ( size %lu ) ", node[node_id].size ));  // Go to the next node
            }
            node_id = node[node_id].next;
          }
        }

        if (buffer->size() != buffer->max_size()) {
          LM_X(1, ("Internal error"));                                                                          // Set the hash-group limits of the header
        }
        header->range.setFrom(info);

        // Free buffer of KVInfo ( not not sended with the buffer )
        free(info);

        // Process the output buffer
        processOutputBuffer(buffer, o, s, finish);
      }
    }
  }
}

void ProcessIsolated::flushTXTBuffer(bool finish) {
#pragma mark ---

  // Size if the firt thing in the buffer
  size_t size = *((size_t *)item->data );

  // Init the data buffer used here
  char *data = item->data + sizeof(size_t);

#pragma mark ---

  if (size > 0) {
    engine::BufferPointer buffer = engine::Buffer::Create("Output of " + processItemIsolated_description
                                                          , "ProcessIsolated"
                                                          , sizeof(KVHeader) + size);

    if (buffer == NULL) {
      LM_X(1, ("Internal error"));
    }
    KVHeader *header = (KVHeader *)buffer->data();
    header->InitForTxt(size);

    // copy the entire buffer to here
    memcpy(buffer->data() + sizeof( KVHeader ), data, size);
    buffer->set_size(sizeof(KVHeader) + size);

    processOutputTXTBuffer(buffer, finish);
  }
}

void ProcessIsolated::initProcessItemIsolated() {
  // Init function in the foreground-process
  initProcessIsolated();

  if (error_.IsActivated()) {
    return;
  }

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
