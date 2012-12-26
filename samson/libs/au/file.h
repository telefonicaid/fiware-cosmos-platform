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
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Usefull functions to work with files
 *
 * ****************************************************************************/

#ifndef _H_AU_FILE
#define _H_AU_FILE

#include <stdio.h>             /* sprintf */
#include <string>				// std::string
#include <sys/time.h>           // struct timeval
#include <sstream>              // std::ostringstream
#include "logMsg/logMsg.h"             // LM_W
#include "au/ErrorManager.h"        // au::ErrorManager
#include "au/containers/map.h"                 // au::map
#include "au/namespace.h"
#include "Status.h"


NAMESPACE_BEGIN(au)

// Get sie of a particular file ( 0 if error )
size_t sizeOfFile( std::string fileName );

// Remove a directory ( get a string error if something happens )
void removeDirectory( std::string fileName , au::ErrorManager & error ); 

// Check type of file
bool isDirectory(char path[]);
bool isRegularFile(char path[]);

// Get the list of regular files contained in a directory
std::vector<std::string> getRegularFilesFromDirectory( std::string directory );


// Working with paths
std::string path_remove_last_component( std::string path );
// Extract directory from path ( example /dir/dir2/file --> /dir/dir2 )
std::string get_directory_from_path( std::string path );

// Create directories
Status createDirectory( std::string path );
Status createFullDirectory( std::string path );

NAMESPACE_END

#endif
