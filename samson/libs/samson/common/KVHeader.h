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
  time_t time;                  // Time stamp for this block ( for debugging )

  // Init header
  // ---------------------------------------------------------------
  void Init(KVFormat format, KVInfo _info);
  void InitForTxt(size_t size);
  void InitForModule(size_t size);

  // Set extra information
  // ---------------------------------------------------------------
  void SetKVFormat(KVFormat format);

  // Functions to set of get information from the header
  // ---------------------------------------------------------------
  bool IsTxt();
  bool IsModule();

  // Format operations
  // ---------------------------------------------------------------
  KVFormat GetKVFormat();

  // Check operations ( magic number and other conditions )
  // ---------------------------------------------------------------
  bool CheckTotalSize(size_t total_size);
  bool Check();

  // Debug strings
  // ---------------------------------------------------------------

  std::string str() const;
};
}

#endif  // ifndef _H_SAMSON_KVHEADER
