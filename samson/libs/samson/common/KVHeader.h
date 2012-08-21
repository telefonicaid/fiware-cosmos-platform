#ifndef _H_SAMSON_KVHEADER
#define _H_SAMSON_KVHEADER

#include "samson/module/KVFormat.h"

#include "KVInfo.h"

namespace samson {
/**
 * Header used in KV-Sets ( Files, Network messages, Operations, etc...)
 */

struct KVHeader {
  // Information about the packet
  // ---------------------------------------------------------------

  int magic_number;             // Magic number to make sure reception is correct
  char keyFormat[100];          // Format for the key
  char valueFormat[100];        // Format for the value

  KVInfo info;                  // Total information in this package ( in all hash-groups )
  KVRange range;                // Range of has-groups used in this file

  time_t time;                  // Time stamp for this block

  // Init header
  // ---------------------------------------------------------------

  void init(KVFormat format, KVInfo _info);
  void initForTxt(size_t size);

  // Set extra information
  // ---------------------------------------------------------------

  void setHashGroups(uint32 _hg_begin, uint32 _hg_end);
  void setInfo(KVInfo _info);
  void setFormat(KVFormat format);


  // Functions to set of get information from the header
  // ---------------------------------------------------------------

  uint32 getTotalSize();
  uint32 getNumHashGroups();
  bool isTxt();

  // Format operations
  // ---------------------------------------------------------------

  KVFormat getKVFormat();

  // Check operations ( magic number and other conditions )
  // ---------------------------------------------------------------

  bool check_size(size_t total_size);
  bool check();

  // Debug strings
  // ---------------------------------------------------------------

  std::string str();
  void getInfo(std::ostringstream &output);
};
}

#endif  // ifndef _H_SAMSON_KVHEADER
