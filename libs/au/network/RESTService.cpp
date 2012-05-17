

#include "RESTService.h" // Own interface


namespace au
{
    namespace network
    {
        
        RESTServiceCommand::RESTServiceCommand()
        {
            http_state = 200; // By default 200 response
        }
        
        // Read command from a socket
        au::Status RESTServiceCommand::read( SocketConnection* socket_connection )
        {
            // Read a line from socket
            char line[1024];
            au::Status s = socket_connection->readLine( line , sizeof(line) , 10 );
            
            if (s == au::OK)
            {
                // Remove last "\n" "\r" characters.
                au::remove_return_chars( line );
                
                // Read the rest of the REST Request, but without parsing it ...
                char flags[1024];
                s = socket_connection->readBuffer(flags, sizeof(flags), 1);
                if (s == au::OK)
                    LM_T(LmtRest, ("Rest of GET:\n%s", flags));
                else
                    LM_W(("error reading rest of REST request"));
                
                // Process incomming line with cmdLine
                au::CommandLine cmdLine;
                cmdLine.parse( line );
                
                if( cmdLine.get_num_arguments() < 2 )
                {
                    error.set( au::str("Unexpected format. Incomming line %s" , line ));
                    return au::Error;
                }
                
                // Get request parts...
                command = cmdLine.get_argument(0);
                resource = cmdLine.get_argument(1);
                
                // Get path componenets and format
                path_components = StringVector::parseFromString( resource , '/' );
                
                // Extract extension from the last one
                if( path_components.size() > 0 )
                {
                    size_t pos = path_components[path_components.size()-1].rfind(".");
                    if( pos != std::string::npos )
                    {
                        format = path_components[path_components.size()-1].substr( pos + 1);
                        path_components[path_components.size()-1] 
                        = path_components[path_components.size()-1].substr(0,pos);
                    }
                    else
                        format == "";
                }
                else
                    format == "";
                
                
                
                return au::OK;
            }
            else
            {
                error.set( "Error reading incomming command" );
                return au::Error;
            }
        }
        
        // Write answer to the socket
        au::Status RESTServiceCommand::write( SocketConnection* socket_connection )
        {
            // String with a complete answer
            std::string data = output.str();
            
            // Prepare header of the HTTP message
            std::ostringstream header; 
            
            // Redirect message
            if( redirect.length() > 0 )
            {
                // Prepare redirect header
                header << "HTTP/1.1 302 Found\n";
                header << "Location:   " << redirect << "\n";
                header << "Content-Type:   application/txt; charset=utf-8\n";
                header << "Content-Length: 0\n";
                header << "\n";
                header << "\n";
                
                std::string full_output = header.str();
                socket_connection->writeLine( full_output.c_str() , 1, 0, 100); 
                return au::OK;
            }
            
            
            switch (http_state)
            {
                case 200:
                    header << "HTTP/1.1 200 OK\n";
                    break;
                    
                case 400:
                    header << "HTTP/1.1 400 Bad Request\n";
                    break;
                    
                case 404:
                    header << "HTTP/1.1 404 Not Found\n";
                    break;
                    
                default:
                    header << "HTTP/1.1 Bad Request \n"; 
                    break;
            }
            
            header << "Content-Length: " << data.length() << "\n";
            header << "Connection: close\n";
            header << "\n";
            
            // Complete output stream of data
            std::ostringstream full_output;
            full_output << header.str();
            full_output << data;
            
            // Write the complete output
            // Try just once, timeout 0.0001 seconds
            socket_connection->writeLine( full_output.str().c_str() , 1, 0, 100); 
            return au::OK;
        }
        
        // Command to append something at the output
        void RESTServiceCommand::append( std::string txt )
        {
            output << txt;
        }
        
        
        void RESTServiceCommand::set_http_state( int s )
        {
            http_state = s;
        }
        
        void RESTServiceCommand::appendFormatedElement( std::string name , std::string value )
        {
            std::ostringstream output;
            if (format == "xml")
                au::xml_simple(output, name, value );
            else if( format == "json" )
                au::json_simple(output, name, value );
            else if( format == "html" )
                output << "<h1>" << name << "</h1>" << value;
            else
                output << name << ":\n" << value;
            
            append( output.str() );
        }
        
        void RESTServiceCommand::appendFormatedError( std::string message )
        {
            appendFormatedElement( "error" , message );
        }
        
        void RESTServiceCommand::appendFormatedError( int _http_state , std::string message )
        {
            set_http_state(_http_state);
            appendFormatedElement( "error" , message );
        }
        
        void RESTServiceCommand::set_redirect( std::string redirect_resource )
        {
            redirect = redirect_resource;
        }
        
        RESTService::RESTService( int port , RESTServiceInterface* _interface ) : Service( port )
        {
            // Keep a pointer to the interface to get REST answers
            interface = _interface;
        }
        
        RESTService::~RESTService()
        {
            
        }
        
        au::Status RESTService::initRESTService()
        {
            // Init socket service 
            return initService();
        }
        
        void RESTService::stop()
        {
            Service::stop();
        }
        
        void RESTService::run( SocketConnection * socket_connection , bool *quit )
        {
            if( *quit )
                return;
            
            // Read HTTP packet
            RESTServiceCommand command;
            au::Status s = command.read(socket_connection);
            if( s != au::OK )
            {
                LM_W(("Error in REST interface (%s)" , status(s)));
                socket_connection->close();
                return;
            }
            
            // Get anser from the service
            interface->process(&command);
            
            // Return anser for this request
            s = command.write( socket_connection );
            if( s != au::OK )
            {
                LM_W(("Error in REST interface (%s)" , status(s)));
                socket_connection->close();
                return;
            }
            
            // Close socket connection in all cases
            socket_connection->close();
            
            
        }
        
        
        
    }
}