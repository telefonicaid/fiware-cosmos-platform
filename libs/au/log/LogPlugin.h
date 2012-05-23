

#ifndef _H_AU_LOG_PLUGIN
#define _H_AU_LOG_PLUGIN

#include "au/log/Log.h"

namespace au
{
    
    class LogPlugin
    {
        
        // Pending filter setup...
        
    public:
        
        void virtual process_log( Log* log )=0;
        
    };
    
}

#endif