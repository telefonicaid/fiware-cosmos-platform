

#include "au/string.h"

#include "Block.h"
#include "SamsonConnectorConnection.h"
#include "SamsonConnectorListener.h"
#include "SamsonConnector.h" // Own interface

extern char input_splitter_name[1024];
extern bool interactive;

namespace samson {
    
    void SamsonConnector::add_outputs ( std::string output_string  , au::ErrorManager* error )
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
                if( !interactive ) // No send to add stdout in interactive mode ;)
                    add( new SamsonConnectorConnection(this, connection_output , "stdout", new au::FileDescriptor("stdout", 1) ) );
                else
                {
                    //error->set("Not possible to add stdout in iterative mode");
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
                
                SamsonConnectorListener* listener = new SamsonConnectorListener( this , connection_output , port );
                if( listener->status_init == au::OK )
                    add( listener );
                else
                {
                    error->set( au::str("Not possible to add listener at port %d. (%s)" , port , status( listener->status_init ) ) );
                    delete listener;
                }
            }
            else if( components[0] == "disk" )
            {
                if( components.size() < 2 )
                {
                    error->set("Usage disk:file_name_or_dir");
                    return;
                }
                add( new DiskConnection( this , connection_output , components[1] ) );
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
                add( new ServerConnection( this , connection_output , host , port ) );
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
                
                add( new SamsonConnection( this , connection_output , host , port  , queue ) );
            }
        }
    }
    
    void SamsonConnector::add_inputs ( std::string input_string , au::ErrorManager* error )
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
                if( !interactive ) // No send to add stdout in interactive mode ;)
                    add( new SamsonConnectorConnection(this, connection_input , "stdin", new au::FileDescriptor("stdin", 0) ) );
                else
                {
                    //error->set("Not possible to add stdin in iterative mode");
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
                
                SamsonConnectorListener* listener = new SamsonConnectorListener( this , connection_input , port );
                if( listener->status_init == au::OK )
                    add( listener );
                else
                {
                    error->set( au::str("Not possible to add listener at port %d. (%s)" , port , status( listener->status_init ) ) );
                    delete listener;
                }
            }
            else if( components[0] == "disk" )
            {
                if( components.size() < 2 )
                {
                    error->set("Usage disk:file_name_or_dir");
                    return;
                }
                add( new DiskConnection( this , connection_input , components[1]  ) );
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
                add( new ServerConnection( this  , connection_input, host , port ) );
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
                
                add( new SamsonConnection( this , connection_input , host , port  , queue ) );
            }
        }
    }

    // Generic method to add an item
    void SamsonConnector::add( SamsonConnectorItem * item , int parent_id )
    {
        au::TokenTaker tt(&token);
        
        if( !item )
            return;
        
        item->samson_connector_id = items_id++;
        item->parent_samson_connector_id = parent_id;
        items.insertInMap(item->samson_connector_id, item);
        
        show_message( au::str("New connection: %s" , item->getDescription().c_str() ));
        
    }
    
    void SamsonConnector::push( engine::Buffer * buffer , SamsonConnectorItem *item )
    {
        
        // Mutex protection
        au::TokenTaker tt(&token);
        
        au::map<int, SamsonConnectorItem>::iterator it_items;
        for( it_items = items.begin() ; it_items != items.end() ; it_items++ )
        {
            if( it_items->second->getType() == connection_output )
                if( it_items->second->isConnected() )
                {
                    _report_output_block( it_items->second->getSamsonconnectorId() , buffer->getSize() );
                    it_items->second->push( buffer );
                }
        }
    }

    void SamsonConnector::_show_last_messages()
    {
        int total_id = 0;
        LM_V(("Total %s" , items.findInMap(total_id)->str_total_statistics().c_str() ));
        
    }

    void SamsonConnector::_exit_if_necessary_for_no_outputs()
    {
        
        if( interactive )
            return; // No exit if interactive is enabled

        // To be implemented
    }
    
    void SamsonConnector::_exit_if_necessary_for_no_inputs()
    {
        
        if( interactive )
            return; // No exit if interactive is enabled
        
        au::map<int, SamsonConnectorItem>::iterator it_items;
        for( it_items = items.begin() ; it_items != items.end() ; it_items++ )
        {
            if( it_items->first == 0 )
                continue;
            
            SamsonConnectorItem *item = it_items->second;

            // There is an input here...
            if ( item->getType() == connection_input )
                return;
            
            size_t output_size = item->getOuputBufferSize();
            if(  output_size > 0 )
                return;
        }
        
        _show_last_messages();
        LM_V(("No more input data... exit."));
        exit(1);
    }
    
    void SamsonConnector::review()
    {
        
        // Mutex protection
        au::TokenTaker tt(&token);

        // ------------------------------------------------------------
        // Review all items
        // ------------------------------------------------------------
        
        au::map<int, SamsonConnectorItem>::iterator it_items;
        for( it_items = items.begin() ; it_items != items.end() ; it_items++ )
            it_items->second->review();

        // ------------------------------------------------------------
        // Remove items if possible
        // ------------------------------------------------------------
        
        for( it_items = items.begin() ; it_items != items.end() ;  )
        {
            SamsonConnectorItem* item = it_items->second;
            if ( item->canBeRemoved() )
            {
                show_message( au::str("Connection finished: %s [%s]" , item->getDescription().c_str() , item->str_total_statistics().c_str() ) );
                items.erase( it_items );
            }

            it_items++;
        }
        
        // ------------------------------------------------------------
        // Exit if necessary
        // ------------------------------------------------------------

        
        if( setup_complete )
        {
            _exit_if_necessary_for_no_inputs();
            _exit_if_necessary_for_no_outputs();        
        }
        
    }
    
    std::string SamsonConnector::getPrompt()
    {
        return "SamsonConnector> ";
        
    }
    
    void SamsonConnector::evalCommand( std::string command )
    {
        au::CommandLine cmdLine;
        cmdLine.parse( command );
        
        if( cmdLine.get_num_arguments() == 0 )
            return;
        
        std::string main_command = cmdLine.get_argument(0);
        
        if( main_command == "help" )
        {
            std::ostringstream output;

            au::tables::Table table( "Command|Description,left" );
            
            table.addRow( au::StringVector("help","Show this help"));
            table.addRow( au::StringVector("show","Show current input and output setup"));
            table.addRow( au::StringVector("add_input","add_input [port:8888] [connection:host:port] [samson:host:queue]"));
            table.addRow( au::StringVector("add_output","add_output [port:8888] [connection:host:port] [samson:host:queue]"));
            table.setTitle("Available commands for samsonConnector:");
            output << table.str();
            
            writeOnConsole( output.str() );
        }
        else if ( main_command == "memory_status" )
        {
            writeOnConsole( au::str("Used memory %s / %s " 
                                    , au::str(engine::MemoryManager::shared()->getUsedMemory() ).c_str()
                                    , au::str(engine::MemoryManager::shared()->getMemory() ).c_str()
                                    ) );
        }
        else if ( main_command == "add_input" )
        {
            if( cmdLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole("Usage: add_input [port:8888] [connection:host:port] [samson:host:queue]");
                return;
            }
            au::ErrorManager error;
            add_inputs( cmdLine.get_argument(1) , &error );
            if( error.isActivated() )
                writeErrorOnConsole( error.getMessage() );
            else
                writeOnConsole("OK\n");
        }
        else if ( main_command == "add_output" )
        {
            if( cmdLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole("Usage: add_output [port:8888] [connection:host:port] [samson:host:queue]");
                return;
            }
            au::ErrorManager error;
            add_outputs( cmdLine.get_argument(1) , &error );
            if( error.isActivated() )
                writeErrorOnConsole( error.getMessage() );
            else
                writeOnConsole("OK\n");
        }
        else if ( main_command == "show" )
        {

            au::tables::Table table( "Id,left|Type,left|Connection,left|Status|In Bytes|In Bytes/s|Out Bytes|Out Bytes/s" );
            
            // Inputs             
            for ( int type = connection_input ; type <= connection_output ; type++ )
            {
                au::map<int, SamsonConnectorItem>::iterator it_items;
                for( it_items = items.begin() ; it_items != items.end() ; it_items++ )
                {
                    SamsonConnectorItem* item = it_items->second;

                    if( item->getSamsonconnectorId() == 0 )
                        continue;
                    
                    if( item->getType() !=  type )
                        continue;
                        
                    au::StringVector values;
                    values.push_back( au::str("%d" , item->getSamsonconnectorId() ) );
                    values.push_back( str_ConnectionType( item->getType() ) );
                    //values.push_back( au::str( "[%d/%d]%s", item->getSamsonconnectorId() , item->getParentSamsonconnectorId(), item->getName().c_str() ) );
                    values.push_back( item->getName() );
                    values.push_back( item->getStatus() );
                    
                    values.push_back( au::str( item->get_input_total() ) );
                    values.push_back( au::str( item->get_input_rate() ) );
                    values.push_back( au::str( item->get_output_total() ) );
                    values.push_back( au::str( item->get_output_rate() ) );
                    
                    table.addRow(values);
                }
                
                table.addSeparator();
            }
            
            // Total
            int total_id = 0;
            SamsonConnectorItem* item = items.findInMap(total_id);
            
            au::StringVector values;
            values.push_back( "" );
            values.push_back( "" );
            values.push_back( "TOTAL" );
            values.push_back( "" );
            values.push_back( au::str( item->get_input_total() ) );
            values.push_back( au::str( item->get_input_rate() ) );
            values.push_back( au::str( item->get_output_total() ) );
            values.push_back( au::str( item->get_output_rate() ) );
            
            table.addRow(values);
            
            
            table.setTitle("SamsonConnector");
            writeOnConsole( table.str() );
            
        }
        else if ( main_command == "quit" )
        {
            exit(0);
            quitConsole();
        }
        else
            writeErrorOnConsole( au::str("Unknown command %s." , main_command.c_str() ) );
        
    }
    
    void SamsonConnector::autoComplete( au::ConsoleAutoComplete* info )
    {
        if( info->completingFirstWord() )
        {
            info->add("quit");
            info->add("help");
            info->add("memory_status");
            info->add("show");
            info->add("show_setup");
            info->add("add_input");
            info->add("add_output");
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
    
    void SamsonConnector::report_input_block( int id , size_t size )
    {        
        au::TokenTaker tt(&token);
        while( id != -1 )
        {
            SamsonConnectorItem* item = items.findInMap(id);
            if( item )
            {
                item->report_input_block(size);
                id = item->parent_samson_connector_id;
            }
            else
                id = -1;
        }
    }
    void SamsonConnector::_report_output_block( int id , size_t size )
    {
        while( id != -1 )
        {
            SamsonConnectorItem* item = items.findInMap(id);
            if( item )
            {
                item->report_output_block(size);
                id = item->parent_samson_connector_id;
            }
            else
                id = -1;
        }
    }

    
}
