

#include "au/utils.h"
#include "log_server_common.h"

#include "LogServer.h" 
#include "LogServerChannel.h"

#include "LogServerQuery.h" // Own interface

namespace au 
{
    
    std::string LogServerQuery::getAnswer( std::string command )
    {
        CommandLine cmdLine;
        cmdLine.set_flag_string("channel", LOG_SERVER_DEFAULT_CHANNEL_NAME);
        cmdLine.set_flag_string("format", "TYPE:TEXT");       // Format of each log
        cmdLine.set_flag_int("limit", 0);                     // Max number of logs
        cmdLine.set_flag_boolean("table");
        cmdLine.set_flag_string("channel", "not used here");
        cmdLine.parse(command);
        
        if( cmdLine.get_num_arguments() == 0)
            return "No command provided\n";
        
        // Get the main command
        std::string main_command = cmdLine.get_argument(0);
        
        if( main_command == "show_connections" )
            return log_server->getConnectionsTables();
        
        if( main_command == "show_channels" )
            return log_server->getChannelsTables();
        
        // Show logs
        if( main_command == "show" )
            return log_server->getLogsTable( &cmdLine );
        
        return au::str("Unknown command %s\n" , main_command.c_str() );
    }
    
    
    void LogServerQuery::run( au::SocketConnection * socket_connection , bool *quit )
    {        
        char line[1024];
        
        while( ! *quit )
        {            
            // Read input line
            au::Status s = socket_connection->readLine(line, 1024, 300);
            
            if( s != au::OK )
            {
                socket_connection->close();
                return;
            }
            
            if( strlen(line) > 0 )
            {
                // Remove returns at the end of the command
                au::remove_return_chars( line );
                
                // Get the answer
                std::string answer = getAnswer( line );
                std::string header = au::str("LogServer %lu\n" , answer.length() ); 
                
                // Write header and message
                socket_connection->partWrite(header.c_str(), header.length(), "LogServer answer header");
                socket_connection->partWrite(answer.c_str(), answer.length(), "LogServer answer");
                
                LM_V(("Answer to '%s' in %lu bytes" , line ,answer.length() ));
                
            }
            sleep(1);
        }
    }
    
}