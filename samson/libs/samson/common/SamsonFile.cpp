

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>  // read

#include "samson/module/ModulesManager.h"  // samson::ModulesManager

#include "logMsg/logMsg.h"                 // LM_T
#include "logMsg/traceLevels.h"            // LmtFileDescriptors, etc.

#include "SamsonFile.h"                    // Own interface

namespace samson {
au::SharedPointer<SamsonFile> SamsonFile::create(const std::string file_name, au::ErrorManager& error) {
  // Candidate samson file
  au::SharedPointer<SamsonFile> samson_file(new SamsonFile());

  // Keep file name
  samson_file->file_name_ = file_name;

  // Get the size of the file
  struct ::stat info;
  if (stat(file_name.c_str(), &info) != 0) {
    error.set(au::str("Error reading file %s (%s)", file_name.c_str(), strerror(errno)));
    return au::SharedPointer<SamsonFile>(NULL);
  }
  size_t file_size = info.st_size;

  if (file_size > 1024 * 1024 * 1024) {
    error.set(au::str("Excesive file size %s (%s)"
                      , file_name.c_str()
                      , au::str(file_size, "B").c_str()));
    return au::SharedPointer<SamsonFile>(NULL);
  }

  // Getting a buffer for this file
  std::string name = au::str("Samson file %s", file_name.c_str());
  engine::BufferPointer buffer = engine::Buffer::create(name, "samson_file", file_size);
  buffer->setSize(file_size);

  // Read content of this file
  FILE *file = fopen(file_name.c_str(), "r");
  if (!file) {
    error.set(au::str("Error reading file %s (%s)", file_name.c_str(), strerror(errno)));
    return au::SharedPointer<SamsonFile>(NULL);
  }

  if (fread(buffer->getData(), file_size, 1, file) != 1) {
    error.set(au::str("Error reading file %s (%s)", file_name.c_str(), strerror(errno)));
    return au::SharedPointer<SamsonFile>(NULL);
  }

  fclose(file);

  // Create File from the buffer
  samson_file->kv_file_ = KVFile::create(buffer, error);

  if (error.IsActivated())
    return au::SharedPointer<SamsonFile>(NULL);

  // Return the newly created samson_file
  return samson_file;
}

class SimpleBuffer {
public:

  char *data;

  SimpleBuffer(std::string fileName) {
    size_t nb;

    // In case of error
    data = NULL;

    struct stat filestatus;
    if (stat(fileName.c_str(), &filestatus) != 0)
      return;

    FILE *file = fopen(fileName.c_str(), "r");

    data = (char *)malloc(filestatus.st_size);

    nb = fread(data, filestatus.st_size, 1, file);
    if (nb == 0)
      LM_W(("No data read from file:'%s'", fileName.c_str())); fclose(file);
  }

  SimpleBuffer(size_t size) {
    if (data)
      data = ( char * )malloc(size);
  }

  ~SimpleBuffer() {
    free(data);
  }
};

void SamsonFile::printHeader(std::ostream& output) {
  KVHeader header = kv_file_->header();

  std::cout << header.str() << " " << " [ " << header.info.kvs << " kvs in " << header.info.size << "bytes]\n";
}

void SamsonFile::printHashGroups(std::ostream &output) {
  if (header().isTxt()) {
    output << "No hash-groups in txt packet\n";
    return;
  }

  for (int i = 0; i < KVFILE_NUM_HASHGROUPS; i++) {
    if (kv_file_->info[i].size > 0)
      output << i << " " << kv_file_->info[i].kvs << " " << kv_file_->info[i].size << "\n";
  }
}

size_t SamsonFile::printContent(size_t limit, bool show_hg, std::ostream &output) {
  return kv_file_->printContent(limit, show_hg, output);
}
}
