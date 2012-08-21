#ifndef _H_CODING
#define _H_CODING

/* ****************************************************************************
 *
 * FILE                 coding.h -
 *
 */
#include <string.h>                     // std::string
#include <sys/time.h>                   // struct timeval

#include "logMsg/logMsg.h"              // LM_X

#include "au/string.h"                  // au::Format
#include "au/xml.h"                     // au::xml...

#include "engine/SimpleBuffer.h"        // engine::SimpleBuffer

#include "samson/common/samson.pb.h"    // network:...

#include "samson/module/KVFormat.h"     // samson::KVFormat
#include <samson/module/Data.h>

#include "KVInfo.h"



#define KV_NODE_SIZE      255           // Size for each node in the shared memory segment
#define KV_NODE_UNASIGNED 4294967295u   // 2^32-1

namespace samson {
bool operator!=(const KVFormat & left, const KVFormat & right);
bool operator==(const KVFormat & left, const KVFormat & right);


struct HashGroupOutput {
  KVInfo info;                          // Basic info of this hg
  uint32 first_node;                    // First block with output for this hash gorup
  uint32 last_node;                     // Last block with output for this hash gorup

  void init() {
    info.clear();
    first_node = KV_NODE_UNASIGNED;
    last_node = KV_NODE_UNASIGNED;
  }
};

/**
 * A channel is the unit of data for a particular server and output
 */

struct OutputChannel {
  KVInfo info;
  HashGroupOutput hg[ KVFILE_NUM_HASHGROUPS ];

  void init() {
    info.clear();
    for (int i = 0; i < KVFILE_NUM_HASHGROUPS; i++) {
      hg[i].init();
    }
  }
};

/*
 * Basic unit of storage for output key-values
 * It is used by a particular output channel in a linked-chain way
 */

struct NodeBuffer {
  uint8 size;                                   // Size used inside this minibuffer
  char data[KV_NODE_SIZE];                      // Real buffer with data
  uint32 next;                                  // Identifier of the next buffer

  void init() {
    size = 0;
    next = KV_NODE_UNASIGNED;
  }

  inline size_t write(char *_data, size_t _size) {
    size_t available = KV_NODE_SIZE - size;

    if (_size > available) {
      memcpy(data + size, _data, available);
      size = KV_NODE_SIZE;
      return available;
    } else {
      memcpy(data + size, _data, _size);
      size += _size;
      return _size;
    }
  }

  inline bool isFull() {
    return ( size == KV_NODE_SIZE );
  }

  inline void setNext(uint32 _next) {
    next = _next;
  }

  inline size_t availableSpace() {
    return KV_NODE_SIZE - size;
  }
};

/**
 * Structure to save the hash-code of the datainstance
 */

struct KeyValueHash {
  size_t key_hash;
  size_t value_hash;

  KeyValueHash() {
    key_hash = 0;
    value_hash = 0;
  }
};
}

#endif  // ifndef _H_CODING
