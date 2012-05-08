

#include "au/string.h"
#include "au/ThreadManager.h"

#include "Block.h"
#include "Channel.h"
#include "SamsonConnectorConnection.h"
#include "SamsonConnectorListener.h"
#include "SamsonConnector.h" // Own interface

extern char input_splitter_name[1024];
extern bool interactive;

namespace samson {
    namespace connector {

        void* review_samson_connector(void* p)
        {
            SamsonConnector* samson_connector = (SamsonConnector*)p;
            
            // Endless loop waiting for data....
            while( true )
            {
                samson_connector->review();
                usleep(100000);
            }

			return NULL;
        }
        
        SamsonConnector::SamsonConnector() 
            : token("SamsonConnector") 
            , token_environment("SamsonConnector::token_environment")
        {
            std::string name = "default";
            channels.insertInMap( name , new Channel(this , "default") );
            
            // No service by default
            service = NULL;
            
            // Create a thread to review 
            pthread_t t;
            au::ThreadManager::shared()->addThread( "SamsonConector:review" , &t, NULL, review_samson_connector, this);
            
        }
        
        void SamsonConnector::add_service()
        {
            if( service )
                return;
            
            service = new SamsonConnectorService( this );
            au::Status s = service->initService();
            if( s != au::OK )
                LM_W(("Not possible to start service to receive connections from samsonConnectorClient"));
        }
        
        
        int SamsonConnector::getNumInputItems()
        {
            int total = 0;
            
            au::TokenTaker tt(&token);
            
            // Review all channels
            au::map<std::string, Channel>::iterator it_channels;
            for( it_channels = channels.begin() ; it_channels != channels.end() ; it_channels++ )
                total += it_channels->second->getNumInputItems();
         
            return total;
        }

        
        void SamsonConnector::review()
        {
            au::TokenTaker tt(&token);

            // Review all channels
            au::map<std::string, Channel>::iterator it_channels;
            for( it_channels = channels.begin() ; it_channels != channels.end() ; it_channels++ )
                it_channels->second->review();
        }

        
        std::string SamsonConnector::getPrompt()
        {
            return "SamsonConnector> ";
        }
        
        void SamsonConnector::evalCommand( std::string command )
        {
            au::ErrorManager error;
            process_command( command , &error );
            write( &error );
        }
        
        void SamsonConnector::autoComplete( au::ConsoleAutoComplete* info )
        {
            if( info->completingFirstWord() )
            {
                info->add("quit");
                info->add("help");
                info->add("memory_status");
                
                info->add("show_channels");
                info->add("show_items");
                info->add("show_connections");
                
                info->add("add_channel");
                
                info->add("add_input");
                info->add("add_output");
                info->add("remove");
            }
            
            if( info->completingSecondWord( "add_input"  ) )
            {
                info->setHelpMessage( "add_input [port:8888] [connection:host:port] [samson:host:queue]" );
                info->add("connection:", "connection:", false);
                info->add("port:", "port:", false);
                info->add("samson:", "samson:", false);
            }
            if( info->completingSecondWord( "add_output"  ) )
            {
                info->setHelpMessage( "add_output [port:8888] [connection:host:port] [samson:host:queue]" );
                info->add("connection:", "connection:", false);
                info->add("port:", "port:", false);
                info->add("samson:", "samson:", false);
            }
        }
        
        void SamsonConnector::process_command( std::string command , au::ErrorManager * error )
        {
            au::TokenTaker tt(&token);

            // Command line to be processed
            CommandLine cmdLine( command );
            
            if( cmdLine.get_num_arguments() == 0 )
            {
                error->set("No command present");
                return;
            }
            
            // My commands
            std::string main_command = cmdLine.get_argument(0);

            if( main_command == "add_channel" )
            {
                if( cmdLine.get_num_arguments() < 2 )
                {
                    error->set("Usage: add_channel name" );
                    return;
                }
                
                std::string name = cmdLine.get_argument(1);
                if( channels.findInMap( name ) != NULL )
                {
                    error->set( au::str("Channel %s already exist" , name.c_str() ) );
                    return;
                }
                    
                // Insert new channel
                channels.insertInMap( name , new Channel( this , name ) );
                error->add_message( au::str("Channel %s added." , name.c_str() ) );
                return;
            }
            
            if( main_command == "help" )
            {
                au::tables::Table table( "Command|Description,left" );
                
                table.addRow( au::StringVector("help","Show this help"));
                table.addRow( au::StringVector("show","Show current input and output setup"));
                table.addRow( au::StringVector("add_input","add_input [port:8888] [connection:host:port] [samson:host:queue]"));
                table.addRow( au::StringVector("add_output","add_output [port:8888] [connection:host:port] [samson:host:queue]"));
                table.setTitle("Available commands for samsonConnector:");
                
                error->add_message( table.str() );
                return;
            }
            
            if ( main_command == "memory_status" )
            {
                std::string message =  au::str("Used memory %s / %s " 
                                               , au::str(engine::MemoryManager::shared()->getUsedMemory() ).c_str()
                                               , au::str(engine::MemoryManager::shared()->getMemory() ).c_str()
                                               );
                error->add_message( message );
                return;
            }
            
            if ( main_command == "show_channels" )
            {
                au::tables::Table table( "Channel,left|#items|#connections|In Bytes|In Bytes/s|Out Bytes|Out Bytes/s" );
                table.setTitle("Channels");
                
                // Review all channels
                au::map<std::string, Channel>::iterator it_channels;
                for( it_channels = channels.begin() ; it_channels != channels.end() ; it_channels++ )
                {
                    
                    Channel *channel = it_channels->second;
                    
                    au::StringVector values;
                    values.push_back( it_channels->first );

                    values.push_back( au::str("%d", channel->getNumItems() ) );
                    values.push_back( au::str("%d", channel->getNumConnections() ) );
                    
                    values.push_back( au::str( channel->traffic_statistics.get_input_total() ) );
                    values.push_back( au::str( channel->traffic_statistics.get_input_rate() ) );
                    values.push_back( au::str( channel->traffic_statistics.get_output_total() ) );
                    values.push_back( au::str( channel->traffic_statistics.get_output_rate() ) );
                    
                    table.addRow(values);
                }
                
                error->add_message(table.str());
                return;
                
            }
            
            if( main_command == "show_items" )
            {
                au::tables::Table table( "Channel,left|Type,left|Item,left|Status|#connections|In Bytes|In Bytes/s|Out Bytes|Out Bytes/s" );
                table.setTitle( "Items" );
                
                au::map<std::string, Channel>::iterator it_channels;
                for( it_channels = channels.begin() ; it_channels != channels.end() ; it_channels++ )
                {
                    
                    Channel *channel = it_channels->second;
                    
                    // Lock the token in the channel
                    au::TokenTaker tt(&channel->token);
                    
                    au::map<int, Item>::iterator it_items;
                    for( it_items = channel->items.begin() ; it_items != channel->items.end() ; it_items++ )
                    {
                        Item *item = it_items->second;
                        
                        
                        au::StringVector values;
                        values.push_back( channel->getName() );
                        values.push_back( item->getTypeStr() );
                        values.push_back( au::str("[%d] %s",it_items->first , item->getName().c_str() ) );
                        values.push_back(item->getStatus());
                        
                        values.push_back( au::str("%d", item->getNumConnections() ) );
                        values.push_back( au::str( item->traffic_statistics.get_input_total() ) );
                        values.push_back( au::str( item->traffic_statistics.get_input_rate() ) );
                        values.push_back( au::str( item->traffic_statistics.get_output_total() ) );
                        values.push_back( au::str( item->traffic_statistics.get_output_rate() ) );
                        
                        table.addRow(values);
                    }
                    
                    error->add_message(table.str());
                    return;
                    
                }
            }
            
            if( main_command == "show_connections" )
            {
                au::tables::Table table( "Channel,left|Type,left|Item,left|Connection,left|Status|In Bytes|In Bytes/s|Out Bytes|Out Bytes/s" );
                table.setTitle( "Connections" );
                
                au::map<std::string, Channel>::iterator it_channels;
                for( it_channels = channels.begin() ; it_channels != channels.end() ; it_channels++ )
                {
                    
                    Channel *channel = it_channels->second;
                    
                    // Lock the token in the channel
                    au::TokenTaker tt(&channel->token);

                    
                    au::map<int, Item>::iterator it_items;
                    for( it_items = channel->items.begin() ; it_items != channel->items.end() ; it_items++ )
                    {
                        Item *item = it_items->second;
                        
                        // Lock the token in the item
                        au::TokenTaker tt(&item->token);
                        
                        au::map<int, Connection>::iterator it_connections;
                        for( it_connections = item->connections.begin() ; it_connections != item->connections.end() ; it_connections++ )
                        {
                            Connection * connection = it_connections->second;
                            
                            au::StringVector values;
                            values.push_back( channel->getName() );
                            values.push_back( connection->getTypeStr() );
                            values.push_back( au::str("[%d] %s",it_items->first , item->getName().c_str() ) );
                            values.push_back( au::str("[%d] %s",it_connections->first , connection->getName().c_str() ) );
                            
                            values.push_back( connection->getStatus() );
                            
                            values.push_back( au::str( connection->traffic_statistics.get_input_total() ) );
                            values.push_back( au::str( connection->traffic_statistics.get_input_rate() ) );
                            values.push_back( au::str( connection->traffic_statistics.get_output_total() ) );
                            values.push_back( au::str( connection->traffic_statistics.get_output_rate() ) );
                            
                            table.addRow(values);

                        }
                    }
                    
                    error->add_message(table.str());
                    return;
                    
                }
            }

            
            // Get channel name
            std::string channel_name = cmdLine.getChannel();
            Channel * channel = channels.findInMap( channel_name );
            if( channel )
                channel->process_command( &cmdLine , error );
            else
                error->set( au::str("Channel %s not found" , channel_name.c_str() ) );
        }
        
    }

    
}
