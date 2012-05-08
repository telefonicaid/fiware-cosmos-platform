#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <netinet/in.h>         // struct sockaddr_in
#include <netdb.h>              // gethostbyname
#include <arpa/inet.h>          // inet_ntoa
#include <netinet/tcp.h>        // TCP_NODELAY
#include <signal.h>
#include <unistd.h>

#include "parseArgs/parseArgs.h"
#include "parseArgs/paBuiltin.h"
#include "parseArgs/paIsSet.h"
#include "parseArgs/paConfig.h"
#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "au/network/SocketConnection.h"
#include "au/string.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/containers/list.h"

#include "au/log/Log.h"
#include "LogToServer.h" // Own interface

extern char            lsHost[64];
extern unsigned short  lsPort;

namespace au {
    
    
    class LogConnection
    {
        // Connection information
        std::string host;
        int port;
        int time_reconnect;
        std::string local_file; // Name of the local file ( if not possible to connect with server )
        
        // List of logs to be sent to the server when connected
        au::list<Log> logs;

        // Socket connection with the logServer
        SocketConnection * socket_connection;
        
        // Local file descriptor to write the log if not possible to connect
        FileDescriptor* local_file_descriptor; 
        
        // Cronometer with the time since last connection
        au::Cronometer time_since_last_try;

        // Mutex to protect socket connection
        au::Token token;
        
    public:
        
        LogConnection( std::string _host , int _port , std::string _local_file ) : token("PermanentSocketConnection")
        {
            host = _host;
            port = _port;
            time_reconnect = 1; // Initial time to reconnect
            local_file = _local_file;
            
            // By default both connections are NULL
            socket_connection = NULL;
            local_file_descriptor = NULL;
            
            // Connect the first time
            try_connect();
        }
        
        int getFd()
        {
            if( socket_connection )
                return socket_connection->getFd();
            if( local_file_descriptor )
                return local_file_descriptor->getFd();
            
            return -1;
        }
        
        void write( Log *log )
        {
            
            au::TokenTaker tt(&token);
            LM_LV(("Writing log..."));
            
            // Check if the estabished connection should be canceled
            check_disconnection();

            // Reconnect if time is acceptable
            if( !socket_connection && ( time_since_last_try.diffTimeInSeconds() >= time_reconnect ) )
                try_connect();
            
            
            // Write the lock
            if( socket_connection )
            {
                // Push log into the queue to be sent
                logs.push_back(log);

                // Keep a maximum of 100K logs
                // Limit the number of logs to be in memory
                while( logs.size() > 100000 )
                    delete logs.extractFront();

                au::Cronometer c;
                while( logs.size() > 0 )
                {
                    if( c.diffTimeInSeconds() > 1 )
                        return; // Never spent more than one second sending traces...
                    
                    Log* tmp_log = logs.extractFront();
                    if( !tmp_log->write( socket_connection ) )
                    {
                        // Puhs back into the queue
                        logs.push_front(tmp_log);
                        
                        // Close if not possible to write the log
                        socket_connection->close();
                        delete socket_connection;
                        socket_connection = NULL;
                        time_since_last_try.reset();
                        return;
                    }
                    
                    delete tmp_log;
                    
                }
                
            }
            else
            {
                // write the log to a local file....
                open_local_file();

                if( local_file_descriptor )
                    if( !log->write(local_file_descriptor ) )
                    {
                        delete local_file_descriptor;
                        local_file_descriptor = NULL;
                    }
                
            }
        }
        
    private:
        
        void check_disconnection()
        {
            LM_LV(("Check if we are still connected with log server..."));

            if( socket_connection && socket_connection->isDisconnected() )
            {
                delete socket_connection;
                socket_connection = NULL;
                time_since_last_try.reset();
            }
            
        }
        
        void try_connect()
        {
            
            LM_LV(("try to connect with log server..."));
            
            if( socket_connection )
                return ; // Already connected

            if( ( host == "" ) || ( host == "NO" ) || ( host == "no" ) )
                return; // Not try to connect...
            
            // Try new connection
            au::Status s = au::SocketConnection::newSocketConnection( host , port , &socket_connection );
            
            if( s!= au::OK )
            {
                time_reconnect *= 2;
                
                size_t time =  time_since_last_try.diffTimeInSeconds();
                if( time < (size_t) time_reconnect )
                {
                    size_t next_time = time_reconnect - time;
                    LM_LW(("Not possible to connect with log server %s:%d (%s). Next try in %s" 
                           , host.c_str() , port, au::status(s) , au::str_time(next_time).c_str() ));
                }
                else
                    LM_LW(("Not possible to connect with log server %s:%d (%s)." 
                           , host.c_str() , port, au::status(s) ));
                    
            }
            else
            {
                size_t time = time_since_last_try.diffTimeInSeconds();
                if( time  > 10 )
                    LM_LW(("Connected to log server after %s disconnected" , au::str_time(time).c_str() ));
                time_reconnect = 1;
            }
        }
        
        void open_local_file()
        {

            if( local_file_descriptor )
                return;
            
            // Name of the logs based on the pid of this process
            std::string tmp_local_file = au::str("%s_%d" , local_file.c_str() , (int) getpid() );

            int fd = open( tmp_local_file.c_str() , O_WRONLY | O_CREAT , 0644 );
            LM_T(LmtFileDescriptors, ("Open FileDescriptor fd:%d", fd));
            
            if( fd >= 0 )
            {
                local_file_descriptor = new FileDescriptor("local_log", fd );
                LM_LW(("Open local log file %s.",tmp_local_file.c_str() ));
            }
            else
                LM_LW(("Not possible to open local log file %s. Logs will be definitely lost",tmp_local_file.c_str() ));
        }
    };
    
    au::LogConnection * log_connection = NULL;
    
    
    int getLogServerConnectionFd()
    {
        if( log_connection )
            return log_connection->getFd();
        return -1;
    }
    
    void start_log_to_server( std::string log_host , int log_port , std::string local_log_file )
    {
        // Local verbose trace to inform about this start...
        LM_LV(("Start log mechanism with host %s:%d ( local file %s )" ,
               log_host.c_str(),
               log_port,
               local_log_file.c_str() ));
        
        // Create the permanent connection ( reconnect if necessary )
        log_connection = new LogConnection( log_host , log_port , local_log_file  ); 
        
        // Set this function as the hook function of the log library
        lmOutHookSet(logToLogServer, NULL);
        
    }
    
    /* ****************************************************************************
     *
     * logToLogServer - 
     */
    
    void logToLogServer(void* vP, char* text, char type, time_t secondsNow, int timezone, int dst, const char* file, int lineNo, const char* fName, int tLev, const char* stre )
    {
        
        // Create the log to be sent
        Log* log = new Log();
        
        // Add "string" fields
        if( progName )
            log->add_field("progName", progName);
        if( text )
            log->add_field("text", text);
        if ( file )
            log->add_field("file", file);
        if( fName )
            log->add_field("fName", fName);
        if( stre )
            log->add_field("stre", stre);
        
        
        log->log_data.lineNo      = lineNo;
        log->log_data.traceLevel  = tLev;
        log->log_data.type        = type;
        log->log_data.timezone    = timezone;
        log->log_data.dst         = dst;
        log->log_data.pid         = getpid();
        //log.log_data.tid         = gettid();
        log->log_data.tid         = 0; // Not possible to obtain this in mac OS. Pending to be fixed

        // Fill log_data.tv
        gettimeofday(&log->log_data.tv,NULL);
        
        // Write over the log_server_connection
        log_connection->write( log );
        
    }
    
}
