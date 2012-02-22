
#ifndef _H_SAMSON_FILE
#define _H_SAMSON_FILE

#include <string>       // std::string
#include <stdio.h>      // FILE*

#include "au/ErrorManager.h"            // au::ErrorManager

#include "samson/common/coding.h"       // KVHeader, KVInfo , ...

namespace samson {

    class SamsonFile
    {
        
        std::string fileName;
        au::ErrorManager error;
        FILE *file;
        
    public:
        
        samson::KVHeader header;
        samson::KVFormat format;
        samson::KVInfo*  info;
        
        SamsonFile( std::string _fileName );
        ~SamsonFile();
        
        bool hasError();
        
        std::string getErrorMessage();
        std::string getHashGroups();
        
        size_t printContent( size_t limit , std::ostream &output );
        std::string getContent( size_t limit , size_t *records );
        
    };

}

#endif