

#include "au/log/LogPluginFile.h"  // Own interface

namespace au {
LogPluginFile::LogPluginFile(std::string file_name) : LogPlugin("file") {
  set_local_file(file_name);
}

void LogPluginFile::set_local_file(const std::string& file_name) {
  // Close previous if any
  local_file_descriptor_ = NULL;

  // Save te name of the local file
  local_file_ = file_name;

  // Open local file ( if possible )
  int fd = open(local_file_.c_str(), O_WRONLY | O_CREAT, 0644);
  if (fd >= 0) {
    local_file_descriptor_ = new FileDescriptor("local_log", fd);
  } else {
    local_file_descriptor_ = NULL;
    // LM_LW(("Not possible to open local log file %s. Logs will be definitely lost", local_file_.c_str()));
  }
}

void LogPluginFile::Emit(SharedPointer<Log> log) {
  // It was not possible, so use disk
  if (local_file_descriptor_ != NULL) {
    if (!log->Write(local_file_descriptor_.shared_object())) {
      // Local log?
    }
  }
}

std::string LogPluginFile::status() {
  return au::str("LocalFile: %s", local_file_.c_str());
}
}