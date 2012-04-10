
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


namespace au {
    
    class LogServerQuery;
    class LogServerChannel;
    
    class LogServer
    {
        // Channels
        ::au::map<std::string,LogServerChannel> channels;
        
        // Query service
        LogServerQuery* log_server_query;
        
        // Mutex protection
        Token token;
        
    public:
        
        LogServer();
        
        // Add a service to attent queries of this port
        bool add_query_channel( int port );
        
        // Add a log channel on this port to receive connections with new logs
        void add_channel( std::string name , int port ,  std::string directory , ErrorManager *error  );
        
        // Get a log table following a particular protocol
        std::string getLogsTable( CommandLine * cmdLine );
        
        // Get table of current channels
        std::string getChannelsTables();
        
        // Get table with current connections ( for all channels and queries )
        std::string getConnectionsTables();
        
        
    };
    
}
#endif