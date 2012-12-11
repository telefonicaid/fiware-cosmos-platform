
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

#ifndef _H_AU_FILE_DESCRIPTOR
#define _H_AU_FILE_DESCRIPTOR

#include "au/Status.h"
#include "au/ThreadManager.h"
#include "au/mutex/Token.h"
#include "au/statistics/Rate.h"
#include "au/string/StringUtilities.h"

/*****************************************************************************
*
*  FileDescriptor
*
*  Handy class to work with a single file-descriptor
*  It offers mutex protection agains close operation and some useful
*  read/write methods
*
*  Note: Use only LM_LM or LM_LW in this class since it is used in hoock function
*
*****************************************************************************/

namespace au {
class FileDescriptor {
public:

  FileDescriptor(const std::string& name, int fd);
  ~FileDescriptor();

  // Accessors
  int fd() const;
  std::string name() const;
  void set_name(const std::string& name);
  au::Rate& rate_in();
  au::Rate& rate_out();

  // Connection management
  void Close();               // Close this file descriptor
  bool IsClosed()  const;     // Check if it is closed

  // Read & Write line
  Status ReadLine(char *line, size_t max_size, int max_seconds = 1);
  Status WriteLine(const char *line, int retries = 1, int tv_sec = 0,
                   int tv_usec = 100);

  // Read buffer
  Status readBuffer(char *line, size_t max_size, int tmoSecs);

  // Auxiliar function to read and write to the socket
  Status okToSend(int tries, int tv_sec, int tv_usec);
  Status msgAwait(int secs, int usecs, const char *what);

  // Generic function to read or write
  Status partWrite(const void *dataP, int dataLen, const char *what,
                   int retries = 300, int tv_sec = 1,
                   int tv_usec = 0);
  Status partRead(void *vbuf, size_t bufLen, const char *what, int max_seconds,
                  size_t *readed_size = NULL);

  // Debug string
  std::string str();

private:

  au::Token token_;        // Token to protect multiple closes agains the same fd
  std::string name_;       // name for debuging
  int fd_;                 // If id is -1 it means it was closed for some reason

  // Statistics about input output rate
  au::Rate rate_in_;
  au::Rate rate_out_;
};
}

#endif  // ifndef _H_AU_FILE_DESCRIPTOR
