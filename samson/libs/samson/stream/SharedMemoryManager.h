/* ****************************************************************************
 *
 * FILE                     SharedMemoryManager.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

#ifndef _H_SHARED_MEMORY_MANAGER
#define _H_SHARED_MEMORY_MANAGER

#include <iostream>                          // std::cout
#include <list>                              // std::list
#include <map>                               // std::map
#include <set>                               // std::set
#include <stdio.h>                           // perror
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include "samson/common/samsonDirectories.h"  // SAMSON_SETUP_FILE

#include "engine/Buffer.h"                   // samson::Buffer
#include "engine/Engine.h"                   // samson::Engine

#include "au/containers/map.h"               // au::map
#include "au/mutex/Token.h"                  // au::Token
#include "au/string/StringUtilities.h"                       // au::Format

namespace engine {
class SharedMemoryItem;

class SharedMemoryManager {
  
  au::Token token;                           // Token to protect this instance

  // Shared memory management
  size_t shared_memory_size_per_buffer;      // Shared memory used in every buffer
  int shared_memory_num_buffers;             // Number of shared memory buffers to create
  bool *shared_memory_used_buffers;          // Bool vector showing if a particular shared memory buffer is used
  int *shm_ids;                              // Vector containing all the shared memory identifiers

  std::vector<SharedMemoryItem *> shared_memory_items;  // Vector containing all the SharedMemoryItem's

  std::string sharedMemoryIdsFileName;

  SharedMemoryManager(int _shared_memory_num_buffers, size_t _shared_memory_size_per_buffer);

public:

  ~SharedMemoryManager();

  // Init
  static void init(int _shared_memory_num_buffers, size_t _shared_memory_size_per_buffer);
  static SharedMemoryManager *shared();
  static void destroy();

  /**
   * Function to retain and release a free shared-memory area
   */

  int RetainSharedMemoryArea();
  void ReleaseSharedMemoryArea(int id);

  /**
   * Functions to get a Shared Memory Item
   */

  SharedMemoryItem *getSharedMemoryPlatform(int i);
  SharedMemoryItem *getSharedMemoryChild(int i);

  // Function to get some information
  static std::string str();
  std::string _str();

private:

  void createSharedMemorySegments();
  static void removeSharedMemorySegments(int *ids, int length);
  void remove_previous_shared_areas();
  void write_current_shared_areas_to_file();

  // Function to create a SharedMemory item
  SharedMemoryItem *createSharedMemory(int i);
};
};

#endif  // ifndef _H_SHARED_MEMORY_MANAGER
