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
/* ****************************************************************************
 *
 * FILE                     Buffer.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */
#include <sstream>                      // std::stringstream
#include <string>                       // std::string

#include "logMsg/logMsg.h"              // lmInit, LM_*
#include "logMsg/traceLevels.h"         // Trace Levels

#include "au/file.h"
#include "au/string/StringUtilities.h"  // au::Format
#include "au/string/xml.h"              // au::xml...

#include "engine/Buffer.h"              // Own interface
#include "engine/Logs.h"
#include "engine/MemoryManager.h"       // MemoryManager

namespace engine {
size_t Buffer::static_buffer_id_ = 0;

Buffer::Buffer(const std::string& name, size_t max_size) {
  name_ = name;
  size_ = 0;
  offset_ = 0;
  max_size_ = max_size;
  buffer_id_ = static_buffer_id_++;

  if (max_size > 0) {
    data_ = reinterpret_cast<char *>(malloc(max_size));

    if (!data_) {
      LM_X(1, ("Error (errno:%d) allocating memory for %d bytes for name:'%s'"
               , errno, max_size, name.c_str()));
    }
  } else {
    LOG_W(logs.buffer, ("Buffer request of max_size(%lu) <= 0, for name:'%s'",
                        max_size, name.c_str()));
    data_ = NULL;
  }

  LOG_M(logs.buffer, ("Creating buffer %s", str().c_str()));

  // Register in the memory manager to track allocated memory
  Engine::memory_manager()->Add(this);
}

au::SharedPointer<Buffer> Buffer::Create(
  const std::string&  name,
  size_t max_size
  ) {
  if (max_size > 1024 * 1024 * 1024) {
    LOG_E(logs.buffer, ("Excessive size for buffer %s", au::str(max_size).c_str()));
    return au::SharedPointer<Buffer>(NULL);
  }
  return au::SharedPointer<Buffer>(new Buffer(name, max_size));
}

Buffer::~Buffer() {
  LOG_M(logs.buffer, ("Destroying buffer %s", str().c_str()));

  // Unregister in the memory manager
  Engine::memory_manager()->Remove(this);

  // Free allocated data
  if (data_) {
    ::free(data_);
  }
}

std::string Buffer::str()  const {
  return au::str("[ %s ( Size: %s/%s Read %s ) ]"
                 , name_.c_str()
                 , au::str(size_, "B").c_str()
                 , au::str(max_size_, "B").c_str()
                 , au::str(offset_, "B").c_str());
}

/**
 * Function to write content updating the size variable coherently
 * If it is not possible to write the entire block, it return false
 * So, it never try to write less than input_size bytes
 */

bool Buffer::Write(const char *input_buffer, size_t input_size) {
  if (size_ + input_size > max_size_) {
    return false;
  }

  memcpy(data_ + size_, input_buffer, input_size);
  size_ += input_size;

  return true;
}

bool Buffer::SkipWrite(size_t size) {
  if (size_ + size <= max_size_) {
    size_ += size;
    return true;
  } else {
    return false;
  }
}

size_t Buffer::SkipRead(size_t size) {
  if (offset_ + size > max_size_) {
    size = (max_size_ - offset_);       // Max offset
  }
  offset_ += size;
  return size;
}

/**
 * Write on the buffer the maximum possible ammount of data
 */

void Buffer::Write(std::ifstream &inputStream) {
  inputStream.read(data_ + size_, max_size_ - size_);
  size_ += inputStream.gcount();
}

/*
 * Remove the last characters of an unfinished line and put them in buffer.
 * Remove the size of this set of characters
 */

int Buffer::RemoveLastUnfinishedLine(char *& buffer, size_t& buffer_size) {
  size_t last_line_size = 0;

  while ((last_line_size < size_) && (data_[size_ - last_line_size - 1] != '\n')) {
    last_line_size++;
  }

  if (last_line_size == size_) {
    return 1;     // Error... not final line found in the buffer
  }
  buffer = reinterpret_cast<char *>(malloc(last_line_size));
  memcpy(buffer, data_ + size_ - last_line_size, last_line_size);

  buffer_size = last_line_size;

  // Reduce the size of this buffer
  size_ -= last_line_size;

  return 0;
}

/**
 * Read content of the buffer in a continuous way
 */

size_t Buffer::Read(char *output_buffer, size_t output_size) {
  size_t read_size = output_size;

  if (read_size > (size_ - offset_)) {
    read_size = (size_ - offset_);
  }
  memcpy(output_buffer, data_ + offset_, read_size);
  offset_ += read_size;
  return read_size;
}

/**
 * Auxilir functions to work directly with the content
 */

size_t Buffer::GetAvailableSizeToRead() const {
  return size_ - offset_;
}

size_t Buffer::GetAvailableSizeToWrite() const {
  return max_size_ - size_;
}

char *Buffer::data() {
  return data_;
}

size_t Buffer::max_size() const {
  return max_size_;
}

size_t Buffer::size() const {
  return size_;
}

void Buffer::set_size(size_t size) {
  if (size <= max_size_) {
    size_ = size;
  }
}

SimpleBuffer Buffer::GetSimpleBuffer() {
  return SimpleBuffer(data_, max_size_);
}

SimpleBuffer Buffer::GetSimpleBufferAtOffset(size_t offset) {
  return SimpleBuffer(data_ + offset, max_size_ - offset);
}

void Buffer::set_name(const std::string& name) {
  name_ = name;
}

// Get internal name for debuggin
std::string Buffer::name() const {
  return name_;
}

void Buffer::WriteFromFile(const std::string& file_name, au::ErrorManager& error) {
  size_t file_size = au::sizeOfFile(file_name);

  if (GetAvailableSizeToWrite() < file_size) {
    error.AddError(au::str("Not enougth space in buffer to read file %s (%s)"
                           , file_name.c_str(), au::str(file_size).c_str()));
    return;
  }

  FILE *file = fopen(file_name.c_str(), "r");
  if (!file) {
    error.AddError(au::str("No possible of open file %s", file_name.c_str()));
    return;
  }

  if (file_size > 0) {
    size_t n = fread(data_ + size_, file_size, 1, file);
    if (n != 1) {
      error.AddError("Error reading");
    } else {
      size_ += file_size;
    }
  }

  fclose(file);
}
}
