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
 * FILE                     SharedMemoryManager.cpp
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

#include "samson/stream/SharedMemoryManager.h"  // Own interface

#include <sstream>                               // std::stringstream

#include "au/file.h"                             // au::sizeOfFile
#include "au/mutex/TokenTaker.h"                 // au::TokenTake
#include "au/string/StringUtilities.h"           // au::Format
#include "engine/Buffer.h"                       // samson::Buffer
#include "engine/Engine.h"                       // samson::Engine
#include "engine/Logs.h"
#include "logMsg/logMsg.h"                       // LM_*
#include "logMsg/traceLevels.h"                  // Trace Levels
#include "samson/common/Logs.h"
#include "samson/stream/SharedMemoryItem.h"   // samson::SharedMemoryItem


#include "samson/common/SamsonSetup.h"   // samson::SamsonSetup

namespace samson {
SharedMemoryManager *sharedMemoryManager = NULL;      // Unique shared memory manager

void SharedMemoryManager::Init(int _shared_memory_num_buffers, size_t _shared_memory_size_per_buffer) {
  if (sharedMemoryManager) {
    LOG_SW(("Init Shared memory manager only once"));
    return;
  }

  // Init the global shard memory element ( after init samsonSetup )
  sharedMemoryManager = new SharedMemoryManager(_shared_memory_num_buffers, _shared_memory_size_per_buffer);
}

SharedMemoryManager *SharedMemoryManager::Shared() {
  if (!sharedMemoryManager) {
    LM_X(1, ("SharedMemoryManager was not initialized"));
  }
  return sharedMemoryManager;
}

void SharedMemoryManager::Destroy() {
  if (sharedMemoryManager) {
    delete sharedMemoryManager;
    sharedMemoryManager = NULL;
  }
}

SharedMemoryManager::SharedMemoryManager(int shared_memory_num_buffers, size_t shared_memory_size_per_buffer)
  : token_("SharedMemoryManager") {
  // Number of buffers and size
  shared_memory_num_buffers_      = shared_memory_num_buffers;
  shared_memory_size_per_buffer_  = shared_memory_size_per_buffer;

  // Name of the log file to store how to clean up memory
  sharedMemoryIdsFileName_ = au::Singleton<samson::SamsonSetup>::shared()->shared_memory_log_filename();

  if (shared_memory_size_per_buffer_ == 0) {
    LM_X(1, ("Error in setup, invalid value for shared memory size %u", shared_memory_size_per_buffer));  // Boolean vector showing if a buffer is used
  }

  // Vector to flag used bufers
  shared_memory_used_buffers_ = (bool *)malloc(shared_memory_num_buffers * sizeof(bool));
  for (int i = 0; i < shared_memory_num_buffers; ++i) {
    shared_memory_used_buffers_[i] = false;
  }

  // Create the shared memory segments
  CreateSharedMemorySegments();

  LOG_D(logs.shared_memory, ("SharedMemoryManager init %d shared memory areas", shared_memory_num_buffers));
}

SharedMemoryManager::~SharedMemoryManager() {
  // Remove the shared memory segments
  RemoveSharedMemorySegments(shm_ids_, shared_memory_num_buffers_);

  // Removing all shared memory items
  for (size_t ix = 0; ix < shared_memory_items_.size(); ++ix) {
    SharedMemoryItem *smItemP;

    smItemP = shared_memory_items_.back();
    shared_memory_items_.pop_back();
    delete smItemP;
  }

  // Remove the file since it will not be necessary
  remove(sharedMemoryIdsFileName_.c_str());

  // Free the vector of flags for shared memory areas
  free(shared_memory_used_buffers_);
  free(shm_ids_);
}

#pragma mark ----

void SharedMemoryManager::RemovePreviousSharedAreas() {
  size_t fileSize = au::sizeOfFile(sharedMemoryIdsFileName_);

  int length = fileSize / sizeof(int);
  int *ids = (int *)malloc(length * sizeof(int));

  if (!ids) {
    LM_X(1, ("Malloc returned NULL"));
  }
  FILE *file = fopen(sharedMemoryIdsFileName_.c_str(), "r");
  if (file) {
    if (fread(ids, length * sizeof(int), 1, file) == 1) {
      LOG_D(logs.shared_memory, ("Removing previous memory segments"));
      RemoveSharedMemorySegments(ids, length);
    }
    fclose(file);
  }

  free(ids);
}

void SharedMemoryManager::WriteCurrentSharedAreasToFile() {
  FILE *file = fopen(sharedMemoryIdsFileName_.c_str(), "w");

  if (file) {
    if (!fwrite(shm_ids_, shared_memory_num_buffers_ * sizeof(int), 1, file) == 1) {
      LOG_SW(("Not possible to write shared memory segments on file %s. This could be a problem if the app crashes.",
              sharedMemoryIdsFileName_.c_str()));
    }
    fclose(file);
  } else {
    LOG_SW(("Not possible to write shared memory segments on file %s. This could be a problem if the app crashes.",
            sharedMemoryIdsFileName_.c_str()));
  }
}

void SharedMemoryManager::CreateSharedMemorySegments() {
  // First try to remove the previous shared memory segments ( if any )
  RemovePreviousSharedAreas();

  // Create a new shared memory buffer
  shm_ids_ = static_cast<int *>(malloc(sizeof(int) * shared_memory_num_buffers_));

  LOG_D(logs.shared_memory, ("Creating shared memory buffers"));

  for (int i = 0; i < shared_memory_num_buffers_; ++i) {
    int shmflg;                 /* shmflg to be passed to shmget() */

    // shmflg  = 384;		// Permission to read / write ( only owner )
    shmflg  = 384;              // Permission to read / write ( only owner )

    int shmid = shmget(IPC_PRIVATE, shared_memory_size_per_buffer_, shmflg);

    if (shmid <= 0) {
      LM_E((
             "Error creating the shared memory buffer of %s ( %d / %d ). \
              Please review SAMSON documentation about shared memory usage",
             au::str(shared_memory_size_per_buffer_, "B").c_str(), i, shared_memory_num_buffers_));

      LM_X(1, ("Error creating shared memory buffers. shmid return -1 (%s)", strerror(errno)));
    }
    shm_ids_[i] = shmid;
  }

  WriteCurrentSharedAreasToFile();


  // Create the SharedMemoryItem's
  for (int i = 0; i < shared_memory_num_buffers_; ++i) {
    shared_memory_items_.push_back(CreateSharedMemory(i));
  }
}

void SharedMemoryManager::RemoveSharedMemorySegments(int *ids, int length) {
  LOG_D(logs.shared_memory, ("Releasing %d shared memory buffers", length));

  for (int i = 0; i < length; ++i) {
    // Remove the shared memory areas
    if (shmctl(ids[i], IPC_RMID, NULL) == -1) {
      LOG_SD(("Error ('%s') trying to release a shared memory buffer(%d)", strerror(errno), ids[i]));
    }
  }
}

int SharedMemoryManager::RetainSharedMemoryArea() {
  au::TokenTaker tk(&token_);

  for (int i = 0; i < shared_memory_num_buffers_; ++i) {
    if (!shared_memory_used_buffers_[i]) {
      shared_memory_used_buffers_[i] = true;
      return i;
    }
  }

  LOG_SW(("Not possible to retain a shared memory area since all %d are busy", shared_memory_num_buffers_));
  return -1;                    // There are no available memory buffers, so we will never get this point
}

void SharedMemoryManager::ReleaseSharedMemoryArea(int id) {
  au::TokenTaker tk(&token_);


  if ((id < 0) || (id > shared_memory_num_buffers_)) {
    LM_X(1, ("Releaseing a wrong Shared Memory Id %d", id));
  }
  shared_memory_used_buffers_[id] = false;
}

SharedMemoryItem *SharedMemoryManager::GetSharedMemoryPlatform(int i) const {
  if ((i < 0) || (i >= (int)shared_memory_items_.size())) {
    return NULL;
  }

  return shared_memory_items_[i];
}

SharedMemoryItem *SharedMemoryManager::GetSharedMemoryChild(int i) {
  return CreateSharedMemory(i);
}

std::string SharedMemoryManager::str() {
  if (sharedMemoryManager) {
    return sharedMemoryManager->_str();
  } else {
    return "SharedMemoryManager not initialized";
  }
}

std::string SharedMemoryManager::_str() {
  std::ostringstream output;

  int used_shared_memory_num_buffers = 0;

  for (int i = 0; i < shared_memory_num_buffers_; ++i) {
    if (shared_memory_used_buffers_[i]) {
      used_shared_memory_num_buffers++;
    }
  }

  output << au::str("Size: %s Used %d/%d",
                    au::str(shared_memory_size_per_buffer_).c_str(),
                    used_shared_memory_num_buffers,
                    shared_memory_num_buffers_);

  return output.str();
}

SharedMemoryItem *SharedMemoryManager::CreateSharedMemory(int i) {
  // Create a new shared memory area
  SharedMemoryItem *_info = new SharedMemoryItem(i, shm_ids_[i], shared_memory_size_per_buffer_);

  return _info;
}
}
