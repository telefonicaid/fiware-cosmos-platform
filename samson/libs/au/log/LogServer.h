
#ifndef _H_LOG_SERVER
#define _H_LOG_SERVER

#include <string>
#include <sys/stat.h> // mkdir
#include <fcntl.h>

#include "logMsg/logMsg.h"

#include "au/network/FileDescriptor.h"
#include "au/network/Service.h"
#include "au/containers/map.h"
#include "au/containers/list.h"
#include "au/tables/Table.h"
#include "au/string/split.h"
#include "au/log/Log.h"
#include "au/network/ConsoleService.h"

namespace au {
    
    class LogServerChannel;
    
    class LogServer : au::network::ConsoleService
    {
        // Channel to accept connection with binary logs
        LogServerChannel* channel;
        
    public:
        
        // Constructor
        LogServer();
        
        // au::network::ConsoleService
        void runCommand( std::string command , au::Environment* environment ,au::ErrorManager* error );
        void autoComplete( ConsoleAutoComplete* info , au::Environment* environment );
        std::string getPrompt( au::Environment* environment );
        
        
    };
    
}
#endif