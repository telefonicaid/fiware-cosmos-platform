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
#include "au/string/StringUtilities.h"       // au::Format

namespace samson {
class SharedMemoryItem;

class SharedMemoryManager {
public:

  ~SharedMemoryManager();

  /**
   * \brief Init shared memory segments to exchange data between main and background processess
   */
  static void Init(int shared_memory_num_buffers, size_t shared_memory_size_per_buffer);

  /**
   * \brief Destroy shared memory segments created with init
   */
  static void Destroy();

  static SharedMemoryManager *Shared();

  /**
   * *\brief  Retain a free shared-memory area for an operation
   */

  int RetainSharedMemoryArea();

  /**
   * *\brief  release a (previously retained) shared-memory area
   */

  void ReleaseSharedMemoryArea(int id);

  /**
   * \brief Get a shared memory segment ( in main platform process )
   */

  SharedMemoryItem *GetSharedMemoryPlatform(int i) const;

  /**
   * \brief Get a shared memory segment ( in main any background process )
   */

  SharedMemoryItem *GetSharedMemoryChild(int i);

  /**
   * \brief Debug string
   */
  static std::string str();

  std::string _str();

private:

  // Private constructor for Singleton implementation
  SharedMemoryManager(int _shared_memory_num_buffers, size_t _shared_memory_size_per_buffer);

  void CreateSharedMemorySegments();
  SharedMemoryItem *CreateSharedMemory(int i);  // Function to create a SharedMemory item
  void RemovePreviousSharedAreas();
  void WriteCurrentSharedAreasToFile();

  static void RemoveSharedMemorySegments(int *ids, int length);

  au::Token token_;                           // Token to protect this instance

  // Shared memory management
  size_t shared_memory_size_per_buffer_;      // Shared memory used in every buffer
  int shared_memory_num_buffers_;             // Number of shared memory buffers to create
  bool *shared_memory_used_buffers_;          // Bool vector showing if a particular shared memory buffer is used
  int *shm_ids_;                              // Vector containing all the shared memory identifiers

  au::vector<SharedMemoryItem> shared_memory_items_;  // Vector containing all the SharedMemoryItem's

  std::string sharedMemoryIdsFileName_;
};
};

#endif  // ifndef _H_SHARED_MEMORY_MANAGER
