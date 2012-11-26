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
 * FILE                     SharedMemoryItem.cpp
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

#include "SharedMemoryItem.h"  // Own interface

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Trace Levels

#include "engine/Buffer.h"             // samson::Buffer
#include "engine/Engine.h"             // samson::Engine
#include "engine/MemoryManager.h"      // Own interface
#include "samson/common/SamsonSetup.h"  // ss:SamsonSetup
#include <sstream>                     // std::stringstream



namespace samson {
SharedMemoryItem::SharedMemoryItem(int id, int shmid, size_t size) : id_(id), shmid_(shmid), size_(size) {
  // Attach to local-space memory
  void *shmat_return = shmat(shmid, 0, 0);

  if (shmat_return == reinterpret_cast<void *>(-1)) {
    data_ = NULL;
    if (errno == EMFILE) {
      LM_X(1,
           ("Error with shared memory while attaching to local memory, "
            "the number of shared memory segments has reached the system-wide limit.\n"
            "Use 'ipcs' to check the allocated shared memory segments."
            "(shmat error %d:%s)(shared memory id %d shmid %d size %lu)\n",
            errno, strerror(errno), id, shmid, size));
    } else {
      LM_X(1,
           ("Error with shared memory while attaching to local memory "
            "(shmat error %d:%s)(shared memory id %d shmid %d size %lu)\n",
            errno, strerror(errno), id, shmid, size));
    }
  } else {
    data_ =  static_cast<char *> (shmat_return);
  }
}

SharedMemoryItem::~SharedMemoryItem() {
  // Detach data if it was previously attached
  if (data_) {
    if (shmdt(data_) == -1) {
      LM_X(1, ("Error (%s) calling shmdt in destructor", strerror(errno)));
    }
  }
}
}
