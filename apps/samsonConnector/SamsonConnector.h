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

#include "common.h"
#include "SamsonConnection.h"
#include "ServerConnection.h"
#include "DiskConnection.h"
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
 stdin             Standard input ( only in non iterative / deamon )
 
 Outputs
 
 port:P            Open port P and accept connections
 connection:H:P    Establish a connection to host H port P
 samson:H:Q        Establish a connection to samson at host H and receive from queue Q
 disk:D            Read content from directory D
 stdout            Standard output ( only in non iterative / deamon )

 
 */

extern bool interactive;

namespace samson 
{
    namespace connector
    {
        class Channel;
        class SamsonConnectorService;
        
        class SamsonConnector :  public au::Console
        {
            // List of channels in this samsonConnector
            au::map<std::string, Channel> channels;

            // Mutex protection for channels
            au::Token token;
            
            // Service to accept monitor connection
            SamsonConnectorService* service;
            
            friend class SamsonConnectorService;


            // General environment
            au::Token token_environment;
            au::Environment environment;
            
        public:
            
            SamsonConnector();
            
            //Add service to accept monitor connection
            void add_service();
            
            // Generic command line
            void process_command( std::string command , au::ErrorManager * error );
            
            // au::Console interface
            std::string getPrompt();
            void evalCommand( std::string command );
            void autoComplete( au::ConsoleAutoComplete* info );
            
            // Review
            void review();
            
            int getNumInputItems();
            
            // Generic way to show messages on screen
            void show_message( std::string message )
            {
                if( interactive )
                    writeWarningOnConsole(message);
                else
                    LM_V(("%s" , message.c_str() ));
            }
            void show_warning( std::string message )
            {
                if( interactive )
                    writeWarningOnConsole(message);
                else
                    LM_V(("%s" , message.c_str() ));
            }
            
            void show_error( std::string message )
            {
                if( interactive )
                    writeErrorOnConsole(message);
                else
                    LM_X( 1 , ("%s" , message.c_str() ));
            }
            
            
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
            
        };
        
        // Class to accept connection to monitor
        class SamsonConnectorService : public au::network::ConsoleService
        {
            SamsonConnector * samson_connector;
            
        public:
            
            SamsonConnectorService( SamsonConnector * _samson_connector ) : au::network::ConsoleService( 1234 )
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