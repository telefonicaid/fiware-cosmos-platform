/* ****************************************************************************
*
* FILE            MemoryManager
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* MemoryManager is the controller of the "big" blocks of memory used by the app
* Buffers can be allocated directly using newBuffer.
* A Notification ( using engine mechanism ) is sent when memory is available
*
* ****************************************************************************/


#ifndef _H_MEMORY_MANAGER
#define _H_MEMORY_MANAGER

#include <iostream>             // std::cout
#include <list>                 // std::list
#include <map>                  // std::map
#include <set>                  // std::set
#include <stdio.h>              // perror
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "au/containers/list.h"  // au::list
#include "au/containers/map.h"  // au::map
#include "au/mutex/Token.h"     // au::Token
#include "au/string/string.h"          // au::Format
#include "engine/Buffer.h"      // samson::Buffer
#include "engine/Engine.h"      // samson::Buffer


#include "au/tables/Table.h"

#define notification_memory_request_response "notification_memory_request_response"

namespace engine {
/**
 *
 * Memory manager is a singleton implementation to manager the memory used by any component of SAMSON
 * A unifierd view of the memory is necessary to block some data-generator elements when we are close to the maximum memory
 * It is also responsible to manage shared-memory areas to share between differnt processes.
 *
 */

class MemoryManager {
public:

  ~MemoryManager();

  // Register and Unregister buffers to keep track of memory allocated
  void Add(Buffer *buffer);
  void Remove(Buffer *buffer);

  // Get table with all buffer
  au::tables::Table getTableOfBuffers();

  // Accessors
  size_t memory();
  int num_buffers();
  size_t used_memory();
  double memory_usage();

private:

  MemoryManager(size_t memory);  // Constructor is private ( see Engine::InitEngine() )

  au::Token token_;                            // Mutex protection

  size_t used_memory_;                         // Total memory used
  size_t memory_;                              // Total available memory

  std::set<Buffer *> buffers_;                 // List of active buffers for better monitoring

  friend class Buffer;                         // Buffer class can call to destoryBuffer
  friend class BufferContainer;                // Buffer container is the only one that can create Buffers directly

  friend class Engine;
};
}

#endif  // ifndef _H_MEMORY_MANAGER
