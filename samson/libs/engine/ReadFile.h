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

#include <string>           // std::string
#include <stdio.h>          // FILE*
#include "au/namespace.h"

NAMESPACE_BEGIN(engine)

class ReadFile
{
    std::string fileName;   // Filename
    
    size_t offset;          // Offset positiong while reading the file
    
public:
    FILE *file;             // File pointer to access this file
    
    ReadFile( std::string _fileName );
    
    int seek( size_t offset );
    
    int read( char * read_buffer , size_t size );
    
    bool isValid();
    
    void close();
    
};

NAMESPACE_END

#endif
