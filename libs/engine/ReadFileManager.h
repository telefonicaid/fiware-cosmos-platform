

/* ****************************************************************************
 *
 * FILE            ReadFileManager
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * Collection of "ReadFile"s. All the already open files to be access in read mode.
 * It is an internal component of the DiskManager
 *
 * ****************************************************************************/
#ifndef _H_ENGINE_READ_FILE_MANAGER
#define _H_ENGINE_READ_FILE_MANAGER


#include "au/ListMap.h"     // au::ListMap
#include <string>           // std::string

namespace engine {
    
    
    class ReadFile;
    
    class ReadFileManager
    {
        au::ListMap<std::string, ReadFile> read_files;
        int max_open_files;
        
    public:
        
        ReadFileManager();
        ~ReadFileManager();
        
        ReadFile *getReadFile( std::string fileName );
        
    };

}

#endif