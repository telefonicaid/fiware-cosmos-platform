
#ifndef _H_LOG_SERVER_CHANNEL
#define _H_LOG_SERVER_CHANNEL

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

#include "au/log/TableLogFormatter.h"
#include "au/log/LogFile.h"
#include "au/log/Log.h"

namespace au 
{
    
    class LogServer;
    class LogServerQuery;
    
    class LogServerChannel : public network::Service
    {
        std::string directory;   // Directory to save data in
        
        au::Token token;         // Mutex protection ( multithread since we receive multiple connections )
        
        int file_counter;        // Used to write over a file
        size_t current_size;     // Current written size
        
        au::FileDescriptor *fd;  // Current file descriptor to save data
        
    public:

        au::rate::Rate rate;     // Estimated data rate for this channel

        // Constructor & destructor
        LogServerChannel( int port , std::string _directory );
        virtual ~LogServerChannel();
        
        // Init service 
        void initLogServerChannel( au::ErrorManager * error );

        // network::Service interface : main function for every active connection
        void run( au::SocketConnection * socket_connection , bool *quit );
        
        // Generic function to get a table of logs ( based on disk files )
        std::string  getTable( au::CommandLine * cmdLine );

        // Generic function to get a table of channels ( log connections )
        std::string  getChannelsTable( au::CommandLine * cmdLine );
        
        // Add a new session mark ( used in future queries )
        void addNewSession();
        
    private:
        
        // Open a new file descriptor ( disk ) to save received logs 
        void openFileDescriptor( au::ErrorManager * error );

        // Add a log to the channel
        void add( Log*log );
        
        // Get name for the counter-th log file
        std::string getFileNameForLogFile( int counter );

        
    };
}
#endif