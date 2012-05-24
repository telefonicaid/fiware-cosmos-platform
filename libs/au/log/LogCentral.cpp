
#include "LogCentral.h" // Own interface

namespace au 
{
    
    LogCentral::LogCentral( std::string _host , int _port , std::string _local_file ) 
    : token("PermanentSocketConnection")
    , token_plugins("token_plugins")
    {
        host = _host;
        port = _port;
        time_reconnect = 60; // Initial time to reconnect
        local_file = _local_file;
        
        // By default both connections are NULL
        socket_connection = NULL;
        local_file_descriptor = NULL;
        
        // Connect the first time
        try_connect();
    }
    
    LogCentral::~LogCentral()
    {
        
        // Close connections
        if( socket_connection ) 
        {
            socket_connection->close();
            delete socket_connection;
            socket_connection = NULL;
        }
        
        if( local_file_descriptor ) 
        {
            local_file_descriptor->close();
            delete local_file_descriptor;
            local_file_descriptor = NULL;
        }

    }
    
    void LogCentral::set_host_and_port( std::string _host , int _port )
    {
        au::TokenTaker tt(&token);

        // Set the new address
        host = _host;
        port = _port;

        // Close previous connections ( if any )
        if( socket_connection ) 
        {
            socket_connection->close();
            delete socket_connection;
            socket_connection = NULL;
        }
        
        // Reconnect to this server if possible
        time_reconnect = 60; // Initial time to reconnect
        try_connect();
        
    }
    
    int LogCentral::getFd()
    {
        au::TokenTaker tt(&token);
        
        if( socket_connection )
            return socket_connection->getFd();
        if( local_file_descriptor )
            return local_file_descriptor->getFd();
        
        return -1;
    }
    void LogCentral::write( Log *log )
    {
        au::TokenTaker tt(&token);
        
        write_to_plugins(log);
        write_to_server_or_file(log); // Log is accumulated and finally removed
    }
    
    void LogCentral::addPlugin( LogPlugin* p )
    {
        au::TokenTaker tt(&token_plugins);
        plugins.insert(p);
    }
    
    void LogCentral::removePlugin( LogPlugin* p )
    {
        au::TokenTaker tt(&token_plugins);
        plugins.erase(p);
    }
    
    void LogCentral::write_to_plugins( Log *log )
    {
        au::TokenTaker tt(&token_plugins);
        
        au::set<LogPlugin>::iterator it;
        for( it = plugins.begin() ; it != plugins.end() ; it++ )
            (*it)->process_log(log);
    }
    
    void LogCentral::write_to_server_or_file( Log *log )
    {
        // Check if the estabished connection should be canceled
        if( socket_connection && socket_connection->isDisconnected() )
        {
            delete socket_connection;
            socket_connection = NULL;
            time_since_last_try.reset();
        }
        
        // Reconnect to server if time is acceptable
        if( !socket_connection && ( time_since_last_try.diffTimeInSeconds() >= time_reconnect ) )
            try_connect();
                
        // General cronometer to never spend more than 1 second to send a trace...
        au::Cronometer c;

        // If socket connection
        if( socket_connection )
        {
            if( log->write( socket_connection ) )
                return;

            // It was not possible to sent to server, close and remove socket connection
            socket_connection->close();
            delete socket_connection;
            socket_connection = NULL;
            
            time_since_last_try.reset();
        }

        // Make sure, local file is ready to write
        open_local_file(); // Make sure, this is open
                
        if( !local_file_descriptor )
        {
            LM_LW(("Local file for logs not ready"));
            return;
        }
        
        if( !log->write(local_file_descriptor ) )
        {
            LM_LW(("Error writing log to local file"));
            return;
            
        }
    }
    
    void LogCentral::try_connect()
    {
        
        //LM_LV(("try to connect with log server..."));
        
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
    
    
    
    void LogCentral::open_local_file()
    {
        if( local_file_descriptor && local_file_descriptor->isDisconnected() )
        {
            delete local_file_descriptor;
            local_file_descriptor = NULL;
        }
        
        if( local_file_descriptor )
            return;
        
        // Name of the logs based on the pid of this process
        
        int fd = open( local_file.c_str() , O_WRONLY | O_CREAT , 0644 );
        LM_LT(LmtFileDescriptors, ("Open FileDescriptor fd:%d", fd));
        
        if( fd >= 0 )
        {
            local_file_descriptor = new FileDescriptor("local_log", fd );
            LM_LW(("Open local log file %s.",local_file.c_str() ));
        }
        else
            LM_LW(("Not possible to open local log file %s. Logs will be definitely lost", local_file.c_str() ));
    }
    
    
    
}