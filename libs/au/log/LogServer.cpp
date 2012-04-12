
#include "au/log/LogServerChannel.h"
#include "au/log/LogServerQuery.h"

#include "LogServer.h" // Own interface

namespace au 
{
    LogServer::LogServer() : token("LogServer")
    {
        log_server_query = NULL;
    }
    
    bool LogServer::add_query_channel( int port )
    {
        if( log_server_query )
        {
            fprintf( stderr , "Only one query channel can be open for a log server\n" );
            return false;
        }
        
        log_server_query = new LogServerQuery( this , port );
        Status s = log_server_query->initService();
        if( s != OK )
        {
            fprintf( stderr , "Not possible to open query channel at port %d (%s)\n" , port , status(s) );
            delete log_server_query;
            log_server_query = NULL;
            return false;
        }
        
        return true;
        
    }
    
    void LogServer::add_channel( std::string name , int port ,  std::string directory , ErrorManager *error  )
    {
        LM_V(("Adding log channel %s at port %d" , name.c_str() , port ));
        
        if( channels.findInMap(name) != NULL )
        {
            error->set("Channel previouly added to this log server");
            return;
        }
        
        LogServerChannel* channel = new LogServerChannel(name , port , directory );
        channel->initLogServerChannel( error );
        
        if( error->isActivated() )
        {
            delete channel;
            return;
        }
        
        // Insert the new channel in the map
        channels.insertInMap(name, channel);
    }
    
    
    std::string LogServer::newSessionCommand(CommandLine * cmdLine)
    {
        TokenTaker tt(&token);
        std::string channel_name = cmdLine->get_flag_string("channel");
        
        // By default take the "default" channel
        LogServerChannel * channel = channels.findInMap( channel_name  );
        
        if( ! channel )
            return str("Channel %s found!\n" , channel_name.c_str() );

        channel->addNewSession();
        return au::str("Added a new session mark to channel %s\n" , channel_name.c_str() );
    }
    
    
    std::string LogServer::getLogsTable( CommandLine * cmdLine )
    {
        TokenTaker tt(&token);
        std::string channel_name = cmdLine->get_flag_string("channel");
        
        // By default take the "default" channel
        LogServerChannel * channel = channels.findInMap( channel_name  );
        
        if( ! channel )
            return str("Channel %s found!" , channel_name.c_str() );
        
        // Return logs
        return channel->getTable( cmdLine );
        
    }
    
    std::string LogServer::getChannelsTables()
    {
        
        tables::Table table("Channel|port|#Lines,f=uint64|Size,f=uint64|#Lines/s,f=uint64|Rate B/s,f=uint64");
        
        map<std::string,LogServerChannel>::iterator it_channels;
        for( it_channels = channels.begin() ; it_channels != channels.end() ; it_channels++ )
        {
            LogServerChannel* channel = it_channels->second;
            
            StringVector values;
            
            values.push_back( channel->getName() );
            values.push_back( str("%d", channel->getPort() ) );
            
            values.push_back( str("%lu", (size_t)channel->rate.getTotalNumberOfHits() ));
            values.push_back( str("%lu", (size_t)channel->rate.getTotalSize() ));
            
            values.push_back( str("%lu", (size_t)channel->rate.getHitRate() ));
            values.push_back( str("%lu", (size_t)channel->rate.getRate() ));
            
            table.addRow(  values );
        }
        
        table.setTitle("List of log channels");
        return table.str() + "\n";
    }
    
    std::string LogServer::getConnectionsTables()
    {
        std::ostringstream output;
        
        if( log_server_query ) 
        {
            tables::Table * table =  log_server_query->getConnectionsTable();
            table->setTitle("LogClient connections");
            output << table->str();
            delete table;
        }
        
        output << "\n";
        
        au::map<std::string,LogServerChannel>::iterator it_channels;
        for( it_channels = channels.begin() ; it_channels != channels.end() ; it_channels++ )
        {
            au::tables::Table * table = it_channels->second->getConnectionsTable();
            table->setTitle( au::str("Channel %s" , it_channels->first.c_str() ) );
            output << table->str();
            output << "\n";
            
            delete table;
        }
        
        return output.str();
    }}