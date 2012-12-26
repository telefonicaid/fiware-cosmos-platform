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
* FILE            file.h
*
* AUTHOR          Andreu Urruela
*
* PROJECT         au library
*
* DATE            September 2011
*
* DESCRIPTION
*
*      useful functions to work with files
*
* ****************************************************************************/

#ifndef _H_AU_FILE
#define _H_AU_FILE

#include "au/ErrorManager.h"   // au::ErrorManager
#include "au/containers/map.h"  // au::map
#include "logMsg/logMsg.h"     // LOG_SW
#include <sstream>             // std::ostringstream
#include <stdio.h>             /* sprintf */
#include <string>              // std::string
#include <sys/time.h>          // struct timeval

#include "Status.h"


namespace au {
// Get sie of a particular file ( 0 if error )
size_t sizeOfFile(std::string fileName);

// Remove a directory ( get a string error if something happens )
void RemoveDirectory(std::string fileName, au::ErrorManager & error);

// Check type of file
bool isDirectory(char path[]);
bool isRegularFile(char path[]);

// Get the list of regular files contained in a directory
std::vector<std::string> getRegularFilesFromDirectory(std::string directory);

// Create a random tmp direcotry
std::string GetRandomTmpFileOrDirectory();

// Working with paths
std::string path_remove_last_component(std::string path);
// Extract directory from path ( example /dir/dir2/file --> /dir/dir2 )
std::string get_directory_from_path(std::string path);

// Create directories
Status CreateDirectory(std::string path);
Status CreateFullDirectory(std::string path);
std::string GetCannonicalPath(const std::string& path);
std::string GetLastPathComponent(const std::string& path);

// Get a fill path from a directory and file name
std::string path_from_directory(const std::string& directory, const std::string& file);

// Get list of files from a file_name ( directory --> all included files )
std::vector<std::string> GetListOfFiles(const std::string file_name, au::ErrorManager& error);
}

#endif  // ifndef _H_AU_FILE
