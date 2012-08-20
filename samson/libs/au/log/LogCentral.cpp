
#include "LogCentral.h" // Own interface

namespace au 
{
    
    LogCentral::LogCentral( std::string _host , int _port , std::string _local_file ) 
    : token("PermanentSocketConnection")
    , token_plugins("token_plugins")
    , token_current_thread("token_current_thread")
    {
        host = _host;
        port = _port;
        local_file = _local_file;
        
        // By default both connections are NULL
        socket_connection = NULL;
        local_file_descriptor = NULL;

        // Connect the next time a trace is sent
        time_reconnect = 0;

        // No current thread by defautl
        current_thread_activated = false;
        
        // Direct mode flag
        direct_mode = false;
        
        // Not using any fd here
        fd = -1;
    }
    
    LogCentral::~LogCentral()
    {
        close_socket_connection();
        close_local_file();
    }

    void LogCentral::close_socket_connection()
    {
        if( socket_connection ) 
        {
            socket_connection->Close();
            delete socket_connection;
            socket_connection = NULL;
            
            // This is the time counter since last connection
            time_since_last_connection.Reset();
            
            // Force try reconnect in next log
            time_reconnect = 0; 
        }
    }
    
    void LogCentral::close_local_file()
    {
        if( local_file_descriptor ) 
        {
            local_file_descriptor->Close();
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
        close_socket_connection();
        close_local_file();
        
        // Connect the next time a trace is sent
        time_reconnect = 0; 
    }
    
    void LogCentral::write( Log* log )
    {
        // In direct mode, we just try to send traces ( not reconnection, no blocking )
        if( direct_mode )
        {
            if( socket_connection )
                log->write( socket_connection );
            else if( local_file_descriptor )
                log->write(local_file_descriptor );

            return;
        }
        
        // If we are the writing thread, just return ( otherwise it will be locked )
        // If the system is taken, just wait
        while( true )
        {
            {
                // Mutex protection
                au::TokenTaker tt(&token_current_thread);
                
                // Get my thread id
                pthread_t my_thread_id = pthread_self();
                
                // Check if I am blocking this log
                if( current_thread_activated )
                {
                    if( current_thread == my_thread_id )
                        return; // Secondary log...
                }
                else
                {
                    current_thread = my_thread_id;
                    current_thread_activated = true;
                    break;
                }
            }
            
            // Sleep waiting to send traces
            usleep(10000);
        }
        
        // Real write operation
        write_to_plugins(log);
        write_to_server_or_file(log); // Log is accumulated and finally removed
        
        {
            // Mutex protection
            au::TokenTaker tt(&token_current_thread);

            // Deactivate this lock
            current_thread_activated = false;
        }
        
        
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
        au::TokenTaker tt(&token);
        
        // In direct mode, we do not reconnect
        
        // Check if the estabished connection should be canceled
        if( socket_connection && socket_connection->IsClosed() )
        {
            close_socket_connection();
            fd = -1;
        }
        
        // Reconnect to server if necessary
        size_t time = time_since_last_connection.seconds();
        if( !socket_connection && ( host != "" )  && ( time >= time_reconnect ) )
        {
            // Try new connection with the server
            au::Status s = au::SocketConnection::Create( host , port , &socket_connection );
            
            if( s != au::OK )
            {
                // Set the next time_reconnect
                if( time == 0 )
                    time_reconnect = 120;
                
                if( time_reconnect < time )
                    time_reconnect = time;
                
                time_reconnect *= 2; // Increase time to reconnect
                
                size_t next_try_time = time_reconnect - time;
                
                LM_W(("Not possible to connect with log server %s:%d (%s). Next try in %s" 
                      , host.c_str() , port, au::status(s) , au::str_time(next_try_time).c_str() ));
            }
            else
            {
                // Set the used fd
                fd = socket_connection->fd();
                
                if( time  > 10 )
                    LM_LW(("Connected to log server after %s disconnected" , au::str_time(time).c_str() ));
            }            
        }
        
        
        // Try socket first...
        if( socket_connection )
        {
            if( log->write( socket_connection ) )
                return;

            // It was not possible to sent to server, close and remove socket connection
            close_socket_connection();
            fd = -1;
        }

        // Make sure, local file is ready to write
        
        if( !local_file_descriptor )
        {
            fd = open( local_file.c_str() , O_WRONLY | O_CREAT , 0644 );
            LM_LT(LmtFileDescriptors, ("Open FileDescriptor fd:%d", fd));
        
            if( fd >= 0 )
            {
                local_file_descriptor = new FileDescriptor("local_log", fd );
            }
            else
            {
                LM_LW(("Not possible to open local log file %s. Logs will be definitely lost", local_file.c_str() ));
                return;
            }
        }
        
        if( !log->write(local_file_descriptor ) )
        {
            close_local_file();
            return;
        }
        
    }
    
    
    void LogCentral::set_direct_mode( bool flag)
    {
        direct_mode = flag;
    }

    
}
