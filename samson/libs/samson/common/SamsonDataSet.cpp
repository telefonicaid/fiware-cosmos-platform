/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#include <dirent.h>
#include <sys/stat.h>


#include "SamsonDataSet.h"      // Own interface
#include "samson/common/coding.h"


namespace samson {
au::SharedPointer<SamsonDataSet> SamsonDataSet::create(const std::string& directory, au::ErrorManager& error) {
  // Candidate to return
  au::SharedPointer<SamsonDataSet> samson_data_set(new SamsonDataSet());

  struct stat filestatus;
  stat(directory.c_str(), &filestatus);

  if (!S_ISDIR(filestatus.st_mode)) {
    error.AddError(au::str("%s is not a directory", directory.c_str()));
    return au::SharedPointer<SamsonDataSet>(NULL);
  }

  // Loop inside the directory to scan
  DIR *pdir = opendir(directory.c_str());
  struct dirent *pent = NULL;
  if (pdir != NULL) {  // if pdir wasn't initialised correctly
    while ((pent = readdir(pdir))) {  // while there is still something in the directory to list
      if (pent != NULL) {
        std::ostringstream localFileName;
        localFileName << directory << "/" << pent->d_name;

        struct stat buf2;
        stat(localFileName.str().c_str(), &buf2);

        if (S_ISREG(buf2.st_mode)) {
          au::SharedPointer<SamsonFile> samson_file = SamsonFile::create(localFileName.str(), error);

          if (error.HasErrors()) {
            return au::SharedPointer<SamsonDataSet>(NULL);
          }

          std::string name = pent->d_name;
          samson_data_set->samson_files.Set(name, samson_file);
        }
      }
    }
    // finally, let's close the directory
    closedir(pdir);
  }

  // Check content
  if (samson_data_set->samson_files.size() == 0) {
    error.AddError("No content");
    return au::SharedPointer<SamsonDataSet>(NULL);
  }

  // Check common format
  std::vector< au::SharedPointer<SamsonFile> > items = samson_data_set->samson_files.items();
  samson_data_set->format_ = items[0]->header().GetKVFormat();
  for (size_t i = 1; i < items.size(); i++) {
    if (items[i]->header().GetKVFormat() != samson_data_set->format_) {
      error.AddError("Different formats found in included files");
      return au::SharedPointer<SamsonDataSet>(NULL);
    }
  }

  // Compute total info
  for (size_t i = 0; i < items.size(); i++) {
    samson_data_set->info_.append(items[i]->header().info);
  }

  return samson_data_set;
}

KVFormat SamsonDataSet::format() {
  return format_;
}

FullKVInfo SamsonDataSet::info() {
  return info_;
}

void SamsonDataSet::printHashGroups(std::ostream &output) {
  std::vector<std::string> file_names = samson_files.getKeysVector();
  for (size_t i = 0; i < file_names.size(); i++) {
    au::SharedPointer<SamsonFile> samson_file = samson_files.Get(file_names[i]);
    output << file_names[i] << std::endl;
    samson_file->printHashGroups(output);
    output << std::endl;
  }
}

void SamsonDataSet::printHeaders(std::ostream &output) {
  std::vector<std::string> file_names = samson_files.getKeysVector();
  for (size_t i = 0; i < file_names.size(); i++) {
    au::SharedPointer<SamsonFile> samson_file = samson_files.Get(file_names[i]);
    output << file_names[i] << std::endl;
    samson_file->printHeader(output);
    output << std::endl;
  }
}

void SamsonDataSet::printContent(size_t limit, bool show_hg, std::ostream &output) {
  size_t records = 0;

  std::vector<std::string> file_names = samson_files.getKeysVector();
  for (size_t i = 0; i < file_names.size(); i++) {
    au::SharedPointer<SamsonFile> samson_file = samson_files.Get(file_names[i]);

    records += samson_file->printContent((limit == 0) ? 0 : (limit - records), show_hg, output);

    if (limit > 0) {
      if (records >= limit) {
        return;
      }
    }
  }
}
}
