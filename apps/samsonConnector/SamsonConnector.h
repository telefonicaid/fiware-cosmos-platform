#ifndef _H_SAMSON_CONNECTOR
#define _H_SAMSON_CONNECTOR

#include <set>

#include "logMsg/logMsg.h"

#include "au/containers/map.h"
#include "au/containers/vector.h"
#include "au/mutex/Token.h"
#include "au/string.h"
#include "au/console/Console.h"

#include "engine/Buffer.h"

#include "au/network/NetworkListener.h"
#include "au/network/ConsoleService.h"
#include "au/network/RESTService.h"
#include "au/network/NetworkListener.h"

#include "common.h"
#include "SamsonConnector.h"
#include "DiskItem.h"
#include "SamsonItem.h"

#include "BufferProcessor.h"

#include "Channel.h"
#include "Item.h"
#include "ConnectorCommand.h"
#include "ConnectorCommand.h"

/*

 ------------------------------------------------------------

 Input 1 -> BufferProcessor                   Output1
 Input 2 -> BufferProcessor --> Channel 1  --> Output2
 Input 3 -> BufferProcessor                   Output3
 
 Input 1 -> BufferProcessor                   Output1
 Input 2 -> BufferProcessor --> Channel 2 --> Output2
 Input 3 -> BufferProcessor                   Output3
 
 ------------------------------------------------------------
 
 Inputs
 
 port:P            Open port P and accept connections
 connection:H:P    Establish a connection to host H port P
 samson:H:Q        Establish a connection to samson at host H and receive from queue Q
 disk:D            Read content from directory D
 channel           All interchannel connections
 stdin             Standard input ( only in non iterative / deamon )
 
 Outputs
 
 port:P            Open port P and accept connections
 connection:H:P    Establish a connection to host H port P
 samson:H:Q        Establish a connection to samson at host H and receive from queue Q
 disk:D            Read content from directory D
 channel:H:C       Connect to channel "C" at host "H"
 stdout            Standard output ( only in non iterative / deamon )

 
 */

extern bool interactive;
extern bool run_as_daemon;
extern int sc_console_port;
extern int sc_web_port;


namespace samson 
{
    namespace connector
    {
        class Channel;
        class SamsonConnectorService;
        class InputInterChannelConnection;
        
        class SamsonConnector :  public au::Console
        , public au::network::RESTServiceInterface 
        , public au::NetworkListenerInterface
        {
            // List of channels in this samsonConnector
            au::map<std::string, Channel> channels;

            // Mutex protection for entire data-model in SamsonConnector
            au::Token token;
            
            // Service to accept monitor connection
            SamsonConnectorService* service;
            
            friend class SamsonConnectorService;

            // General environment
            au::Token token_environment;
            au::Environment environment;
            
            // REST Interface
            au::network::RESTService *rest_service;
            
            // Simple listener for interchannel connection
            au::NetworkListener *inter_channel_listener;
            
            // List of connection for interchannel
            au::list<InputInterChannelConnection> input_inter_channel_connections;
            
        public:
            
            SamsonConnector();
            
            // Init rest interface
            void initRESTInterface();

            // Init listener to receive inter channel connections
            void initInterChannelInteface();
            
            //Add service to accept monitor connection
            void add_service();
            
            // Generic command line
            void process_command( std::string command , au::ErrorManager * error );
            
            // au::Console interface
            std::string getPrompt();
            void evalCommand( std::string command );
            void autoComplete( au::ConsoleAutoComplete* info );
            void autoCompleteWithChannelNames( au::ConsoleAutoComplete* info );
            
            // Review
            void review();
            
            // Get the number of input items
            int getNumInputItems();

            // Get pending data to be sent
            size_t getOutputConnectionsSize();
            
            std::string getPasswordForUser( std::string user )
            {
                au::TokenTaker tt( &token_environment);
                return environment.get( au::str("user_%s", user.c_str()) , "" );
            }
            
            void setPasswordForUser( std::string user , std::string password )
            {
                au::TokenTaker tt( &token_environment);
                environment.set( au::str("user_%s", user.c_str()) , password );
            }
            
            // au::network::RESTServiceInterface
            void process( au::network::RESTServiceCommand* command );

            // au::network::NetworkListenerInterface
            virtual void newSocketConnection( au::NetworkListener* listener 
                                             , au::SocketConnection * socket_connetion );

            // Write errors depending on setup ( interactive, deamon, normal )
            void writeError( std::string message );
            
            // Select channel for an interchannel connection
            void select_channel( InputInterChannelConnection* connection , std::string target_channel , au::ErrorManager *error );

        private:
            
            au::tables::Table* getChannelsTable();
            au::tables::Table* getItemsTable();
            au::tables::Table* getItemsTableForChannel( std::string channel );
            au::tables::Table* getConnectionsTable();
            au::tables::Table* getConnectionsTableForChannel( std::string channel );

            au::tables::Table* getInputInterChannelConnections();
            
            
        };


        // Class to accept connection to monitor
        class SamsonConnectorService : public au::network::ConsoleService
        {
            SamsonConnector * samson_connector;
            
        public:
            
            SamsonConnectorService( SamsonConnector * _samson_connector ) : au::network::ConsoleService( sc_console_port )
            {
                samson_connector = _samson_connector;
            }
            
            virtual void runCommand( std::string command , au::Environment* environment ,au::ErrorManager* error )
            {
                // Parse login and password commands....
                au::CommandLine cmdLine;
                cmdLine.set_flag_string("p", "");
                cmdLine.parse(command);
                
                if( cmdLine.get_num_arguments() > 0 )
                {
                    if( cmdLine.get_argument(0) == "login" )
                    {
                        if( cmdLine.get_num_arguments() < 2 )
                        {
                            error->set("Usage: login user [-p password]");
                            return;
                        }
                        
                        std::string user = cmdLine.get_argument(1);
                        std::string password = cmdLine.get_flag_string("p");
                        
                        if( user != "root" )
                        {
                            error->set("Only root user supported in this release");
                            return;
                        }
                        
                        if( password == samson_connector->getPasswordForUser( user ) )
                        {
                            error->add_warning(au::str("Login correct as %s" , user.c_str()));
                            environment->set("user" , user );
                        }
                        else
                        {
                            error->add_error(au::str("Wrong password for user %s" , user.c_str()));
                        }
                        return;
                    }
                    
                    if( cmdLine.get_argument(0) == "password" )
                    {
                        if( cmdLine.get_num_arguments() < 2 )
                        {
                            error->set("Usage: password new_password");
                            return;
                        }
                        
                        if( !environment->isSet("user") )
                        {
                            error->set("Not logged! Please log using command 'login user-name -p password'");
                            return;
                        }

                        std::string user = environment->get("user","no-user");
                        std::string new_password = cmdLine.get_argument(1);
                        samson_connector->setPasswordForUser( user , new_password );
                        error->add_warning(au::str("Set new password for user %s correctly" , user.c_str()));
                        return;
                        
                    }
                    
                }
                
                // Protect against non authorized users
                if( !environment->isSet("user") )
                {
                    error->set("Not logged! Please log using command 'login user-name -p password'");
                    return;
                }
                
                // Direct connection
                samson_connector->process_command(command, error);
            }
            
            virtual void autoComplete( au::ConsoleAutoComplete* info )
            {
                if( info->completingFirstWord() )
                {
                    info->add("login");
                    info->add("password");
                }
                
                // Complete with the rest of options
                samson_connector->autoComplete(info);
                
            }
            
        };
        
        
    }
}
#endif