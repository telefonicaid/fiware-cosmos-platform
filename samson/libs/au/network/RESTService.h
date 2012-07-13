
#ifndef _H_AU_NETWORK_REST_SERVICE
#define _H_AU_NETWORK_REST_SERVICE

#include "au/Status.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/containers/set.h"

#include "au/tables/Table.h"

#include "au/Environment.h"
#include "au/au.pb.h"
#include "au/gpb.h"

#include "au/string.h"
#include "au/utils.h"
#include "au/Object.h"

#include "au/console/Console.h"

#include "au/network/NetworkListener.h"
#include "au/network/SocketConnection.h"
#include "au/network/Service.h"

namespace au
{
    namespace network
    {
        
        
        class RESTServiceCommandBase : public au::Object
        {
            // Mechanism to block main thread
            au::Token token;
            bool finished;
            
        public:
            
            RESTServiceCommandBase() : token( "RESTServiceCommandBase" )
            {
                finished = false;
            }
            
            void wait()
            {
                while( true )
                {
                    au::TokenTaker tt(&token);
                    if( finished )
                        return;
                    else
                        tt.stop();
                }
            }
            
            void finish()
            {
                au::TokenTaker tt(&token);
                finished = true;
                tt.wakeUp();
            }

        };
        
        class RESTServiceCommand : public RESTServiceCommandBase
        {
            
        public:
            
            // Command fields
            std::string command;           // GET, PUT, ...
            std::string resource;          // url... 
            StringVector path_components;  // Paths in the url
            std::string format;            // Extension of the resource (.xml , .json , .html )

            // First line in request
            char request_line[1024];
            
            // Header environment
            au::Environment header;
            
            // Body
            char * data;
            size_t data_size;
            
            // Error manager
            au::ErrorManager error;
            
            // Output commands
            int http_state;
            std::ostringstream output;
            
            // Redirect location ( if any, output is ignored )
            std::string redirect;

            // Constructor
            RESTServiceCommand();
            ~RESTServiceCommand();
            
            // Read command from a socket
            au::Status read( SocketConnection* socket_connection );
            
            // Write answer to the socket
            au::Status write( SocketConnection* socket_connection );
            
            // Command to append something at the output
            void append( std::string txt );

            // Set status to respond for this request
            void set_http_state( int s );
            
            void appendFormatedElement( std::string name , std::string value );
            void appendFormatedError( std::string message );
            void appendFormatedError( int _http_state , std::string message );

            void set_redirect( std::string redirect_resource );
            
            std::string getErrorMessage()
            {
                return error.getMessage();
            }
            
            
        };
        
        class RESTServiceInterface
        {
        public:
            virtual void process( RESTServiceCommand* command )=0;
        };
        
        class RESTService : private Service
        {
            // Interface to build response for the request
            RESTServiceInterface* interface;
            
        public:
            
            RESTService( int port , RESTServiceInterface* _interface );
            virtual ~RESTService();

            // Init service ( runnning something in background )
            au::Status initRESTService();
            
            // Stop background thread
            void stop( bool wait);
            
            // Service interface
            void run( SocketConnection * socket_connection , bool *quit );
            
            // Debug information
            std::string getStatus()
            {
                return Service::getStatus();
            }
            
        };
        
    }
}

#endif