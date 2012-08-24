
/* ****************************************************************************
*
* FILE            ReadFile
*
* AUTHOR          Andreu Urruela
*
* DATE            July 2011
*
* DESCRIPTION
*
* ReadFile describes an open file to be read
* It is managed by ReadFileManager inside DiskManager
*
* ****************************************************************************/

#ifndef _H_ENGINE_READFILE
#define _H_ENGINE_READFILE

#include <stdio.h>          // FILE*
#include <string>           // std::string


namespace engine {
class ReadFile {
public:

  ReadFile(const std::string& file_Name);
  ~ReadFile();

  int Seek(size_t offset);
  int Read(char *read_buffer, size_t size);
  bool IsValid() const;
  void Close();

  // Accessorts
  std::string file_name() const;
  size_t offset() const;

private:

  std::string file_name_;     // Filename
  size_t offset_;             // Offset positiong while reading the file
  FILE *file_;                // File pointer to access this file
};
}

#endif  // ifndef _H_ENGINE_READFILE
