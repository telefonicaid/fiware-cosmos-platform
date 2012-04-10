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
#include "au/Token.h"
#include "au/TokenTaker.h"

#include "au/log/Log.h"
#include "LogToServer.h" // Own interface

extern char            lsHost[64];
extern unsigned short  lsPort;

namespace au {
    
    
    class LogConnection
    {
        std::string host;
        int port;
        int time_reconnect;
        
        SocketConnection * socket_connection;
        au::Cronometer time_since_last_try;
        
        au::Token token;
        
    public:
        
        LogConnection( std::string _host , int _port ) : token("PermanentSocketConnection")
        {
            host = _host;
            port = _port;
            time_reconnect = 1; // Initial time to reconnect
            
            socket_connection = NULL;
            
            // Connect the first time
            try_connect();
        }
        
        void write( Log *log )
        {
            au::TokenTaker tt(&token);
            
            // Check if the estabished connection should be canceled
            check_disconnection();

            // Reconnect if time is acceptable
            if( !socket_connection && ( time_since_last_try.diffTimeInSeconds() >= time_reconnect ) )
                try_connect();
            
            // Write the lock
            if( socket_connection )
            {
                if( !log->write( socket_connection ) )
                {
                    // Close if not possible to write the log
                    socket_connection->close();
                    delete socket_connection;
                    socket_connection = NULL;
                    time_since_last_try.reset();
                }
            }
        }
        
    private:
        
        void check_disconnection()
        {
            if( socket_connection && socket_connection->isDisconnected() )
            {
                delete socket_connection;
                socket_connection = NULL;
                time_since_last_try.reset();
            }
            
        }
        
        void try_connect()
        {
            
            if( socket_connection )
                return ; // Already connected
            
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
        
    };
    
    au::LogConnection * log_connection = NULL;
    
    
    void start_log_to_server()
    {
        // Local verbose trace to inform about this start...
        LM_LV(("Logging to log server"));
        
        // Create the permanent connection ( reconnect if necessary )
        log_connection = new LogConnection( lsHost , lsPort ); // Reconnect if necessary...
        
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
        Log log; 
        
        // Add "string" fields
        if( progName )
            log.add_field("progName", progName);
        if( text )
            log.add_field("text", text);
        if ( file )
            log.add_field("file", file);
        if( fName )
            log.add_field("fName", fName);
        if( stre )
            log.add_field("stre", stre);
        
        
        log.log_data.lineNo      = lineNo;
        log.log_data.traceLevel  = tLev;
        log.log_data.type        = type;
        log.log_data.timezone    = timezone;
        log.log_data.dst         = dst;
        log.log_data.pid         = getpid();
        //log.log_data.tid         = gettid();
        log.log_data.tid         = 0; // Not possible to obtain this in mac OS. Pending to be fixed

        // Fill log_data.tv
        gettimeofday(&log.log_data.tv,NULL);
        
        // Write over the log_server_connection
        log_connection->write( &log );
        
    }
    
}
