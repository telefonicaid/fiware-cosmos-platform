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

/*
 * FILE            file
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            August 2012
 *
 * DESCRIPTION
 *
 *  File manipulation methods
 *
 */

#include <dirent.h>
#include <sstream>          // std::ostringstream
#include <stdarg.h>         /* va_start, va_arg, va_end                  */
#include <string.h>         // strchr
#include <string>
#include <sys/stat.h>   // stat(.)
#include <unistd.h>         /* rmdir() */
#include <vector>

#include "au/file.h"        // Own interface
#include "au/log/LogMain.h"
#include "au/string/StringUtilities.h"

namespace au {
size_t sizeOfFile(std::string fileName) {
  struct ::stat info;

  if (stat(fileName.c_str(), &info) == 0) {
    return info.st_size;
  } else {
    return 0;
  }
}

bool isDirectory(std::string fileName) {
  struct stat buf;

  stat(fileName.c_str(), &buf);
  return S_ISDIR(buf.st_mode);
}

bool isRegularFile(std::string fileName) {
  struct stat buf;

  stat(fileName.c_str(), &buf);
  return S_ISREG(buf.st_mode);
}

void RemoveDirectory(std::string fileName, au::ErrorManager & error) {
  if (isRegularFile(fileName)) {
    // Just remove
    int s = remove(fileName.c_str());
    if (s != 0) {
      error.AddError("Not possible to remove file " + fileName);
    }
    return;
  }

  // Navigate in the directory
  if (!isDirectory(fileName)) {
    error.AddError(au::str("%s is not a directory or a file", fileName.c_str()));
    return;
  }

  // first off, we need to create a pointer to a directory
  DIR *pdir = opendir(fileName.c_str());      // "." will refer to the current directory
  struct dirent *pent = NULL;
  if (pdir != NULL) {    // if pdir wasn't initialised correctly
    while ((pent = readdir(pdir))) {      // while there is still something in the directory to list
      if (pent != NULL) {
        if (strcmp(".", pent->d_name) == 0) {
          continue;
        }
        if (strcmp("..", pent->d_name) == 0) {
          continue;
        }
        std::ostringstream localFileName;
        localFileName << fileName << "/" << pent->d_name;

        RemoveDirectory(localFileName.str(), error);
      }
    }
    // finally, let's close the directory
    closedir(pdir);
  }

  // Remove the directory properly
  int s = rmdir(fileName.c_str());
  if (s != 0) {
    error.AddError(au::str("Not possible to remove directory %s", fileName.c_str()));
  }
}

std::string path_remove_last_component(std::string path) {
  size_t pos = path.find_last_of('/');

  if (pos == std::string::npos) {
    return "";
  } else {
    if (pos == 0) {
      return "/";
    }

    return path.substr(0, pos);
  }
}

std::vector<std::string> getRegularFilesFromDirectory(std::string directory) {
  std::vector<std::string> files;

  // Add all plain files in this directory
  DIR *pdir = opendir(directory.c_str());      // "." will refer to the current directory
  struct dirent *pent = NULL;
  if (pdir != NULL) {    // if pdir wasn't initialised correctly
    while ((pent = readdir(pdir))) {      // while there is still something in the directory to list
      if (pent != NULL) {
        if (strcmp(".", pent->d_name) == 0) {
          continue;
        }
        if (strcmp("..", pent->d_name) == 0) {
          continue;
        }
        std::ostringstream localFileName;
        localFileName << directory << "/" << pent->d_name;

        // Add to the list of files
        files.push_back(localFileName.str());
      }
    }
    // finally, let's close the directory
    closedir(pdir);
  }

  return files;
}

std::string get_directory_from_path(std::string path) {
  if ((path == "") || (path == ".") || (path == "./")) {
    return "./";
  }

  if (path == "/") {
    return path;
  }

  // Remove the last part of the path
  size_t pos = path.find_last_of('/');

  if (path[0] == '/') {
    if (pos != 0) {
      return path.substr(0, pos + 1);
    } else {
      return "/";
    }
  }

  if (path[0] == '.') {
    if (pos == std::string::npos) {
      return "./";
    } else {
      return path.substr(0, pos + 1);
    }
  }

  if (pos == std::string::npos) {
    return "./";
  } else {
    return "./" + path.substr(0, pos + 1);
  }
}

Status CreateDirectory(std::string path) {
  if (mkdir(path.c_str(), 0755) == -1) {
    if (errno != EEXIST) {
      LOG_SW(("Error creating directory %s (%s)", path.c_str(), strerror(errno)));
      return Error;
    }
  }
  return OK;
}

Status CreateFullDirectory(std::string path) {
  if (path.length() == 0) {
    return au::Error;
  }

  std::vector<std::string> components;
  au::split(path, '/', components);

  std::string accumulated_path;
  if (path[0] == '/') {
    accumulated_path += "/";
  }
  for (size_t i = 0; i < components.size(); i++) {
    accumulated_path += components[i];
    Status s = CreateDirectory(accumulated_path);
    if (s != OK) {
      LOG_SW(("Error creating directory %s (%s)", accumulated_path.c_str(), status(s)));
      return s;
    }

    accumulated_path += "/";
  }

  return OK;
}

std::string GetCannonicalPath(const std::string& path) {
  size_t pos = path.size() - 1;

  while ((pos > 0) && path[pos] == '/') {
    pos--;
  }
  return path.substr(0, pos + 1);
}

std::vector<std::string> GetListOfFiles(const std::string file_name, au::ErrorManager& error) {
  std::vector<std::string> file_names;

  struct stat buf;
  int rc = stat(file_name.c_str(), &buf);

  if (rc) {
    error.AddError(au::str("%s is not a valid local file or dir ", file_name.c_str()));
    return file_names;
  }

  if (S_ISREG(buf.st_mode)) {
    file_names.push_back(file_name);
  } else if (S_ISDIR(buf.st_mode)) {
    {
      // first off, we need to create a pointer to a directory
      DIR *pdir = opendir(file_name.c_str());      // "." will refer to the current directory
      struct dirent *pent = NULL;
      if (pdir != NULL) {    // if pdir wasn't initialised correctly
        while ((pent = readdir(pdir))) {    // while there is still something in the directory to list
          if (pent != NULL) {
            std::ostringstream localFileName;
            localFileName << file_name << "/" << pent->d_name;

            struct stat buf2;
            stat(localFileName.str().c_str(), &buf2);

            if (S_ISREG(buf2.st_mode)) {
              file_names.push_back(localFileName.str());
            }
          }
        }
        // finally, let's close the directory
        closedir(pdir);
      }
    }
  } else {
    error.AddError(au::str("%s is not a valid local file or dir ", file_name.c_str()));
  }

  return file_names;
}

std::string path_from_directory(const std::string& directory, const std::string& file) {
  if (directory.length() == 0) {
    return file;
  }

  if (directory[ directory.length() - 1 ] == '/') {
    return directory + file;
  } else {
    return directory + "/" + file;
  }
}

std::string GetRandomTmpFileOrDirectory() {
  char tmp_directory[100];

  sprintf(tmp_directory, "/tmp/tmpXXXXXXX");
  return mktemp(tmp_directory);
}
}
