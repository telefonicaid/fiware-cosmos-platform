
#ifndef _H_SAMSON_DATA_SET
#define _H_SAMSON_DATA_SET

#include "au/map.h"                         // au::map
#include <string>                           // std::string
#include "samson/delilah/SamsonFile.h"      // samson::SamsonFile

namespace samson {
    
    class SamsonDataSet
    {
        
    public:
        
        // Common info
        KVFormat format;
        FullKVInfo info;
        
        au::map< std::string , SamsonFile > files;
        au::ErrorManager error;
        
        SamsonDataSet( std::string directory );
        ~SamsonDataSet();

        void printHashGroups();
        void printHeaders();
        void printContent( size_t limit );
        
        std::string str();
    };
}

#endif