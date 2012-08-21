
#include "logMsg/logMsg.h"

#include "StringCollection.h"  // Own interface

namespace au {
StringCollection::StringCollection() {
  v = (char *)malloc(1);
  size = 0;
  max_size = 1;
}

StringCollection::~StringCollection() {
  if (v)
    free(v);
}

int StringCollection::add(const char *string) {
  // --------------------------------------------------------
  // Check previously introduced the same string
  // --------------------------------------------------------
  std::string _string = string;

  std::map<std::string, int>::iterator it = previous_strings.find(_string);
  if (it != previous_strings.end())
    return it->second;  // --------------------------------------------------------

  size_t len = strlen(string);
  size_t required_size = size + len + 1;

  // Alloc more space if necessary
  if (required_size > max_size) {
    size_t future_max_size = max_size;
    while (future_max_size < required_size) {
      future_max_size *= 2;
    }

    char *vv = (char *)malloc(future_max_size);
    memcpy(vv, v, size);

    free(v);
    v = vv;
    max_size = future_max_size;
  }

  int pos = (int)size;

  memcpy(v + size, string, len + 1);
  size += (len + 1);


  // Add in the map of preivous string
  previous_strings.insert(std::pair<std::string, int>(_string, pos));

  return pos;
}

size_t StringCollection::write(FILE *file) {
  size_t t = fwrite(v, size, 1, file);

  if (t == 1)
    return size; else
    return 0;
}

void StringCollection::read(FILE *file, size_t _size) {
  // Free preivous buffer if any
  if (v)
    free(v); v = (char *)malloc(_size);
  size = _size;
  max_size = _size;

  // Read content from file
  size_t r = fread(v, _size, 1, file);
  if (r != 1)
    LM_W(("Error reading StringCollection"));
}

size_t StringCollection::getSize() {
  return size;
}

const char *StringCollection::get(int pos) {
  return &v[pos];
}
}