#ifndef _H_TXT_FILE_SET
#define _H_TXT_FILE_SET

#include "au/file.h"

namespace samson {
class Delilah;
class Buffer;



class DataSource {
public:

  virtual ~DataSource() {
  }

  virtual bool isFinish() = 0;
  virtual int fill(engine::BufferPointer b) = 0;
  virtual size_t getTotalSize() = 0;
  virtual std::string get_name() = 0;
};

class AgregatedFilesDataSource : public DataSource {
public:

  AgregatedFilesDataSource(const std::vector<std::string> &file_names) {
    file_names_ = file_names;

    // Previous buffer accumulated
    previous_buffer_size_ = 0;
    previous_buffer_ = NULL;

    finish_ = false;
    file_ = 0;

    OpenNextFile();
  }

  virtual std::string get_name() {
    if (file_names_.size() == 1) {
      return au::str("FILE/DIR %s", file_names_[0].c_str());
    } else {
      return au::str("%lu FILEs/DIRs", file_names_.size());
    }
  }

  void OpenNextFile() {
    while (!current_input_stream_.is_open()) {
      if (file_ >= (int)file_names_.size()) {
        finish_ = true;
        return;
      }

      current_input_stream_.open(file_names_[file_].c_str());
      if (!current_input_stream_.is_open()) {
        failed_files_.push_back(file_names_[file_]);  // Add to the list of failed files
      }
      file_++;
    }
  }

  bool isFinish() {
    return finish_;
  }

  // Read as much as possible breaking in lines
  int fill(engine::BufferPointer b);

  std::vector<std::string> getFailedFiles() {
    return failed_files_;
  }

  std::string getStatus() {
    std::ostringstream output;

    if (finish_) {
      output << "finished";
    } else {
      output << "running: " << file_names_.size() << " pending files and " << failed_files_.size() << " failed files";
    }
    return output.str();
  }

  size_t getTotalSize() {
    size_t totalSize = 0;

    // Compute the total size for all the files
    for (size_t i =  0; i < file_names_.size(); i++) {
      totalSize += au::sizeOfFile(file_names_[i]);
    }

    return totalSize;
  }

public:

  std::vector<std::string> file_names_;    // List of files to read
  std::ifstream current_input_stream_;     // Stream of the file we are reading

  int file_;
  bool finish_;

  char *previous_buffer_;                   // Part of the previous buffer waiting for the next read
  size_t previous_buffer_size_;

  std::vector<std::string> failed_files_;   // List of files that could not be uploaded
};


class IndividualFilesDataSources : public DataSource {
public:

  IndividualFilesDataSources(const std::vector<std::string>& file_names) {
    file_names_ = file_names;
    total_size_ = 0;
    for (size_t i = 0; i < file_names_.size(); i++) {
      total_size_ += au::sizeOfFile(file_names_[i]);
    }
    next_file_ = 0;
  }

  virtual ~IndividualFilesDataSources() {
  }

  virtual bool isFinish() {
    return ( next_file_ >= file_names_.size());
  }

  virtual int fill(engine::BufferPointer b) {
    if (isFinish()) {
      return 0;   // No more data to fill
    }
    size_t file_size = au::sizeOfFile(file_names_[next_file_]);

    if (b->GetAvailableSizeToWrite() >= file_size) {
      au::ErrorManager error;
      b->WriteFile(file_names_[next_file_], error);
      if (error.IsActivated()) {
        return 1;   // Error reading this
      }
      next_file_++;
      return 0;   // OK
    }
    return 1;   // Error buffer is not enougth
  }

  virtual size_t getTotalSize() {
    return total_size_;
  }

  virtual std::string get_name() {
    if (file_names_.size() == 1) {
      return au::str("FILE/DIR %s", file_names_[0].c_str());
    } else {
      return au::str("%lu FILEs/DIRs", file_names_.size());
    }
  }

private:

  std::vector<std::string> file_names_;
  size_t total_size_;
  size_t next_file_;
};
}

#endif  // ifndef _H_TXT_FILE_SET
