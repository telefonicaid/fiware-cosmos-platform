#ifndef _H_SAMSON_KVFILE
#define _H_SAMSON_KVFILE

#include "au/ErrorManager.h"

#include "samson/common/KVHeader.h"
#include "samson/common/KVInfo.h"


namespace samson 
{
    /**
     Structure used to work with contents of KVFiles
     **/
    
    class KVFile
    {
        KVHeader *header;
        
        KVInfo *info;      // Vector with all KVInfos for each hash-group
        size_t *offsets;   // Vector containing offsets to each hash-group
        
        char *data;
        
        au::ErrorManager error;
        
    public:
        
        KVFile( char *_data );
        ~KVFile();
        
        char * dataForHashGroup( int hg );
        KVInfo getKVInfoForHashGroup( int hg );
        KVHeader* getKVHeader();
        
        bool isErrorActivated()
        {
            return error.isActivated();
        }
        
        std::string getErrorMessage()
        {
            return error.getMessage();
        }
        
    private:
        
        size_t offset( int hg );
        
        
    };        
}

#endif
