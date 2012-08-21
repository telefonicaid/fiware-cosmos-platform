/* ****************************************************************************
 *
 * FILE                     Buffer.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */


#include <sstream>              // std::stringstream

#include "logMsg/logMsg.h"      // lmInit, LM_*
#include "logMsg/traceLevels.h"  // Trace Levels

#include "au/string.h"          // au::Format
#include "au/xml.h"             // au::xml...

#include "engine/Buffer.h"      // Own interface
#include "engine/MemoryManager.h"

namespace engine {
Buffer::Buffer(const std::string& name, const std::string& type,  size_t max_size) {
  LM_T(LmtBuffer, ("Creating %s", str().c_str()));

  name_ = name;
  type_ = type;

  size_ = 0;
  offset_ = 0;
  max_size_ = max_size;

  if (max_size > 0) {
    data_ = (char *)malloc(max_size);

    if (!data_)
      LM_X(1, ("Error (errno:%d) allocating memory for %d bytes for name:'%s' type:'%s'"
               , errno, max_size, name.c_str(), type.c_str()));
  } else {
    LM_W(("Buffer request of max_size(%lu) <= 0, for name:'%s' type:'%s'", max_size, name.c_str(), type.c_str()));
    data_ = NULL;
  }

  // Register in the memory manager to track allocated memory
  MemoryManager::shared()->Add(this);
}

au::SharedPointer<Buffer> Buffer::create(const std::string& name, const std::string& type, size_t max_size) {
  if (max_size > 1024 * 1024 * 1024)
    LM_X(1, ("Excesive size for buffer %s", au::str(max_size).c_str()));
  return au::SharedPointer<Buffer>(new Buffer(name, type, max_size));
}

Buffer::~Buffer() {
  // Unregister in the memory manager
  MemoryManager::shared()->Remove(this);

  // Free allocated data
  if (data_)
    ::free(data_);
}

std::string Buffer::str() {
  return au::str("[ Buffer (%s / %s) Size: %lu/%lu Read_offset %lu (%p) ]"
                 , name_.c_str(), type_.c_str(), size_, max_size_, size_, offset_, this);
}

/**
 * Function to write content updating the size variable coherently
 * If it is not possible to write the entire block, it return false
 * So, it never try to write less than input_size bytes
 */

bool Buffer::write(const char *input_buffer, size_t input_size) {
  if (size_ + input_size > max_size_)
    return false;

  memcpy(data_ + size_, input_buffer, input_size);
  size_ += input_size;

  return true;
}

bool Buffer::skipWrite(size_t size) {
  if (size_ + size <= max_size_) {
    size_ += size;
    return true;
  } else {
    return false;
  }
}

size_t Buffer::skipRead(size_t size) {
  if (offset_ + size > max_size_)
    size = (max_size_ - offset_);       // Max offset
  offset_ += size;
  return size;
}

/**
 * Write on the buffer the maximum possible ammount of data
 */

void Buffer::write(std::ifstream &inputStream) {
  inputStream.read(data_ + size_, max_size_ - size_);
  size_ += inputStream.gcount();
}

/*
 * Remove the last characters of an unfinished line and put them in buffer.
 * Remove the size of this set of characters
 */

int Buffer::removeLastUnfinishedLine(char *& buffer, size_t& buffer_size) {
  size_t last_line_size = 0;

  while (( last_line_size < getSize()) && (data_[size_ - last_line_size - 1] != '\n' )) {
    last_line_size++;
  }

  if (last_line_size == getSize())
    return 1;     // Error... not final line found in the buffer

  buffer = (char *)malloc(last_line_size);
  memcpy(buffer, data_ + size_ - last_line_size, last_line_size);

  buffer_size = last_line_size;

  // Reduce the size of this buffer
  size_ -= last_line_size;

  return 0;
}

/**
 * Read content of the buffer in a continuous way
 */

size_t Buffer::read(char *output_buffer, size_t output_size) {
  size_t read_size = output_size;

  if (read_size > ( size_ - offset_ ))
    read_size = (size_ - offset_); memcpy(output_buffer, data_ + offset_, read_size);
  offset_ += read_size;
  return read_size;
}

/**
 * Auxilir functions to work directly with the content
 */

size_t Buffer::getSizePendingRead() {
  return size_ - offset_;
}

size_t Buffer::getAvailableWrite() {
  return max_size_ - size_;
}

char *Buffer::getData() {
  return data_;
}

size_t Buffer::getMaxSize() {
  return max_size_;
}

size_t Buffer::getSize() {
  return size_;
}

void Buffer::setSize(size_t size) {
  if (size <= max_size_)
    size_ = size;
}

SimpleBuffer Buffer::getSimpleBuffer() {
  return SimpleBuffer(data_, max_size_);
}

SimpleBuffer Buffer::getSimpleBufferAtOffset(size_t offset) {
  return SimpleBuffer(data_ + offset, max_size_ - offset);
}

void Buffer::setNameAndType(std::string name, std::string type) {
  name_ = name;
  type_ = type;
}

void Buffer::addToName(std::string description) {
  name_.append(description);
}

// Get internal name for debuggin
std::string Buffer::getName() {
  return name_;
}

// Get internal type for debuggin
std::string Buffer::getType() {
  return type_;
}
}
