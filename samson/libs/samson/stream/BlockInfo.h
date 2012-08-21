#ifndef _H_BLOCK_INFO
#define _H_BLOCK_INFO

#include <string>                       // std::string

#include "samson/common/FullKVInfo.h"
#include "samson/common/KVInfo.h"
#include "samson/module/KVFormat.h"

namespace samson {
/**
 * Structure used to report information about blocks
 **/

struct BlockInfo {
  int num_blocks;

  // Size of the blocks ( including headers )
  size_t size;
  size_t size_on_memory;
  size_t size_on_disk;
  size_t size_locked;

  FullKVInfo info;      // Number of key-values and data size

  KVFormat format;      // Common format for all the information

  time_t min_time;
  time_t max_time;

  // Default constructor to initialize all values
  BlockInfo();

  // Push information
  // ----------------------------------------------------------------
  void push(KVFormat _format);
  void pushTime(time_t time);

  // Get information
  // ----------------------------------------------------------------

  time_t min_time_diff();
  time_t max_time_diff();
  double getOverhead();
  bool isContentOnMemory();
  bool isContentOnDisk();
  double onMemoryPercentadge();
  double onDiskPercentadge();
  double lockedPercentadge();

  // Debug strings
  // ----------------------------------------------------------------
  std::string str();
  std::string strShort();
};
}

#endif // ifndef _H_BLOCK_INFO
