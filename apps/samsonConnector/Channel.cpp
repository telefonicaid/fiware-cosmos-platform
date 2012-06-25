
#include "Item.h"
#include "common.h"
#include "ServerConnection.h"
#include "ListenerItem.h"
#include "SamsonItem.h"
#include "DiskItem.h"
#include "InterChannelItem.h"
#include "Channel.h" // Own interface


extern bool interactive;
extern bool run_as_daemon;

namespace samson {
    
    namespace connector 
    {
        
        Channel::Channel( SamsonConnector * connector , std::string name , std::string splitter ) : token( "token_Channel")
        {
            // First id for an item
            items_id = 0; 
            
            // Keep name and pointer to connector
            connector_ = connector;
            name_ = name;
            splitter_ = splitter;
        }
        
        
        void Channel::review()
        {
            // Mutex protection
            au::TokenTaker tt(&token);
            
            // ------------------------------------------------------------
            // Review all items
            // ------------------------------------------------------------
            
            au::map<int, Item>::iterator it_items;
            for( it_items = items.begin() ; it_items != items.end() ; it_items++ )
            {
                Item* item = it_items->second;
                item->review();
                
                // Remove item if necessary & possible...
                if( item->removing )
                {
                    //LM_M(("Item %s should be removed..." , item->name.c_str() ));
                    
                    if( item->getNumConnections() == 0 )
                        if ( item->canBeRemoved() )
                        {
                            //LM_M(("Item %s finally removed..." , item->name.c_str() ));
                            delete item;
                            items.erase( it_items );
                        }
                }
            }
        }
        
        void Channel::add_outputs ( std::string output_string  , au::ErrorManager* error )
        {
            au::CommandLine cmdLine;
            cmdLine.parse(output_string);
            
            if( cmdLine.get_num_arguments() == 0 )
            {
                error->set("No output specified");
                return;
            }
            
            for ( int i = 0 ; i < cmdLine.get_num_arguments() ; i++ )
            {
                std::string input_string = cmdLine.get_argument(i);
                
                std::vector<std::string> components = au::split(input_string, ':');
                
                if( components[0] == "stdout" )
                {
                    if( !interactive && !run_as_daemon ) 
                    {
                        add( new StdoutItem( this ) );
                    }
                    else
                    {
                        // Non using stdout in interactive mode
                        return;
                    }
                }
                else if( components[0] == "port" )
                {
                    if( components.size() < 2 )
                    {
                        error->set("Output port without number. Please specifiy port to open (ex port:10000)");
                        return;
                    }
                    
                    int port = atoi( components[1].c_str() );
                    if( port == 0 )
                    {
                        error->set("Wrong output port");
                        return;
                    }
                    
                    // Add a listen item
                    add( new ListenerItem( this , connection_output , port ) );
                    error->add_message( au::str("Added an output item to channel %s listening plain socket connection on port %d " , name_.c_str() , port ));
                    return;
                    
                }
                else if( components[0] == "disk" )
                {
                    if( components.size() < 2 )
                    {
                        error->set("Usage disk:file_name_or_dir");
                        return;
                    }
                    
                    add( new DiskItem( this , connection_output , components[1] ) );
                    
                }
                else if( components[0] == "connection" )
                {
                    if( components.size() < 3 )
                    {
                        error->set("Output connection without host and port. Please specifiy as connection:host:port)");
                        return;
                    }
                    
                    std::string host = components[1];
                    int port = atoi( components[2].c_str() );
                    if( port == 0 )
                    {
                        error->set(au::str("Wrong connection port for %s" , host.c_str() ));
                        return;
                    }
                    
                    add( new ConnectionItem( this , connection_output , host , port ) );
                    
                }
                else if( components[0] == "samson" )
                {
                    std::string host = "localhost";
                    std::string queue = "input";
                    int port = SAMSON_WORKER_PORT;
                    
                    if( components.size() == 2 )
                    {
                        queue = components[1];
                    }
                    else if( components.size() == 3 )
                    {
                        host = components[1];
                        queue = components[2];
                    }
                    else if( components.size() >= 3 )
                    {
                        host = components[1];
                        port = atoi( components[2].c_str() );
                        queue = components[3];
                    }            
                    
                    add( new SamsonItem( this , connection_output , host , port  , queue ) );
                }
                else if( components[0] == "channel" )
                {
                    if( components.size() < 3 )
                    {
                        error->set("Output connection without host and channel. Please specifiy as channel:host:channel)");
                        return;
                    }
                    
                    std::string host = components[1];
                    std::string channel = components[2];
                    
                    add( new OutputInterChannelItem(this, host, channel ) );
                    
                }

                else
                {
                    // Error message
                    error->add_error( au::str("Unknown input %s" , components[0].c_str() ) );
                }

            }
        }
        
        void Channel::add_inputs ( std::string input_string , au::ErrorManager* error )
        {
            au::CommandLine cmdLine;
            cmdLine.parse(input_string);
            
            if( cmdLine.get_num_arguments() == 0 )
            {
                error->set("No input specified");
                return;
            }
            
            for ( int i = 0 ; i < cmdLine.get_num_arguments() ; i++ )
            {
                std::string input_string = cmdLine.get_argument(i);
                
                std::vector<std::string> components = au::split(input_string, ':');
                
                if( components[0] == "stdin" )
                {
                    if( !interactive && !run_as_daemon ) // No send to add stdout in interactive mode ;)
                        add( new StdinItem( this ) );
                    else
                    {
                        // Not adding stdin in interactive mode
                        return;
                    }
                }
                else if( components[0] == "port" )
                {
                    if( components.size() < 2 )
                    {
                        error->set("Input port without number. Please specifiy port to open (ex port:10000)");
                        return;
                    }
                    
                    int port = atoi( components[1].c_str() );
                    if( port == 0 )
                    {
                        error->set("Wrong input port");
                        return;
                    }
                    
                    // Add a listen item
                    add( new ListenerItem( this , connection_input , port ) );
                    error->add_message( au::str("Added an input item to channel %s listening plain socket connection on port %d " , name_.c_str() , port ));
                    return;
                }
                else if( components[0] == "disk" )
                {
                    if( components.size() < 2 )
                    {
                        error->set("Usage disk:file_name_or_dir");
                        return;
                    }
                    
                    add( new DiskItem( this , connection_input , components[1]  ) );
                }
                else if( components[0] == "connection" )
                {
                    if( components.size() < 3 )
                    {
                        error->set("Input connection without host and port. Please specifiy as connection:host:port)");
                        return;
                    }
                    
                    std::string host = components[1];
                    int port = atoi( components[2].c_str() );
                    if( port == 0 )
                    {
                        error->set(au::str("Wrong connection port for %s" , host.c_str() ));
                        return;
                    }
                    
                    add( new ConnectionItem( this , connection_input , host , port ) );
                    
                }
                else if( components[0] == "samson" )
                {
                    std::string host = "localhost";
                    std::string queue = "input";
                    int port = SAMSON_WORKER_PORT;
                    
                    if( components.size() == 2 )
                    {
                        queue = components[1];
                    }
                    else if( components.size() == 3 )
                    {
                        host = components[1];
                        queue = components[2];
                    }
                    else if( components.size() >= 3 )
                    {
                        host = components[1];
                        port = atoi( components[2].c_str() );
                        queue = components[3];
                    }
                    
                    add( new SamsonItem( this , connection_input , host , port  , queue ) );
                    
                }
                else if( components[0] == "channel" )
                {
                    // Able to receive connections for inter-channel connection
                    add( new InputInterChannelItem( this ) );
                }
                else
                {
                    // Error message
                    error->add_error( au::str("Unknown input %s" , components[0].c_str() ) );
                }

            }
        }
        
        // Generic method to add an item
        void Channel::add( Item * item )
        {
            if( !item )
                return;
            
            items.insertInMap(items_id, item);
            items_id++;
        }
        
        void Channel::push( engine::Buffer * buffer )
        {
            // Mutex protection
            au::TokenTaker tt(&token);
            
            au::map<int, Item>::iterator it_items;
            for( it_items = items.begin() ; it_items != items.end() ; it_items++ )
            {
                if( it_items->second->getType() == connection_output )
                    it_items->second->push( buffer );
            }
        }
        
        int Channel::getNumItems()
        {
            // Mutex protection
            au::TokenTaker tt(&token);
            return items.size();
        }
        
        int Channel::getNumOutputItems()
        {
            // Mutex protection
            au::TokenTaker tt(&token);
            int total = 0;
            au::map<int, Item>::iterator it_items;
            for( it_items = items.begin() ; it_items != items.end() ; it_items++ )
            {
                if( it_items->second->getType() == connection_output )
                    total++;
            }
            return total;

            
        }
        int Channel::getNumInputItems()
        {
            // Mutex protection
            au::TokenTaker tt(&token);
            int total = 0;
            au::map<int, Item>::iterator it_items;
            for( it_items = items.begin() ; it_items != items.end() ; it_items++ )
            {
                if( it_items->second->getType() == connection_input )
                    total++;
            }
            return total;
            
        }
        
        int Channel::getNumConnections()
        {
            // Mutex protection
            au::TokenTaker tt(&token);
            int total = 0;
            
            au::map<int, Item>::iterator it_items;
            for( it_items = items.begin() ; it_items != items.end() ; it_items++ )
                    total += it_items->second->getNumConnections();
            return total;
        }

        std::string Channel::getName()
        {
            return name_;
        }
        
        std::string Channel::getSplitter()
        {
            return splitter_;
        }

        
        std::string Channel::getInputsString()
        {
            std::ostringstream output;
            
            au::map<int, Item>::iterator it_items;
            for( it_items = items.begin() ; it_items != items.end() ; it_items++ )
            {
                Item* item = it_items->second;
                if( item->getType() == connection_input )
                {
                    output << item->short_name_  << " ";
                }
            }
            return output.str();
        }
        
        std::string Channel::getOutputsString()
        {
            std::ostringstream output;
            
            au::map<int, Item>::iterator it_items;
            for( it_items = items.begin() ; it_items != items.end() ; it_items++ )
            {
                Item* item = it_items->second;
                if( item->getType() == connection_output )
                {
                    output << item->short_name_ << " ";
                }
            }
            return output.str();
        }

        
    }
    
}
