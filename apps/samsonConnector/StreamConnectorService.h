

#ifndef _H_SAMSON_CONNECTOR_SERVICE
#define _H_SAMSON_CONNECTOR_SERVICE

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
#include "StreamConnector.h"
#include "DiskAdaptor.h"
#include "SamsonAdaptor.h"

#include "BufferProcessor.h"

#include "Channel.h"
#include "Adaptor.h"
#include "ConnectorCommand.h"
#include "ConnectorCommand.h"

extern bool interactive;
extern bool run_as_daemon;
extern int sc_console_port;
extern int sc_web_port;


namespace samson { namespace connector {
    
    // Class to accept connection to monitor
    class StreamConnectorService : public au::network::ConsoleService
    {
        StreamConnector * samson_connector;
        
    public:
        
        StreamConnectorService( StreamConnector * _samson_connector ) : au::network::ConsoleService( sc_console_port )
        {
            samson_connector = _samson_connector;
        }
        
        virtual ~StreamConnectorService()
        {
            
        }
        
        virtual void runCommand( std::string command , au::Environment* environment , au::ErrorManager* error )
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
            
            // Log activity
            samson_connector->log( new Log( "RemoteConsole", "Message", command ) );
            
            // Direct activity
            samson_connector->process_command(command, error);
        }
        
        virtual void autoComplete( au::ConsoleAutoComplete* info , au::Environment* environment )
        {
            if( info->completingFirstWord() )
            {
                info->add("login");
                info->add("password");
            }
            
            // Complete with the rest of options
            samson_connector->autoComplete(info);
            
        }
      
        virtual std::string getPrompt( au::Environment* environment )
        {
            char host[1024];
            host[1023] = '\0';
            gethostname(host, 1023);
            
            if( environment->isSet("user") )
            {
                std::string user = environment->get("user","?");
                return "stc://" + user + "@" + host + " >>";
            }
            else
            {
                return std::string("stc://") + host + " >>";
            }
        }

        
    };
    
} } // End of namespace samson connector

#endif