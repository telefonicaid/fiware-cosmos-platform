
#include "Rate.h"  // Own interface

namespace samson {
Rate::Rate() {
}

void Rate::push(size_t kvs, size_t size) {
  rate_kvs.Push(kvs);
  rate_size.Push(size);
}

void Rate::push(FullKVInfo info) {
  push(info.kvs, info.size);
}

size_t Rate::get_total_size() {
  return rate_size.size();
}

size_t Rate::get_total_kvs() {
  return rate_kvs.size();
}

double Rate::get_rate_size() {
  return rate_size.rate();
}

double Rate::get_rate_kvs() {
  return rate_kvs.rate();
}
}