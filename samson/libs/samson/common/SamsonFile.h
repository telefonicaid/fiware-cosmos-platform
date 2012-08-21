
#ifndef _H_SAMSON_FILE
#define _H_SAMSON_FILE

#include <string>       // std::string

#include "au/ErrorManager.h"
#include "au/containers/SharedPointer.h"

#include "samson/common/KVFile.h"
#include "samson/common/KVHeader.h"
#include "samson/module/KVFormat.h"

namespace samson {
class SamsonFile {
  SamsonFile() {
  };

public:

  static au::SharedPointer<SamsonFile> create(const std::string file_name, au::ErrorManager& error);
  ~SamsonFile() {
  };

  void printHeader(std::ostream& output);
  void printHashGroups(std::ostream &output);
  size_t printContent(size_t limit, bool show_hg, std::ostream &output);

  // std::string getContent( size_t limit , size_t *records );

  KVHeader header() {
    return kv_file_->header();
  }

private:

  au::SharedPointer<KVFile> kv_file_;
  std::string file_name_;
};
}

#endif // ifndef _H_SAMSON_FILE
