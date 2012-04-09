
#ifndef _H_SAMSON_DATA_SET
#define _H_SAMSON_DATA_SET

#include <string>                           // std::string

#include "au/containers/map.h"                         // au::map

#include "samson/common/FullKVInfo.h"

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

        void printHashGroups( std::ostream &output);
        void printHeaders( std::ostream &output);
        void printContent( size_t limit , std::ostream &output );
        
        std::string strSize();
        std::string strFormat();
    };
}

#endif