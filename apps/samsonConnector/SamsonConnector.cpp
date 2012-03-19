

#include "au/string.h"

#include "Block.h"
#include "SamsonConnectorConnection.h"
#include "SamsonConnector.h" // Own interface

namespace samson {

    void SamsonConnector::add_stdin()
    {
        FileDescriptor *file_descriptor = new FileDescriptor("stdin", 0);
        connections.insert( new SamsonConnectorConnection(this, file_descriptor, connection_input ) );
    }
    
    void SamsonConnector::add_stdout()
    {
        FileDescriptor *file_descriptor = new FileDescriptor("stdout", 1);
        connections.insert( new SamsonConnectorConnection(this, file_descriptor, connection_output ) );
    }

    void SamsonConnector::add_stderr()
    {
        FileDescriptor *file_descriptor = new FileDescriptor("stdout", 2);
        connections.insert( new SamsonConnectorConnection(this, file_descriptor, connection_output ) );
    }
        
    void SamsonConnector::add_input_port( int port )
    {
        samson::NetworkListener* listener = new samson::NetworkListener( this );
        input_listeners.insertInMap(port,listener);
        listener->initNetworkListener( port );    
        listener->runNetworkListenerInBackground();
    }
    
    void SamsonConnector::add_output_port( int port )
    {
        samson::NetworkListener* listener = new samson::NetworkListener( this );
        output_listeners.insertInMap(port,listener);
        listener->initNetworkListener( port );    
        listener->runNetworkListenerInBackground();
    }
    
    void SamsonConnector::add_input_connection( std::string host , int port )
    {
        server_connections.push_back( ServerConnection( host , port , connection_input ) );
        
    }
    
    void SamsonConnector::add_output_connection( std::string host  , int port )
    {
        server_connections.push_back( ServerConnection( host , port , connection_output ) );
    }
    
    void SamsonConnector::add_samson_input_connection( std::string host  , int port , std::string queue )
    {
        samson_connections.push_back( new SamsonConnection( host , port , connection_input , queue ) );
    }
    
    void SamsonConnector::add_samson_output_connection( std::string host  , int port , std::string queue )
    {
        samson_connections.push_back( new SamsonConnection( host , port , connection_output , queue ) );
    }
    
    void SamsonConnector::push( engine::Buffer * buffer )
    {
        LM_V(("Pushing a block %s" , au::str(buffer->getSize()).c_str() ));
        
        // Create a block
        Block* block = new Block( buffer );
        
        // Put in all possible output
        int num_output = 0;
        std::set<SamsonConnectorConnection*>::iterator it_connections;
        for ( it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
        {
            (*it_connections)->push( block );
            num_output++;
        }
        
        // Check output samson connections
        for ( size_t i = 0 ; i < samson_connections.size() ; i++)
            samson_connections[i]->push( block );
        
        
        LM_V(("Pushed block to %d outputs" , num_output));
        
        // Release block
        LM_V(("Releasing block after pushing to all available outputs"));
        block->release();
        
    }
    
    void SamsonConnector::newSocketConnection( samson::NetworkListener* listener 
                                              , samson::SocketConnection * socket_connetion )
    {
        // Mutex protection
        au::TokenTaker tt(&token);
        
        // Check input or output
        int port = listener->getPort();
        if( input_listeners.findInMap(port) != NULL )
        {        
            SamsonConnectorConnection * connection;
            connection = new SamsonConnectorConnection(this,  socket_connetion , connection_input );
            connections.insert(connection);
        }
        else if( output_listeners.findInMap(port) != NULL )
        {
            SamsonConnectorConnection * connection;
            connection = new SamsonConnectorConnection(this,  socket_connetion , connection_output );
            connections.insert(connection);
        }
        else
        {
            LM_W(("Received a connection that is not an input or an output"));
            socket_connetion->close();
            delete socket_connetion;
        }
    }
    
    void SamsonConnector::exit_if_necessary()
    {
        // Mutex protection
        au::TokenTaker tt(&token);

        // ------------------------------------------------------------
        // If no input, exit
        // ------------------------------------------------------------
        if( 
           ( getNumInputConnections() == 0 ) 
           && (input_listeners.size() == 0)  
           && (getNumInputServerConnections() == 0)
           && ( getTotalOutputBufferSize() == 0 )
           )
        {
            LM_X(0, ("No more input to get data from & not more output buffers to be sent"));
        }
        
        // ------------------------------------------------------------
        // If no input, exit
        // ------------------------------------------------------------
        if( 
           ( getNumOutputConnections() == 0 ) 
           && (output_listeners.size() == 0) 
           && (getNumOutputServerConnections() == 0) 
           )
            LM_X(0, ("No more output to push data to"));
        
    }

    
    void SamsonConnector::review()
    {
        LM_V(("Review SamsonConnector"));
        
        // Mutex protection
        au::TokenTaker tt(&token);
        
        // ------------------------------------------------------------
        // Check individual input connections
        // ------------------------------------------------------------
        for ( size_t i = 0 ; i < server_connections.size() ; i++ )
        {
            std::string name = server_connections[i].getName();
            
            if( getConnection(name) != NULL )
                continue;
            
            SocketConnection* socket_connection;
            Status s = SocketConnection::newSocketConnection(server_connections[i].host
                                                             , server_connections[i].port
                                                             , &socket_connection);                                  
            if( s == OK )
            {
                // Change the name of the socket connection
                socket_connection->setName( name );
                                
                ConnectionType type = server_connections[i].type;
                SamsonConnectorConnection * connection = new SamsonConnectorConnection(this, socket_connection , type );
                LM_V(("Established conneciton %s" , connection->str().c_str() ));
                connections.insert( connection );
            }
            
        }
        
        // ------------------------------------------------------------
        // Close connection is no thread running...
        // ------------------------------------------------------------
        
        std::set<SamsonConnectorConnection*>::iterator it_connections;
        for( it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
        {
            SamsonConnectorConnection* connection = *it_connections;
            if ( connection->isFinished() )
                connections.erase( it_connections );
        }
    }
    
    size_t SamsonConnector::getNumConnections()
    {
        // Mutex protection
        au::TokenTaker tt(&token);
        
        return connections.size();
    }
    
    
    int SamsonConnector::getNumInputConnections()
    {
        int total = 0;
        
        std::set<SamsonConnectorConnection*>::iterator it_connections;
        for ( it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
            if( (*it_connections)->getType() == connection_input )
                total++;
        
        return total;
    }
    int SamsonConnector::getNumOutputConnections()
    {
        int total = 0;
        
        std::set<SamsonConnectorConnection*>::iterator it_connections;
        for ( it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
            if( (*it_connections)->getType() == connection_output )
                total++;
        
        return total;
        
    }
    
    SamsonConnectorConnection* SamsonConnector::getConnection( std::string name )
    {
        std::set<SamsonConnectorConnection*>::iterator it_connections;
        for ( it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
            if( (*it_connections)->getName() == name )
                return *it_connections;

        return NULL;
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

            output << "Available commands for samsonConnector:\n";
            output << "help        Show this help\n";
            output << "show        Show current input and output setup\n";
            output << "input XXX    Add an input ( port , connection or samson)\n";
            output << "output XXX   Add an output ( port , connection or samson)\n";
            writeOnConsole( output.str() );
        }
        else if ( main_command == "show_setup" )
        {
            au::StringVector fields ("Type" , "Method",  "Connection");
            au::StringVector formats ("left" , "left" , "left");

            au::tables::Table table( fields , formats );
            
            au::map<int , samson::NetworkListener>::iterator it_listeners;
            for( it_listeners = input_listeners.begin() ; it_listeners != input_listeners.end() ; it_listeners++ )
            {
                au::StringVector values;
                values.push_back("Input");
                values.push_back("Listener");
                values.push_back( au::str("Listening at port %d" , it_listeners->second->getPort() ));
                table.addRow(values);
            }
            
            for( it_listeners = output_listeners.begin() ; it_listeners != output_listeners.end() ; it_listeners++ )
            {
                au::StringVector values;
                values.push_back("Output");
                values.push_back("Listener");
                values.push_back( au::str("Listening at port %d" , it_listeners->second->getPort() ));
                table.addRow(values);
            }

            for ( size_t i = 0 ; i < server_connections.size() ; i++ )
            {
                au::StringVector values;
                if( server_connections[i].type == 0 )
                    values.push_back("Input");
                else
                    values.push_back("Output");

                values.push_back("Connection");
                values.push_back( au::str("Connect to %s:%d" , server_connections[i].host.c_str() , server_connections[i].port ));
                table.addRow(values);
                
            }
            
            writeOnConsole( table.str("Setup") );
            
        }
        else if ( main_command == "show" )
        {

            au::StringVector fields ("Type" , "Connection" , "Total In Bytes" , "Rate In Bytes/s", "Total Out Bytes" , "Rate Out Bytes/s" );
            au::StringVector formats ("left" , "left");
            au::tables::Table table( fields , formats );

            // Current connections
            std::set<SamsonConnectorConnection*>::iterator it_connections;
            for ( it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
            {
                SamsonConnectorConnection* connection = *it_connections;
                
                au::StringVector values;
                
                // Input or output
                if ( connection->getType() == connection_input )
                    values.push_back("Input");
                else
                    values.push_back("Output");

                // name
                values.push_back( connection->getName() );

                // Statistics
                values.push_back( au::str( connection->input_rate.getTotalSize() ) );
                values.push_back( au::str( connection->input_rate.getRate() ) );

                values.push_back( au::str( connection->output_rate.getTotalSize() ) );
                values.push_back( au::str( connection->output_rate.getRate() ) );
                
                
                table.addRow( values );
                
            }
            
            // SAMSON Connection
            for ( size_t i = 0 ; i < samson_connections.size() ; i++ )
            {
                au::StringVector values;
                
                SamsonConnection * connection = samson_connections[i];
                
                // Input or output
                if ( connection->getType() == connection_input )
                    values.push_back("Input");
                else
                    values.push_back("Output");
                
                // name
                values.push_back( connection->getName() );
                
                // Statistics
                values.push_back( "?" );
                values.push_back( "?" );
                
                values.push_back( "?" );
                values.push_back( "?" );
                
                table.addRow( values );                
            }
            
 
            writeOnConsole( table.str("Current connections") );
            
        }
        else if ( main_command == "quit" )
        {
            quitConsole();
        }
        else
            writeErrorOnConsole( au::str("Unknown command %s." , main_command.c_str() ) );
        
    }
    
    size_t SamsonConnector::getTotalOutputBufferSize()
    {
        size_t total = 0;
        
        // Current connections
        std::set<SamsonConnectorConnection*>::iterator it_connections;
        for ( it_connections = connections.begin() ; it_connections != connections.end() ; it_connections++ )
            total += (*it_connections)->getOutputBufferSize();
        
        return total;
    }

    
    void SamsonConnector::autoComplete( au::ConsoleAutoComplete* info )
    {
        if( info->completingFirstWord() )
        {
            info->add("quit");
            info->add("help");
            info->add("show");
            info->add("show_setup");
            info->add("input");
            info->add("output");
        }
        
    }
    
}
