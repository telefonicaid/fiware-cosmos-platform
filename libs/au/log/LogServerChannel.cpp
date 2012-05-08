#include <time.h>
#include "LogServerChannel.h" // Own interface
#define Char_to_int(x) ((x)-48)

#include "logMsg/logMsg.h"                                              // LM_T
#include "logMsg/traceLevels.h"            // LmtFileDescriptors, etc.

namespace au 
{
    LogServerChannel::LogServerChannel( std::string _name , int port , std::string _directory ) 
    : network::Service( port ) , token("LogServerChannel")
    {
        name = _name;
        directory = _directory;
        
    }
    
    LogServerChannel::~LogServerChannel()
    {
        
    }
    
    std::string LogServerChannel::getFileNameForLogFile( int counter )
    {
        return au::str("%s/log_%d" , directory.c_str() , counter );
    }
    
    void LogServerChannel::openFileDescriptor( au::ErrorManager * error )
    {
        
        // Check if disconnected
        if ( fd )
            if( fd->isDisconnected() )
            {
                delete fd;
                fd = NULL;
            }
        
        if( !fd )
        {
            std::string current_file_name;
            
            while( true )
            {
                // Get the name of the log file
                current_file_name = getFileNameForLogFile( file_counter++ );
                
                struct stat buf;
                int s = stat( current_file_name.c_str() , &buf );
                if( s == 0 ) 
                    continue; // File exist, so let's try the next number...
                
                LM_V(("Opening log file %s" , current_file_name.c_str()));
                int _fd = open( current_file_name.c_str() , O_CREAT | O_WRONLY , 0644 );
                LM_T(LmtFileDescriptors, ("Open FileDescriptor fd:%d", fd));
                
                if( _fd < 0 )
                {
                    error->set( au::str("Error opening file %s (%s)" , current_file_name.c_str() , strerror(errno) ));
                    return;
                }
                
                // Create a file descriptor
                fd = new au::FileDescriptor( "log fd" , _fd );
                
                // Init byte counter for the current file
                current_size = 0;
                
                // File created!
                return;
            }
        }
    }
    
    void LogServerChannel::initLogServerChannel( au::ErrorManager * error )
    {
        // Create directory
        if( ( mkdir( directory.c_str() , 0755 ) != 0 ) && ( errno != EEXIST ) )
        {
            error->set(au::str("Error creating directory %s (%s)" , directory.c_str() , strerror( errno ) ));
            return;
        }
        
        // Open the first file-descriptor
        openFileDescriptor( error );
        
        if( error->isActivated() )
            return;
        
        au::Status s = initService();
        if( s!= au::OK )
        {
            error->set( au::str("Error initializing server (%s)" , au::status(s) ) );
            return;
        }
        
    }
    
    void LogServerChannel::run( au::SocketConnection * socket_connection , bool *quit )
    {
        LM_V(("New connection from %s" , socket_connection->getHostAndPort().c_str() ));
        
        while( !*quit )
        {
            // Rean a log
            Log *log = new Log( );
            log->add_field("host", socket_connection->getHostAndPort() );
            
            if( !log->read( socket_connection ) )
            {
                return; // Not possible to read a log...
                
                LM_V(("Closed connection from %s" , socket_connection->getHostAndPort().c_str() ));
            }
            
            // Add log...
            add( log );
            
        }
    }
    
    
    void LogServerChannel::add( Log*log )
    {
        // Mutex protection
        au::TokenTaker tt(&token); 
        
        LM_V(("Received log: %s" , log->str().c_str() ));
        
        // Write to file
        if( fd )
            log->write( fd );
        
        // Monitorize rate of logs
        rate.push( log->getTotalSerialitzationSize() );
        
    }
    
    std::string LogServerChannel::getName()
    {
        return name;
    }
    
    

    
    void LogServerChannel::addNewSession()
    {
        Log log;
        log.set_new_session();
        
        // Add is protected with the mutex
        add(&log);
    }
    
    
    std::string LogServerChannel::getTable( au::CommandLine * cmdLine )
    {
        LM_V(("Get table..."));
        
        // Mutex protection ( not necessary any more since it is based on files )
        //au::TokenTaker tt(&token); 
        
        // Get formats from 
        std::string format = cmdLine->get_flag_string("format");
        int limit = cmdLine->get_flag_int("limit");
        bool is_table = cmdLine->get_flag_bool("table");        
        bool is_reverse = cmdLine->get_flag_bool("reverse");        
        bool is_multi_session = cmdLine->get_flag_bool("multi_session");        

        std::string pattern = cmdLine->get_flag_string("pattern");
        std::string str_time = cmdLine->get_flag_string("time");
        std::string str_date = cmdLine->get_flag_string("date");
        std::string str_type = cmdLine->get_flag_string("type");
        
        // Formatter to create table
        TableLogFormatter table_log_formater( format );

        // Setup of the table log formatter
        table_log_formater.set_pattern( pattern );
        table_log_formater.set_time(str_time);
        table_log_formater.set_date(str_date);
        table_log_formater.set_as_table( is_table );
        table_log_formater.set_reverse( is_reverse );
        table_log_formater.set_as_multi_session( is_multi_session );
        table_log_formater.set_limit( limit );
        
        au::ErrorManager error;
        table_log_formater.init(&error);

        if( error.isActivated() )
            return au::str("Error: %s" , error.getMessage().c_str() );
        
        // Get current log file
        int tmp_file_counter = file_counter;
        
        // Push logs while not enougth
        while( !table_log_formater.enougthRecords() )
        {
            LogFile* log_file = NULL;
            std::string file_name = getFileNameForLogFile( tmp_file_counter );
            Status s = LogFile::read( file_name , &log_file );
            
            if( s == OK )
            {
                // Read logs from this file
                size_t num_logs = log_file->logs.size();
                
                LM_V(("Getting %lu logs from %s" , num_logs,  file_name.c_str()));
                
                for( size_t i = 0 ; i < num_logs ; i++ )
                {
                    // Get log and add to the table log formatter
                    Log* log = log_file->logs[ num_logs - i - 1];
                    table_log_formater.add( log );
                }

                // Delete the openen file
                delete log_file;
            }

            // Move to the previous log file
            tmp_file_counter--;
                        
            // Finish if no more files to scan
            if( tmp_file_counter < 0 )
                break;

        }
        
        LM_V(("Returning table..."));
        return table_log_formater.str();
    }
}
