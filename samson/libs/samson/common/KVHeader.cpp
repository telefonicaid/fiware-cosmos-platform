
#include "au/string.h"
#include "au/xml.h"

#include "KVHeader.h"  // Own interface

namespace samson {
void KVHeader::init(KVFormat format, KVInfo _info) {            // Complete init function
  magic_number =  4652783;

  setFormat(format);
  setInfo(_info);

  // Default initialization of the hash-group to full-files
  range.set(0, KVFILE_NUM_HASHGROUPS);

  // Current time-stamp
  time = ::time(NULL);
}

void KVHeader::initForTxt(size_t size) {
  magic_number =  4652783;
  setFormat(KVFormat("txt", "txt"));

  // Random hash-group
  int hg = rand() % KVFILE_NUM_HASHGROUPS;
  range.set(hg, hg + 1);

  info.kvs = 0;  // In txt data-sets there are not "key-values"
  info.size = size;

  // Current time-stamp
  time = ::time(NULL);
}

bool KVHeader::isTxt() {
  return getKVFormat().isEqual(KVFormat("txt", "txt"));
}

bool KVHeader::check_size(size_t total_size) {
  return (total_size == ( sizeof(KVHeader) + info.size ));
}

void KVHeader::setHashGroups(uint32 _hg_begin, uint32 _hg_end) {
  range.set(_hg_begin, _hg_end);
}

void KVHeader::setFormat(KVFormat format) {
  snprintf(keyFormat, 100, "%s", format.keyFormat.c_str());
  snprintf(valueFormat, 100, "%s", format.valueFormat.c_str());
}

// Functions to set of get information from the header
// ---------------------------------------------------------------

void KVHeader::setInfo(KVInfo _info) {
  info = _info;
}

uint32 KVHeader::getTotalSize() {
  // Get the total size of the message including header, hash-group info and data
  return sizeof(KVHeader) + sizeof(KVInfo) * getNumHashGroups() + info.size;
}

uint32 KVHeader::getNumHashGroups() {
  return range.getNumHashGroups();
}

// Format operations
// ---------------------------------------------------------------

KVFormat KVHeader::getKVFormat() {
  return KVFormat(keyFormat, valueFormat);
}

// Check operations ( magic number and other conditions )
// ---------------------------------------------------------------

bool KVHeader::check() {
  return ( magic_number == 4652783);
}

std::string KVHeader::str() {
  std::ostringstream output;

  output << "KVHeader: " << info.str() << "(" << range.str() << ") (" << getKVFormat().str() << ")";
  return output.str();
}

void KVHeader::getInfo(std::ostringstream &output) {
  au::xml_open(output, "kv_header");

  getKVFormat().getInfo(output);
  info.getInfo(output);
  range.getInfo(output);

  au::xml_close(output, "kv_header");
}
}