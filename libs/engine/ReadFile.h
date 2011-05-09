

#ifndef _H_ENGINE_READFILE
#define _H_ENGINE_READFILE

#include <string>           // std::string
#include <stdio.h>          // FILE*

namespace engine
{
    class ReadFile
    {
        std::string fileName;   // Filename
        FILE *file;             // File pointer to access this file
        
    public:
        
        ReadFile( std::string _fileName );

        int seek( size_t offset );
        
        int read( char * read_buffer , size_t size );
        
        bool isValid();
        
        void close();
        
    };

}

#endif