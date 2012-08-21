

#include "ReadFile.h"       // Own interface
#include "logMsg/logMsg.h"         // LM_M ...

namespace engine {
ReadFile::ReadFile(std::string _fileName) {
  fileName = _fileName;
  file = fopen(fileName.c_str(), "r");
  offset = 0;
}

int ReadFile::seek(size_t _offset) {
  if (!file)
    return 1;

  if (offset == _offset)
    return 0;       // Correct... just do not move


#if 0
  // Get a warning to be aware of this seeks if it is too large
  if (llabs(_offset - offset) > 100000)
    LM_W(("Seeking file %s from %lu to %lu", fileName.c_str(), offset, _offset));

#endif

  // Set the current offset
  offset = _offset;

  if (fseek(file, offset, SEEK_SET) != 0) {
    close();
    return 3;
  }

  // Everything corrent
  return 0;
}

int ReadFile::read(char *read_buffer, size_t size) {
  if (size == 0)
    return 0;

  if (fread(read_buffer, size, 1, file) == 1) {
    offset += size;     // Move the offser according to the read bytes
    return 0;
  } else {
    return 1;       // Error at reading
  }
}

bool ReadFile::isValid() {
  return (file != NULL);
}

void ReadFile::close() {
  if (file) {
    fclose(file);
    file = NULL;
    offset = 0;
  }
}
}