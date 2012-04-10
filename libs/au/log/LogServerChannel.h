
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
    
    // Service to add logs
    class LogServerChannel : public network::Service
    {
        std::string name;
        std::string directory;
        
        
        au::Token token;         // Mutex protection ( multithread since we receive multiple connections )
        int file_counter;        // Used to write over a file
        size_t current_size;     // Current written size
        au::FileDescriptor *fd;
        
    public:
        
        au::rate::Rate rate;

        // Constructor & destructor
        LogServerChannel( std::string _name , int port , std::string _directory );
        virtual ~LogServerChannel();
        
        // Get the name of the channel        
        std::string getName();
        

        // Funciton to init
        void initLogServerChannel( au::ErrorManager * error );

        // network::Service interface : main function for every active connection
        void run( au::SocketConnection * socket_connection , bool *quit );
        
        // Generic function to get a table of logs ( based on disk files )
        std::string  getTable( au::CommandLine * cmdLine );
        
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