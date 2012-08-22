/* ****************************************************************************
 *
 * FILE                     SharedMemoryManager.cpp
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

#include <sstream>                               // std::stringstream

#include "logMsg/logMsg.h"                       // LM_*
#include "logMsg/traceLevels.h"                  // Trace Levels

#include "au/file.h"                             // au::sizeOfFile
#include "au/mutex/TokenTaker.h"                 // au::TokenTake
#include "au/string.h"                           // au::Format

#include "SharedMemoryItem.h"   // samson::SharedMemoryItem
#include "engine/Buffer.h"                       // samson::Buffer
#include "engine/Engine.h"                       // samson::Engine


#include "samson/isolated/SharedMemoryManager.h"  // Own interface

#include "samson/common/SamsonSetup.h"   // samson::SamsonSetup

namespace engine {
SharedMemoryManager *sharedMemoryManager = NULL;      // Unique shared memory manager

void SharedMemoryManager::init(int _shared_memory_num_buffers, size_t _shared_memory_size_per_buffer) {
  if (sharedMemoryManager) {
    LM_W(("Init Shared memory manager only once"));
    return;
  }

  // Init the global shard memory element ( after init samsonSetup )
  sharedMemoryManager = new SharedMemoryManager(_shared_memory_num_buffers, _shared_memory_size_per_buffer);
}

SharedMemoryManager *SharedMemoryManager::shared() {
  if (!sharedMemoryManager) {
    LM_X(1, ("SharedMemoryManager was not initialized"));
  }
  return sharedMemoryManager;
}

void SharedMemoryManager::destroy() {
  if (sharedMemoryManager) {
    delete sharedMemoryManager;
    sharedMemoryManager = NULL;
  }
}

SharedMemoryManager::SharedMemoryManager(int _shared_memory_num_buffers, size_t _shared_memory_size_per_buffer)
  : token("SharedMemoryManager") {
  // Default values ( no shared memories )
  shared_memory_num_buffers      = _shared_memory_num_buffers;
  shared_memory_size_per_buffer  = _shared_memory_size_per_buffer;

  // Name of the log file to store how to clean up memory
  sharedMemoryIdsFileName = samson::SamsonSetup::shared()->sharedMemoryLogFileName();

  if (shared_memory_size_per_buffer == 0) {
    LM_X(1, ("Error in setup, invalid value for shared memory size %u", shared_memory_size_per_buffer ));  // Boolean vector showing if a buffer is used
  }
  shared_memory_used_buffers = (bool *)malloc(shared_memory_num_buffers * sizeof(bool));

  for (int i = 0; i < shared_memory_num_buffers; i++) {
    shared_memory_used_buffers[i] = false;
  }

  // Create the shared memory segments
  createSharedMemorySegments();

  LM_T(LmtMemory, ("SharedMemoryManager init %d shared memory areas", shared_memory_num_buffers ));
}

SharedMemoryManager::~SharedMemoryManager() {
  // Remove the shared memory segments
  removeSharedMemorySegments(shm_ids, shared_memory_num_buffers);

  // Removing all shared memory items
  int smItems = (int)shared_memory_items.size();
  for (int ix = 0; ix < smItems; ix++) {
    SharedMemoryItem *smItemP;

    smItemP = shared_memory_items.back();
    shared_memory_items.pop_back();

    delete smItemP;
  }

  // Remove the file since it will not be necessary
  remove(sharedMemoryIdsFileName.c_str());

  // Free the vector of flags for shared memory areas
  free(shared_memory_used_buffers);
  free(shm_ids);
}

#pragma mark ----

void SharedMemoryManager::remove_previous_shared_areas() {
  size_t fileSize = au::sizeOfFile(sharedMemoryIdsFileName);

  int length = fileSize / sizeof(int);
  int *ids = (int *)malloc(length * sizeof(int));

  if (!ids) {
    LM_X(1, ("Malloc returned NULL"));
  }
  FILE *file = fopen(sharedMemoryIdsFileName.c_str(), "r");
  if (file) {
    if (fread(ids, length * sizeof(int), 1, file) == 1) {
      LM_T(LmtMemory, ("Removing previous memory segments"));
      removeSharedMemorySegments(ids, length);
    }
    fclose(file);
  }

  free(ids);
}

void SharedMemoryManager::write_current_shared_areas_to_file() {
  FILE *file = fopen(sharedMemoryIdsFileName.c_str(), "w");

  if (file) {
    if (!fwrite(shm_ids, shared_memory_num_buffers * sizeof(int), 1, file) == 1) {
      LM_W(("Not possible to write shared memory segments on file %s. This could be a problem if the app crashes.",
            sharedMemoryIdsFileName.c_str()));
    }
    fclose(file);
  } else {
    LM_W(("Not possible to write shared memory segments on file %s. This could be a problem if the app crashes.",
          sharedMemoryIdsFileName.c_str()));
  }
}

void SharedMemoryManager::createSharedMemorySegments() {
  // First try to remove the previous shared memory segments ( if any )
  remove_previous_shared_areas();

  // Create a new shared memory buffer
  shm_ids = (int *)malloc(sizeof(int) * shared_memory_num_buffers);

  LM_T(LmtMemory, ("Creating shared memory buffers"));

  for (int i = 0; i < shared_memory_num_buffers; i++) {
    int shmflg;                 /* shmflg to be passed to shmget() */

    // shmflg  = 384;		// Permission to read / write ( only owner )
    shmflg  = 384;              // Permission to read / write ( only owner )

    int shmid = shmget(IPC_PRIVATE, shared_memory_size_per_buffer, shmflg);

    if (shmid <= 0) {
      LM_E((
             "Error creating the shared memory buffer of %s ( %d / %d ). \
              Please review SAMSON documentation about shared memory usage",
             au::str(shared_memory_size_per_buffer, "B").c_str(), i, shared_memory_num_buffers ));
      LM_X(1, ("Error creating shared memory buffers. shmid return -1 (%s)", strerror(errno)));
    }
    shm_ids[i] = shmid;
  }

  write_current_shared_areas_to_file();


  // Create the SharedMemoryItem's
  for (int i = 0; i < shared_memory_num_buffers; i++) {
    shared_memory_items.push_back(createSharedMemory(i));
  }
}

void SharedMemoryManager::removeSharedMemorySegments(int *ids, int length) {
  LM_T(LmtMemory, ("Releasing %d shared memory buffers", length ));

  for (int i = 0; i < length; i++) {
    // Remove the shared memory areas
    if (shmctl(ids[i], IPC_RMID, NULL) == -1) {
      LM_W((
             "Error trying to release a shared memory buffer Please review shared-memory problems in SAMSON documentation"));
    }
  }
}

int SharedMemoryManager::RetainSharedMemoryArea() {
  au::TokenTaker tk(&token);

  for (int i = 0; i < shared_memory_num_buffers; i++) {
    if (!shared_memory_used_buffers[i]) {
      shared_memory_used_buffers[i] = true;
      return i;
    }
  }

  LM_W(("Not possible to retain a shared memory area since all %d are busy", shared_memory_num_buffers ));
  return -1;                    // There are no available memory buffers, so we will never get this point
}

void SharedMemoryManager::ReleaseSharedMemoryArea(int id) {
  au::TokenTaker tk(&token);


  if ((id < 0) || ( id > shared_memory_num_buffers)) {
    LM_X(1, ("Releaseing a wrong Shared Memory Id %d", id));
  }
  shared_memory_used_buffers[id] = false;
}

SharedMemoryItem *SharedMemoryManager::getSharedMemoryPlatform(int i) {
  if (( i < 0 ) || ( i >= (int)shared_memory_items.size())) {
    return NULL;
  }

  return shared_memory_items[i];
}

SharedMemoryItem *SharedMemoryManager::getSharedMemoryChild(int i) {
  return createSharedMemory(i);
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

  for (int i = 0; i < shared_memory_num_buffers; i++) {
    if (shared_memory_used_buffers[i]) {
      used_shared_memory_num_buffers++;
    }
  }

  output << au::str("Size: %s Used %d/%d",
                    au::str(shared_memory_size_per_buffer).c_str(),
                    used_shared_memory_num_buffers,
                    shared_memory_num_buffers);

  return output.str();
}

SharedMemoryItem *SharedMemoryManager::createSharedMemory(int i) {
  // Create a new shared memory area
  SharedMemoryItem *_info = new SharedMemoryItem(i, shm_ids[i], shared_memory_size_per_buffer);

  return _info;
}
}
