#ifndef _H_SAMSON_KVFILE
#define _H_SAMSON_KVFILE

#include "au/ErrorManager.h"
#include "engine/Buffer.h"
#include "samson/common/KVHeader.h"
#include "samson/common/KVInfo.h"
#include "samson/module/DataInstance.h"
#include "samson/module/KV.h"


namespace samson {
/**
 * Structure used to work with contents of KVFiles
 **/

class KVFile {
  KVFile() {
    // Default values ( for correct deallocation in case of errors )
    kvs = NULL;
    info = NULL;
    kvs_index = NULL;
  };

public:

  static au::SharedPointer<KVFile> create(engine::BufferPointer buffer, au::ErrorManager& error);
  ~KVFile();

  // Get header information
  KVHeader header();

  // Main interface to interact with key-values contained in this block
  KV *kvs;             // Pointers to all key-values
  KVInfo *info;        // Vector with all KVInfos for each hash-group
  int *kvs_index;      // Index in the kvs vector for each hash-group

  // Main interface to interact with content if txt content is provided
  char *data;
  size_t data_size;

  // Print content of key-values ( mainly drebugging )
  size_t printContent(size_t limit, bool show_hg, std::ostream &output);

  // Get key-values vector for a particular hash-group
  KV *kvs_for_hg(int hg) {
    return &kvs[kvs_index[hg]];
  }

  char* data_for_hg(int hg)
  {
    return kvs[kvs_index[hg]].key;
  }
private:

  // Buffer of data ( retained by the shared reference counter )
  engine::BufferPointer buffer_;

  // Copy of the heder contained in the buffer
  KVHeader header_;

  // Data instances for parsing and printing contente
  DataInstance *key_;
  DataInstance *value_;
};
}

#endif  // ifndef _H_SAMSON_KVFILE
