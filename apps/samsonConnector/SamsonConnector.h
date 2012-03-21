#ifndef _H_SAMSON_CONNECTOR
#define _H_SAMSON_CONNECTOR

#include <set>

#include "logMsg/logMsg.h"

#include "au/map.h"
#include "au/Token.h"
#include "au/string.h"
#include "au/Console.h"

#include "engine/Buffer.h"

#include "samson/network/NetworkListener.h"

#include "common.h"
#include "SamsonConnection.h"

namespace samson {
    
    
    class InputReader;
    class OutputWriter;
    class SamsonConnectorConnection;
    
    class SamsonConnector : public samson::NetworkListenerInterface , public au::Console
    {
        
        au::Token token;
        
        // Set of connections ( stdin , stdout , ports, connections )
        std::set<SamsonConnectorConnection*> connections;
        
        // Input & output listners
        au::map<int , samson::NetworkListener> input_listeners;
        au::map<int , samson::NetworkListener> output_listeners;
        
        // Individual connections
        std::vector<ServerConnection> server_connections;
        
        // Samson connections
        std::vector<SamsonConnection*> samson_connections;
        
    public:
        
        SamsonConnector() : token("SamsonPushConnectionsManager")
        {
        }
        
        // Add stdin and stdout
        void add_stdin();
        void add_stdout();
        void add_stderr();

        // Add input and output ports
        void add_input_port( int port );
        void add_output_port( int port );
        
        // Add input / output connection
        void add_input_connection( std::string  , int port );
        void add_output_connection( std::string  , int port );

        void add_samson_input_connection( std::string host  , int port , std::string queue );
        void add_samson_output_connection( std::string host  , int port , std::string queue );
        
        // samson::NetworkListenerInterface
        void newSocketConnection( samson::NetworkListener* listener 
                                         , samson::SocketConnection * socket_connetion );
        
        // General review
        void review();
        void exit_if_necessary();
        
        size_t getNumConnections();
        
        // Common method to push data
        void push( engine::Buffer * buffer );
        
        int getNumInputConnections();
        int getNumOutputConnections();
        
        int getNumInputServerConnections()
        {
            int total = 0;
            for( size_t i = 0 ; i < server_connections.size() ; i++)
                if( server_connections[i].type == connection_input )
                    total++;
            return total;
        }
        
        int getNumOutputServerConnections()
        {
            int total = 0;
            for( size_t i = 0 ; i < server_connections.size() ; i++)
                if( server_connections[i].type == connection_output )
                    total++;
            return total;
        }

        int getNumInputSamsonConnections()
        {
            int total = 0;
            for( size_t i = 0 ; i < samson_connections.size() ; i++)
                if( samson_connections[i]->getType() == connection_input )
                    total++;
            return total;
            
        }
        
        int getNumOutputSamsonConnections()
        {
            int total = 0;
            for( size_t i = 0 ; i < samson_connections.size() ; i++)
                if( samson_connections[i]->getType() == connection_output )
                    total++;
            return total;
        }

        
        // au::Console interface
        std::string getPrompt();
        void evalCommand( std::string command );
        void autoComplete( au::ConsoleAutoComplete* info );

        // Get total size pending to be sent
        size_t getTotalOutputBufferSize();
        
        // Easy way to check if a connection is stablish
        SamsonConnectorConnection* getConnection( std::string name );
        
        
        
    };
}
#endif