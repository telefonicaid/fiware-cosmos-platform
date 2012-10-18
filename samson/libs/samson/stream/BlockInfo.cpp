

#include "au/string/string.h"
#include "au/string/xml.h"

#include "BlockInfo.h"      // Own interface
#include "samson/common/coding.h"


namespace samson {
BlockInfo::BlockInfo() {
  num_blocks = 0;
  size = 0;
  size_on_memory = 0;
  size_on_disk = 0;
  size_locked = 0;

  format = KVFormat("*", "*");

  // Initial values for time
  min_time = 0;
  max_time = 0;
}

double BlockInfo::onMemoryPercentadge() {
  if (size == 0) {
    return 0;
  }

  return (double)size_on_memory / (double)size;
}

double BlockInfo::onDiskPercentadge() {
  if (size == 0) {
    return 0;
  }

  return (double)size_on_disk / (double)size;
}

double BlockInfo::lockedPercentadge() {
  if (size == 0) {
    return 0;
  }

  return (double)size_locked / (double)size;
}

void BlockInfo::push(KVFormat _format) {
  if (format == KVFormat("*", "*")) {
    format = _format;
  } else if (format != KVFormat("?", "?")) {
    if (format != _format) {
      format = KVFormat("?", "?");
    }
  }
}

void BlockInfo::pushTime(time_t time) {
  if (min_time == 0) {
    min_time = time;
    max_time = time;
    return;
  }

  if (time < min_time) {
    min_time = time;
  }
  if (time > max_time) {
    max_time = time;
  }
}

time_t BlockInfo::min_time_diff() {
  if (min_time == 0) {
    return 0;
  }

  return time(NULL) - min_time;
}

time_t BlockInfo::max_time_diff() {
  if (max_time == 0) {
    return 0;
  }

  return time(NULL) - max_time;
}

double BlockInfo::getOverhead() {
  if (size == 0) {
    return 0;
  }

  return (double)( size - info.size  ) / (double)size;
}

bool BlockInfo::isContentOnMemory() {
  return ( size == size_on_memory );
}

bool BlockInfo::isContentOnDisk() {
  return ( size == size_on_disk );
}

  std::string BlockInfo::str() {
    return au::str("%s [ %s | %s on memory / %s on disk / %s locked ] %s "
                   , au::str(num_blocks, "Blocs").c_str()
                   , au::str(size, "bytes").c_str()
                   , au::str_percentage(size_on_memory, size).c_str()
                   , au::str_percentage(size_on_disk, size).c_str()
                   , au::str_percentage(size_locked, size).c_str()
                   , info.str().c_str()
                   );
  }

  std::string BlockInfo::strShort() {
  if (num_blocks == 0) {
    return "empty";
  }

  return au::str("%3dbs %s %s %c%c"
                 , num_blocks
                 , au::str(info.kvs, "kvs").c_str()
                 , au::str(info.size, "B").c_str()
                 , ( size_locked == size ) ? 'L' : (( size_on_memory == size ) ? 'M' : ' ')
                 , ( size_on_disk == size ) ? 'D' : ' '
                 );
}
  std::string BlockInfo::strShortInfo() {
    if (num_blocks == 0) {
      return "empty";
    }
    
    return au::str("%3dbs %s %s"
                   , num_blocks
                   , au::str(info.kvs, "kvs").c_str()
                   , au::str(info.size, "B").c_str()
                   );
  }
}