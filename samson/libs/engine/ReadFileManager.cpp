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

#include "ReadFile.h"               // engine::ReadFile
#include "ReadFileManager.h"        // Own interface

namespace engine {
ReadFileManager::ReadFileManager() {
  // Default number of open files
  max_open_files_ = 100;
}

ReadFileManager::~ReadFileManager() {
  read_files_.clearListMap();
}

ReadFile *ReadFileManager::GetReadFile(const std::string& file_name) {
  // Recover ReadFile for this filename
  ReadFile *f = read_files_.extractFromMap(file_name);

  // Remove non-valid ReadFiles
  if (f && !f->IsValid()) {
    delete f;
    f = NULL;
  }

  if (f == NULL) {
    f = new ReadFile(file_name);
  }

  if (f && !f->IsValid()) {
    delete f;
    return NULL;
  }

  // Insert at front ( make sure most recent are at front )
  read_files_.insertAtFront(file_name, f);

  // Remove old File descriptors if necessary
  while ((int)read_files_.size() > max_open_files_) {
    ReadFile *rf = read_files_.extractFromBack();

    if (rf == f) {
      LM_X(1, ("Internal error"));
    }
    delete rf;
  }

  return f;
}
}
