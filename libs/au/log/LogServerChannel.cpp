#include <time.h>
#include "LogServerChannel.h" // Own interface
#define Char_to_int(x) ((x)-48)

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
    
    
    class Pattern
    {
        
    public:
        
        bool activated;
        regex_t preg;
        int r;          // Return value of the regcomp
        au::ErrorManager error;
        
        Pattern( std::string pattern )
        {
            LM_V(("Pattern %s" , pattern.c_str() ));
            if( pattern == "" )
            {
                activated= false;
                return;
            }
            
            r = regcomp( &preg, pattern.c_str(), 0 );            
            activated = ( r == 0 );
            if( r != 0 )
            {
                char buffer[1024];
                regerror( errno , &preg , buffer , sizeof( buffer ) );
                error.set( buffer );
            }
        }

        ~Pattern()
        {
            if( activated )
                regfree(&preg);
        }

    };
    
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
        
        // Patern
        Pattern p( pattern );
        if( p.error.isActivated() )
            return au::str("Error in pattern definition: %s" , p.error.getMessage().c_str() ); // Not possible to compile regular expression

        // Time restrictions
        time_t now = time(NULL);
        time_t time = 0;
        struct tm tm;
        gmtime_r( &now , &tm );
        
        if( ( str_time != "" ) || ( str_date != "" ) )
        {
            
            if( str_time != "" )
            {
                if( str_time.length() != 8 )
                    return "Error: Wrong format for -time. It is -time HH:MM:SS\n";

                tm.tm_hour = Char_to_int(str_time[0]) * 10 + Char_to_int(str_time[1]);
                tm.tm_min = Char_to_int(str_time[3]) * 10 + Char_to_int(str_time[4]);
                tm.tm_sec = Char_to_int(str_time[6]) * 10 + Char_to_int(str_time[7]);

                LM_V(("Time interpretado %d:%d:%d" , tm.tm_hour , tm.tm_min , tm.tm_sec ));
                
            }
            
            if( str_date != "" )
            {
                if( str_date.length() != 8 )
                    return "Error: Wrong format for -date. It is -date DD/MM/YY\n";
                
                // DD/MM/YY
                tm.tm_year = 100 + Char_to_int( str_date[6] ) * 10 + Char_to_int( str_date[7] );
                tm.tm_mon  = Char_to_int( str_date[3] ) * 10 + Char_to_int( str_date[4] ) - 1;
                tm.tm_mday = Char_to_int( str_date[0] ) * 10 + Char_to_int( str_date[1] );            
            }

            // Create a different time
            time = timelocal( &tm );
            
            LM_V(("Time selector  (%s)(%s)  %lu (Now %lu)" , str_time.c_str() , str_date.c_str() ,  time , now ));
        }
        
        
        // Formatter to create table
        TableLogFormatter table_log_formater( is_table , is_reverse , format );
        
        LM_V(("Get table... table log formatter ok"));
        
        int tmp_file_counter = file_counter;
        size_t log_counter = 0;
        
        bool finish = false;
        while( !finish )
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
                    Log* log = log_file->logs[ num_logs - i - 1];
                    
                    // Check if log match the pattern
                    if( p.activated )
                        if( !log->match( &p.preg ) )
                            continue;

                    // Check if log math the time ( smaller than .... )
                    if( time > 0 )
                        if( !log->check_time(time) )
                            continue;
                    
                    // Check if the type is correct
                    if( str_type != "" )
                        if( log->log_data.type != str_type[0] )
                            continue;

                    // Detect a new session mark
                    if( !is_multi_session )
                        if( log->is_new_session() )
                        {
                            finish = true; // No more logs to be displayed
                            break;
                        }
                    
                    table_log_formater.add(log);
                    
                    // Finish condition if enougth records are collected
                    log_counter++;
                    if( limit > 0 )
                        if( log_counter >= (size_t) limit )
                        {
                            finish = true; // No more logs to be displayed
                            break;
                        }
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