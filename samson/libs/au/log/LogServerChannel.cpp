/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#include <time.h>
#include "LogServerChannel.h" // Own interface
#define Char_to_int(x) ((x)-48)

#include "au/containers/vector.h"
#include "logMsg/logMsg.h"                                              // LM_T
#include "logMsg/traceLevels.h"            // LmtFileDescriptors, etc.

namespace au 
{
    LogServerChannel::LogServerChannel( int port , std::string _directory ) 
    : network::Service( port ) , token("LogServerChannel")
    {
        directory = _directory;
        
        file_counter = 0;
        current_size = 0;
        fd = NULL;
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
                
                int _fd = open( current_file_name.c_str() , O_CREAT | O_WRONLY , 0644 );
                LM_LT(LmtFileDescriptors, ("Open FileDescriptor fd:%d", fd));
                
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
        
        while( !*quit )
        {
            // Rean a log
            Log *log = new Log( );
            log->add_field("host", socket_connection->getHostAndPort() );
            
            if( !log->read( socket_connection ) )
            {
                LM_V(("Closed connection from %s" , socket_connection->getHostAndPort().c_str() ));
                return; // Not possible to read a log...
            }
            
            // Add channel information to keep logs separated
            std::string exec_name = log->get("EXEC");
            std::string channel = au::str("%s_%s_%d"
                                          , exec_name.c_str()
                                          , socket_connection->getHostAndPort().c_str() 
                                          , log->log_data.pid );
            log->add_field("channel", channel );

            // Add log...
            add( log );
            
        }
    }
    
    
    void LogServerChannel::add( Log*log )
    {
        // Mutex protection
        au::TokenTaker tt(&token); 

        // Push log to the on-memory log
        log_container.push( log );

        // Monitorize rate of logs
        rate.push( log->getTotalSerialitzationSize() );
        
        // Check max size for file
        if( fd )
            if( current_size > 64000000 )
            {
                fd->close();
                delete fd;
                fd = NULL;
            }
        
        // Open if necessary
        if (!fd)
        {
            current_size = 0;
            
            au::ErrorManager error;
            openFileDescriptor(&error);        
            if( error.isActivated() )
            {
                LM_W(("Not possible to open local file to save logs. Logs will be deninitelly lost"));
                return;
            }
        }
        
        // Write to file
        if( fd )
            log->write( fd );
        
        
        current_size += log->getTotalSerialitzationSize();
        
    }
    
    void LogServerChannel::addNewSession()
    {
        Log log;
        log.set_new_session();
        
        // Add is protected with the mutex
        add(&log);
        
        // Clear on memory logs
        log_container.clear();
        
    }
    
    class ChannelInfo
    {
    public:
        
        std::string name_;
        
        int logs_;      // Number of logs
        size_t size_;   // Total size

        au::Descriptors descriptors_;
        
        std::string time_; // Most recent time stamp
        
        ChannelInfo( std::string name , Log* log )
        {
            name_ = name;
            
            logs_ = 0;
            size_ = 0;
            
            // First time
            time_ = log->get("date") + " " + log->get("time");
            
            add_log( log );
        }
        
        void add_log( Log* log )
        {
            logs_ ++;
            size_ += log->getTotalSerialitzationSize();

            std::string type = log->get("TYPE");
            if( type == "" )
                type = "?";
            descriptors_.add(type);
        }
    };
    
    class ChannelsInfo
    {
        au::vector<ChannelInfo> channel_info;
        
    public:
        
        void add_channel( std::string channel , Log* log )
        {
            for( size_t i = 0 ; i < channel_info.size() ; i++)
                if( channel_info[i]->name_ == channel )
                {
                    channel_info[i]->add_log( log );
                    return;
                }
            // Create a new one
            ChannelInfo* _channel_info = new ChannelInfo( channel , log );
            channel_info.push_back( _channel_info );
            
        }
        
        std::string getTable()
        {
            au::tables::Table table( "Channel|Last time|#Logs|Type|Size,f=uint64" );
            table.setTitle("Channels");
            for( size_t i = 0 ; i < channel_info.size() ; i++ )
            {
                au::StringVector values;

                values.push_back( channel_info[i]->name_ );
                values.push_back( channel_info[i]->time_ );
                
                values.push_back( au::str("%d" , channel_info[i]->logs_ ) );
                values.push_back( channel_info[i]->descriptors_.str() );
                values.push_back( au::str("%lu" , channel_info[i]->size_ ) );
                
                table.addRow( values );
            }
            return table.str();
        }
        
        size_t getNumChannels()
        {
            return channel_info.size();
        }
    };
    
    
    std::string LogServerChannel::getInfo()
    {
        return log_container.getInfo();
    }
    
    std::string  LogServerChannel::getChannelsTable( au::CommandLine * cmdLine )
    {
        // Get formats from 
        int limit             = cmdLine->get_flag_int("limit");
        bool is_multi_session = cmdLine->get_flag_bool("multi_session");        
        bool file = cmdLine->get_flag_bool("file");
        
        
        // Get current log file
        int tmp_file_counter = file_counter;
        
        // Information about channels
        ChannelsInfo channel_info;
        
        // Table based on on-memory logs
        // --------------------------------------------------------
        if( !file )
        {
            au::TokenTaker tt( &log_container.token );
            
            au::list<Log>::iterator it;
            for( it = log_container.logs.begin() ; it != log_container.logs.end() ; it++ )
            {
                // Get the log
                Log* log = *it;
                
                // Get channel
                std::string channel = log->get("channel");
                
                // Collect this information
                channel_info.add_channel( channel , log );
                
                if( limit > 0 )
                    if( ((int)channel_info.getNumChannels()) >= limit )
                        break;
            }
            return channel_info.getTable();
        }
        
        // Table based on files
        // --------------------------------------------------------
        bool finish = false;
        while( !finish )
        {
            LogFile* log_file = NULL;
            std::string file_name = getFileNameForLogFile( tmp_file_counter );
            Status s = LogFile::read( file_name , &log_file );
            
            if( s == OK )
            {
                // Scan logs for this channel
                size_t num_logs = log_file->logs.size();
                for( size_t i = 0 ; i < num_logs ; i++ )
                {
                    // Get log and add to the table log formatter
                    Log* log = log_file->logs[ num_logs - i - 1 ];
                    
                    if( !is_multi_session && log->is_new_session() )
                    {
                        finish = true;
                        break;
                    }
                    
                    // Get channel
                    std::string channel = log->get("channel");

                    // Collect this information
                    channel_info.add_channel( channel , log );
                    
                    if( limit > 0 )
                        if( ((int)channel_info.getNumChannels()) >= limit )
                        {
                            finish =true;
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
        
        return channel_info.getTable();
    }

    std::string LogServerChannel::getTable( au::CommandLine * cmdLine )
    {
        // Mutex protection is not necessary any more since it is based on files
        //au::TokenTaker tt(&token); 
        
        // Get formats from 
        std::string format = cmdLine->get_flag_string("format");
        int limit = cmdLine->get_flag_int("limit");
        bool is_table = cmdLine->get_flag_bool("table");        
        bool is_reverse = cmdLine->get_flag_bool("reverse");        
        bool is_multi_session = cmdLine->get_flag_bool("multi_session");        
        bool file = cmdLine->get_flag_bool("file");

        std::string pattern = cmdLine->get_flag_string("pattern");
        std::string str_time = cmdLine->get_flag_string("time");
        std::string str_date = cmdLine->get_flag_string("date");
        std::string str_type = cmdLine->get_flag_string("type");
        std::string channel = cmdLine->get_flag_string("channel");
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
        table_log_formater.set_channel( channel );
        table_log_formater.set_type(str_type);
        
        au::ErrorManager error;
        table_log_formater.init(&error);

        if( error.isActivated() )
            return au::str("Error: %s" , error.getMessage().c_str() );
        
        // Get current log file
        int tmp_file_counter = file_counter;
       
        // Table based on on-memory logs
        // --------------------------------------------------------
        if( !file )
        {
            au::TokenTaker tt( &log_container.token );

            au::list<Log>::iterator it;
            for( it = log_container.logs.begin() ; it != log_container.logs.end() ; it++ )
            {
                // Add the log to the table formatter
                table_log_formater.add( *it );

                if( table_log_formater.enougthRecords() )
                    break;
            }
            
            return table_log_formater.str();
        }
        
        // Table based on files
        // --------------------------------------------------------
        while( !table_log_formater.enougthRecords() )
        {
            LogFile* log_file = NULL;
            std::string file_name = getFileNameForLogFile( tmp_file_counter );
            Status s = LogFile::read( file_name , &log_file );
            
            if( s == OK )
            {
                // Read logs from this file
                size_t num_logs = log_file->logs.size();
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
        
        return table_log_formater.str();
    }
}
