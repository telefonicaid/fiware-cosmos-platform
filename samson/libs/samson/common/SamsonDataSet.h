
#ifndef _H_SAMSON_DATA_SET
#define _H_SAMSON_DATA_SET

#include <string>                           // std::string

#include "au/containers/Dictionary.h"

#include "samson/common/FullKVInfo.h"

#include "samson/common/SamsonFile.h"      // samson::SamsonFile

namespace samson {
class SamsonDataSet {
  SamsonDataSet() {
  };

public:

  static au::SharedPointer<SamsonDataSet> create(const std::string& directory, au::ErrorManager& error);
  ~SamsonDataSet() {
  };

  void printHashGroups(std::ostream &output);
  void printHeaders(std::ostream &output);
  void printContent(size_t limit, bool show_hg, std::ostream &output);

  std::string strSize();
  std::string strFormat();

  KVFormat format();
  FullKVInfo info();

private:

  au::Dictionary< std::string, SamsonFile > samson_files;
  KVFormat format_;       // Common format
  FullKVInfo info_;       // Common info
};
}

#endif // ifndef _H_SAMSON_DATA_SET