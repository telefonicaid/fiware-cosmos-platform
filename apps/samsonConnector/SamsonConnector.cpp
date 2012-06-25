

#include "au/string.h"
#include "au/ThreadManager.h"

#include "Channel.h"
#include "InterChannelItem.h"
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
            
            // No service by default
            service = NULL;
            
            // No interchannel service by default
            inter_channel_listener = NULL;
            
            // Create a thread to review 
            pthread_t t;
            au::ThreadManager::shared()->addThread( "SamsonConector:review" , &t, NULL, review_samson_connector, this);
        }
        
        void SamsonConnector::initRESTInterface()
        {
            if( rest_service )
                return; // Already init
            
            // Create the rest service
            rest_service = new au::network::RESTService( sc_web_port , this );
            au::Status s = rest_service->initRESTService();
            if( s != au::OK )
            {
                writeError( au::str("Error opening REST interface on port %d" , sc_web_port) );
                delete rest_service;
                rest_service = NULL;
            }
        }

        void SamsonConnector::initInterChannelInteface()
        {
            if( inter_channel_listener )
                return;
            
            // Create a listener over port SAMSON_CONNECTOR_INTERCHANNEL_PORT to receive inter channel connections
            int p = SAMSON_CONNECTOR_INTERCHANNEL_PORT;
            inter_channel_listener = new au::NetworkListener( this );
            au::Status s = inter_channel_listener->initNetworkListener( p );
            if( s != au::OK )
            {
                // It is not possible to init  port for inter channel connections
                writeError( au::str("Not possible to init inter-channel listener at port %d. This could be a major issue" , p ) );
                delete inter_channel_listener;
                inter_channel_listener=NULL;
            }
            else
                inter_channel_listener->runNetworkListenerInBackground();
        }
        
        // au::network::RESTServiceInterface
        void SamsonConnector::process( au::network::RESTServiceCommand* command )
        {
            // Mutex protection
            au::TokenTaker tt(&token);
            
            // No command error
            if( command->path_components.size() == 0 )
            {
                command->appendFormatedError("No command provided");
                return;
            }
            
            if( command->path_components[0] == "channels" )
            {
                if( command->path_components.size() != 1 )
                {
                    command->appendFormatedError("Wrong format. Only /channels is supported");
                    return;
                }
                
                au::tables::Table* table = getChannelsTable();
                command->append( table->strFormatted( command->format ) );
                return;
            }

            if( command->path_components[0] == "channel" )
            {
                if( command->path_components.size() < 2 )
                {
                    command->appendFormatedError("Wrong format. Only /channel/name/items|connections is supported");
                    return;
                }
                
                std::string channel = command->path_components[1];
                std::string channel_info = "items"; // Default values 
                if( command->path_components.size() > 2 )
                    channel_info = command->path_components[2];

                if( channel_info == "items" )
                {
                    au::tables::Table* table = getItemsTableForChannel( channel );
                    if( ! table )
                    {
                        command->appendFormatedError( au::str("Channel %s not found" , channel.c_str() ) );
                        return;
                    }
                    
                    command->append( table->strFormatted( command->format ) );
                    delete table;
                    return;
                }
                
                if( channel_info == "connections" )
                {
                    au::tables::Table* table = getConnectionsTableForChannel( channel );
                    if( ! table )
                    {
                        command->appendFormatedError( au::str("Channel %s not found" , channel.c_str() ) );
                        return;
                    }
                    
                    command->append( table->strFormatted( command->format ) );
                    delete table;
                    return;
                }
                
                command->appendFormatedError("Wrong format. Only /channel/name/items|connections is supported");
                return;
            }
        }
        
        void SamsonConnector::add_service()
        {
            if( service )
                return;
            
            service = new SamsonConnectorService( this );
            au::Status s = service->initService();
            if( s != au::OK )
                LM_W(("Not possible to start service to receive connections from samsonConnectorClient"));
            else
                LM_M(("Console Service started..."));
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
        
        size_t SamsonConnector::getOutputConnectionsSize()
        {
            size_t total = 0;
            
            au::TokenTaker tt(&token);
            
            au::map<std::string, Channel>::iterator it_channels;
            for( it_channels = channels.begin() ; it_channels != channels.end() ; it_channels++ )
            {
                Channel * channel = it_channels->second;
                
                // Lock the token in the channel
                au::TokenTaker tt(&channel->token);
                
                au::map<int, Item>::iterator it_items;
                for( it_items = channel->items.begin() ; it_items != channel->items.end() ; it_items++ )
                {
                    Item *item = it_items->second;
                    
                    if( item->getType() == connection_output )
                    {
                        
                        // Lock the token in the item
                        au::TokenTaker tt(&item->token);
                        
                        au::map<int, Connection>::iterator it_connections;
                        for( it_connections = item->connections.begin() ; it_connections != item->connections.end() ; it_connections++ )
                        {
                            Connection * connection = it_connections->second;
                            total += connection->getSize(); // Get accumulated size to be sent
                        }
                    }
                }                
            }
            
            return total;
            
        }


        
        void SamsonConnector::review()
        {
            au::TokenTaker tt(&token);

            // Review input inter channel connections
            au::list<InputInterChannelConnection>::iterator it;
            for( it = input_inter_channel_connections.begin() ; it != input_inter_channel_connections.end() ; it++ )
            {
                InputInterChannelConnection* c = *it;
                
                if( c->canBeRemoved() )
                {
                    delete c;
                    input_inter_channel_connections.erase( it );
                }
                
            }
            
            
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

        void SamsonConnector::autoCompleteWithChannelNames( au::ConsoleAutoComplete* info )
        {
            au::map<std::string, Channel>::iterator it;
            for (it = channels.begin() ; it != channels.end() ; it++ )
                info->add( it->first );
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
                
                info->add("add_input_to_channel");
                info->add("add_output_to_channel");
                
                info->add("show_input_inter_channel_connections");
                
                info->add("remove_item_in_channel");
            }

            if( ( info->completingSecondWord("add_input_to_channel") )
               || ( info->completingSecondWord("add_output_to_channel") )
               || ( info->completingSecondWord("remove_item_in_channel") ))
            {
                // Autocomplete with channel names
                autoCompleteWithChannelNames( info );
            }
            
            if( ( info->firstWord() == "add_input_to_channel" ) && info->completingThirdWord() )
            {
                info->setHelpMessage( "add_input_to_channel channel [port:8888] [connection:host:port] [samson:host:queue]" );
                info->add("connection:", "connection:", false);
                info->add("port:", "port:", false);
                info->add("samson:", "samson:", false);
                info->add("inter_channel");
            }
            
            if( ( info->firstWord() == "add_output_to_channel" ) && info->completingThirdWord() )
            {
                info->setHelpMessage( "add_output [port:8888] [connection:host:port] [samson:host:queue]" );
                info->add("connection:", "connection:", false);
                info->add("port:", "port:", false);
                info->add("samson:", "samson:", false);
            }
        }
        
        void SamsonConnector::process_command( std::string command , au::ErrorManager * error )
        {
            // Mutex protection
            au::TokenTaker tt(&token);

            // Command line to be processed
            CommandLine cmdLine( command );
            
            if( cmdLine.get_num_arguments() == 0 )
                return;
            
            // My commands
            std::string main_command = cmdLine.get_argument(0);

            if( main_command == "add_channel" )
            {
                if( cmdLine.get_num_arguments() < 2 )
                {
                    error->set("Usage: add_channel name splitter" );
                    return;
                }
                
                std::string name = cmdLine.get_argument(1);
                std::string splitter = "";
                if( cmdLine.get_num_arguments() > 2 ) 
                    splitter = cmdLine.get_argument(2);
                
                if( channels.findInMap( name ) != NULL )
                {
                    error->set( au::str("Channel %s already exist" , name.c_str() ) );
                    return;
                }
                    
                // Insert new channel
                channels.insertInMap( name , new Channel( this , name , splitter ) );
                error->add_message( au::str("Channel %s added." , name.c_str() ) );
                return;
            }
            
            if( main_command == "help" )
            {
                au::tables::Table table( "Command|Description,left" );
                
                table.addRow( au::StringVector("help","Show this help"));
                
                table.addRow( au::StringVector("show_channels","Show current channels"));
                table.addRow( au::StringVector("show_items","Show items for all channels"));
                table.addRow( au::StringVector("show_connections","Show all connections for all items for all channels"));
                
                table.addRow( au::StringVector("add_input_to_channel","add_input channel [port:8888] [connection:host:port] [samson:host:queue]"));
                table.addRow( au::StringVector("add_output_to_channel","add_output channel [port:8888] [connection:host:port] [samson:host:queue]"));

                table.addRow( au::StringVector("remove_item_in_channel","remove_item_in_channel channel item_id"));
                
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
                au::tables::Table* table = getChannelsTable();
                error->add_message(table->str());
                delete table;
                return;
                
            }
            
            if( main_command == "show_items" )
            {
                au::tables::Table* table = getItemsTable();
                error->add_message(table->str());
                delete table;
                
                return;
            }
            
            if( main_command == "show_connections" )
            {
                au::tables::Table *table = getConnectionsTable();
                error->add_message(table->str());
                delete table;
                return;
            }

            if( main_command == "show_input_inter_channel_connections" )
            {
                au::tables::Table *table = getInputInterChannelConnections();                
                error->add_message(table->str());
                delete table;
                return;
            }
            
            if( main_command == "add_input_to_channel" )
            {
                if( cmdLine.get_num_arguments() < 3 )
                {
                    error->set("Usage: add_input_to_channel channel input_definition" );
                    return;
                }
                
                std::string channel_name = cmdLine.get_argument(1);
                std::string input_definition = cmdLine.get_argument(2);
                
                Channel * channel = channels.findInMap( channel_name );
                
                if( !channel )
                {
                    error->set( au::str("Unknown channel %s" , channel_name.c_str() ));
                    return;
                }
                
                channel->add_inputs( input_definition , error );
                if( !error->isActivated() )
                    error->add_message( au::str("Input %s added to channel %s" 
                                                , input_definition.c_str() 
                                                , channel_name.c_str() ));
                return;
            }
            
            if( main_command == "add_output_to_channel" )
            {
                if( cmdLine.get_num_arguments() < 3 )
                {
                    error->set("Usage: add_output_to_channel channel input_definition" );
                    return;
                }
                
                std::string channel_name = cmdLine.get_argument(1);
                std::string input_definition = cmdLine.get_argument(2);
                
                Channel * channel = channels.findInMap( channel_name );
                
                if( !channel )
                {
                    error->set( au::str("Unknown channel %s" , channel_name.c_str() ));
                    return;
                }
                
                channel->add_outputs( input_definition , error );
                if( !error->isActivated() )
                    error->add_message( au::str("Output %s added to channel %s" 
                                                , input_definition.c_str() 
                                                , channel_name.c_str() ));
                return;
            }
            
            if( main_command == "remove_item_in_channel" )
            {
                if( cmdLine.get_num_arguments() < 3 )
                {
                    error->set("Usage: remove channel item_id" );
                    return;
                }
                
                std::string channel_name = cmdLine.get_argument(1);
                int id = ::atoi( cmdLine.get_argument(1).c_str());

                Channel * channel = channels.findInMap( channel_name );
                
                if( !channel )
                {
                    error->set( au::str("Unknown channel %s" , channel_name.c_str() ));
                    return;
                }

                Item* item = channel->items.findInMap(id);
                
                if( !item )
                {
                    error->set( au::str("Item %d not found in channel %s" , id, channel_name.c_str() ));
                    return;
                }
                
                error->add_message( au::str("Item [%d][%s] is marked to be removed" , id , item->getName().c_str() ) );
                item->set_removing(); // Close all connections and mark as to be removed
                return;
            }
            
            // Unknown command error
            error->set( au::str("Unknown command %s" , main_command.c_str() ) );
            
            
        }
        
        au::tables::Table* SamsonConnector::getChannelsTable()
        {
            std::string fields = "Channel,left|Inputs|splitter,left|Outputs|#items|#connections|In Bytes|In Bytes/s|Out Bytes|Out Bytes/s";
            
            au::tables::Table *table = new au::tables::Table( fields );
            table->setTitle("Channels");
            
            // Review all channels
            au::map<std::string, Channel>::iterator it_channels;
            for( it_channels = channels.begin() ; it_channels != channels.end() ; it_channels++ )
            {
                
                Channel *channel = it_channels->second;
                
                au::StringVector values;
                values.push_back( it_channels->first );


                values.push_back( channel->getInputsString() );
                values.push_back( channel->getSplitter() );
                values.push_back( channel->getOutputsString() );
                
                values.push_back( au::str("%d", channel->getNumItems() ) );
                values.push_back( au::str("%d", channel->getNumConnections() ) );
                
                values.push_back( au::str( channel->traffic_statistics.get_input_total() ) );
                values.push_back( au::str( channel->traffic_statistics.get_input_rate() ) );
                values.push_back( au::str( channel->traffic_statistics.get_output_total() ) );
                values.push_back( au::str( channel->traffic_statistics.get_output_rate() ) );
                
                table->addRow(values);
            }
            
            return table;
        }
        
        au::tables::Table* SamsonConnector::getItemsTableForChannel( std::string channel_name )
        {
            Channel* channel = channels.findInMap(channel_name);
            if( !channel )
                return NULL;
            
            std::string fields = "Channel,left|Type,left|Item,left|Status|#connections|In Bytes|In Bytes/s|Out Bytes|Out Bytes/s";
            au::tables::Table* table = new au::tables::Table( fields );
            table->setTitle( au::str( "Items for channel %s" , channel_name.c_str()  ) );
            
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
                
                table->addRow(values);
            }
            
            return table;
        }
        
        au::tables::Table* SamsonConnector::getInputInterChannelConnections()
        {
            
            au::tables::Table *table = new au::tables::Table( "Host&Port|Status,left" );
            table->setTitle( "Input InterChannel connections" );
            
             au::list<InputInterChannelConnection>::iterator it;
            for( it = input_inter_channel_connections.begin() ; it != input_inter_channel_connections.end() ; it++ )
            {
                
                InputInterChannelConnection* c = *it;
                table->addRow( au::StringVector(c->getHostAndPort(),c->getStatus()));
            }
            return table;
        }

        
        au::tables::Table* SamsonConnector::getConnectionsTableForChannel( std::string channel_name )
        {
            Channel* channel = channels.findInMap(channel_name);
            if( !channel )
                return NULL;

            std::string fields = "Channel,left|Type,left|Item,left|Connection,left|Status|In Bytes|In Bytes/s|Out Bytes|Out Bytes/s";
            au::tables::Table *table = new au::tables::Table( fields );
            table->setTitle( "Connections" );
            
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
                    
                    table->addRow(values);
                    
                }
            }
            
            return table;
            
            
        }
        
        au::tables::Table* SamsonConnector::getConnectionsTable()
        {
            
            std::string fields = "Channel,left|Type,left|Item,left|Connection,left|Status|In Bytes|In Bytes/s|Out Bytes|Out Bytes/s";
            au::tables::Table *table = new au::tables::Table( fields );
            table->setTitle( "Connections" );
            
            au::map<std::string, Channel>::iterator it_channels;
            for( it_channels = channels.begin() ; it_channels != channels.end() ;  )
            {
                
                Channel *channel = it_channels->second;
                
                // Lock the token in the channel
                au::TokenTaker tt(&channel->token);
                
                
                au::map<int, Item>::iterator it_items;
                for( it_items = channel->items.begin() ; it_items != channel->items.end() ;  )
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
                        
                        table->addRow(values);
                        
                    }
                    
                    it_items++;
                    
                    // Write a separator in the table if this is not the last element
                    if( it_items !=  channel->items.end() )
                        if( item->connections.size() > 0 )
                            table->addSeparator();

                }
                
                it_channels++;
                
                // Write a separator in the table if this is not the last element
                if( it_channels !=  channels.end() )
                    if( channel->items.size() > 0 )
                        table->addSeparator();

            }
            
            return table;
        }
        
        au::tables::Table* SamsonConnector::getItemsTable()
        {
            std::string fields = "Channel,left|Type,left|Item,left|Status|#connections|In Bytes|In Bytes/s|Out Bytes|Out Bytes/s";
            au::tables::Table* table = new au::tables::Table( fields );
            table->setTitle( "Items" );
            
            au::map<std::string, Channel>::iterator it_channels;
            for( it_channels = channels.begin() ; it_channels != channels.end() ;  )
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
                    
                    table->addRow(values);
                }

                it_channels++;

                // Write a separator in the table if this is not the last line of the table
                if( it_channels !=  channels.end() )
                    if( channel->items.size() > 0 )
                        table->addSeparator();
                 

            }
            
            return table;
        }
        
        void SamsonConnector::writeError( std::string message )
        {
            if( interactive )
            {
                writeErrorOnConsole( message );
                return;
            }
            
            if( run_as_daemon )
            {
                // Accumulate this error in a log system....
            }
            
            LM_E(( message.c_str() ));
        }
        
        void SamsonConnector::newSocketConnection( au::NetworkListener* listener 
                                         , au::SocketConnection * socket_connetion )
        {

            LM_W(("New connection %s" , socket_connetion->getHostAndPort().c_str() ));
            
            // Create a new connection
            std::string name = au::str("inter_channel from %s" , socket_connetion->getHostAndPort().c_str() );
            InputInterChannelConnection * connection = new InputInterChannelConnection( this , name , socket_connetion ); 
            
            // Push into the list of connections
            input_inter_channel_connections.push_back( connection );
                                          
        }

        void SamsonConnector::select_channel( InputInterChannelConnection* connection , std::string target_channel , au::ErrorManager *error )
        {
            au::TokenTaker tt(&token);
            
            Channel* channel = channels.findInMap(target_channel);
            
            if( !channel )
            {
                error->set( au::str("Channel %s not found" , target_channel.c_str() ));
                return;
            }
            
            au::TokenTaker tt2(&channel->token);
            
            au::map<int, Item>::iterator it_items;
            for( it_items = channel->items.begin() ; it_items != channel->items.end() ; it_items++ )
            {
                Item* item = it_items->second;
                
                // Check if this item can receive this connection
                if( item->accept(connection) )
                {
                    // Set this item as connection's item
                    connection->item = item;
                    
                    // Extract from the list of pending connections
                    input_inter_channel_connections.extractFromList( connection );
                    
                    // Inser in selected item
                    item->add( connection );
                    
                    return;
                }
            }
            
            error->set( au::str("Channel %s do not accept this connection" , target_channel.c_str() ));
            return;
        }
    }
}
