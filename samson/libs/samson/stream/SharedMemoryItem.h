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
 * FILE                     SharedMemoryItem.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

#ifndef _H_SHARED_MEMORY_ITEM
#define _H_SHARED_MEMORY_ITEM

#include <iostream>                          // std::cout
#include <list>                              // std::list
#include <map>                               // std::map
#include <set>                               // std::set
#include <stdio.h>                           // perror
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>


#include "au/containers/map.h"               // au::map
#include "au/mutex/Token.h"                  // au::Token
#include "au/string/StringUtilities.h"       // au::Format

#include "engine/Buffer.h"                   // samson::Buffer

// network::..
#include "samson/common/samsonDirectories.h"  // SAMSON_SETUP_FILE


namespace samson {
/*
 * SharedMemoryItem is a class that contains information about a region of memory shared between different processes
 * Memory manager singleton provides pointers to these objects
 */

class SharedMemoryItem {
public:

  // Constructor and destructor
  SharedMemoryItem(int id, int shmid, size_t size);
  ~SharedMemoryItem();

  // --------------------------------------------------------------------------------
  // Interfaces to get SimpleBuffer elements in order to read or write to them
  // --------------------------------------------------------------------------------

  engine::SimpleBuffer getSimpleBuffer() const {
    return engine::SimpleBuffer(data_, size_);
  }

  engine::SimpleBuffer getSimpleBufferAtOffset(size_t offset) const {
    return engine::SimpleBuffer(data_ + offset, size_ - offset);
  }

  engine::SimpleBuffer getSimpleBufferAtOffsetWithMaxSize(size_t offset, size_t size) const {
    if (size > (size_ - offset)) {
      LM_X(1, ("Error cheking size of a simple Buffer"));
    }
    return engine::SimpleBuffer(data_ + offset, size_ - offset);
  }

  int id() const {
    return id_;
  }

  int shmid() const {
    return shmid_;
  }

  size_t size() const {
    return size_;
  }

  char *data() const {
    return data_;
  }

private:

  int id_;                                               /* Identifier of the shared memory area 0 .. N-1 */
  int shmid_;                                            /* return value from shmget() */
  char *data_;                                           /* Shared memory data */
  size_t size_;                                          /* Information about the size of this shared memory item */
};
};

#endif  // ifndef _H_SHARED_MEMORY_ITEM
