#ifndef _H_AU_NETWORK_CONSOLE_SERVICE
#define _H_AU_NETWORK_CONSOLE_SERVICE

#include "au/Status.h"
#include "au/mutex/Token.h"
#include "au/mutex/TokenTaker.h"
#include "au/containers/set.h"

#include "au/tables/Table.h"

#include "au/Environment.h"
#include "au/au.pb.h"
#include "au/gpb.h"

#include "au/console/Console.h"

#include "au/network/Service.h"

namespace au
{
    namespace network
    {
        
        class Service;
        
        
        class ConsoleServiceClient : public Console
        {
            int port;
            SocketConnection * socket_connection;
            
        public:
            
            ConsoleServiceClient( int _port )
            {
                port = _port;
                socket_connection = NULL;
            }
            
            bool write( au::gpb::ConsolePacket* packet )
            {
                if( !socket_connection )
                {
                    writeErrorOnConsole("Not connected to any host");
                    return false;
                }
                
                // Write on the socket
                au::Status s = writeGPB( socket_connection->getFd() , packet);
                
                if( s != OK )
                {
                    writeErrorOnConsole(au::str("Not possible to sent message (%s). Disconnecting...",status(s)));
                    disconnect();
                    return false;
                }
                return true;
            }
            
            bool read( au::gpb::ConsolePacket** packet )
            {
                if( !socket_connection )
                {
                    writeErrorOnConsole("Not connected to any host");
                    return false;
                }
                
                Status s = readGPB( socket_connection->getFd() , packet , -1);
                if( s != OK )
                {
                    writeErrorOnConsole(au::str("Not possible to receive answer (%s). Disconnecting...",status(s)));
                    disconnect();
                    return false;
                }
                return true;
            }
            
            
            
            void fill_message( au::gpb::ConsolePacket* message , au::ErrorManager* error )
            {
                for ( int i = 0 ; i < message->message_size() ; i ++ )
                {
                    std::string txt = message->message(i).txt();
                    
                    switch ( message->message(i).type() ) 
                    {
                        case au::gpb::Message::message:
                            error->add_message(txt);
                            break;
                        case au::gpb::Message::warning:
                            error->add_warning(txt);
                            break;
                        case au::gpb::Message::error:
                            error->add_error(txt);
                            break;
                    }
                    
                }
            }
            
            void disconnect()
            {
                if( socket_connection )
                {
                    writeWarningOnConsole(
                                          au::str("Closing connection with %s\n" 
                                                  , socket_connection->getHostAndPort().c_str() ));
                    
                    socket_connection->close();
                    delete socket_connection;
                    socket_connection = NULL;
                    
                }
            }
            
            void connect( std::string host )
            {
                // Disconnect from previos one if any...
                disconnect(); 
                
                // Try connection
                au::Status s = SocketConnection::newSocketConnection(host, port, &socket_connection);
                if( s != OK )
                {
                    disconnect();
                    writeErrorOnConsole( au::str("Not possible to connect with %s (%s)\n" , host.c_str() , status(s) ) );
                }
                else
                    writeWarningOnConsole( au::str("Connection stablished with %s\n" , host.c_str() ) );
            }
            
            virtual std::string getPrompt()
            {
                // Still not implemented the remove-prompt mechanism...
                return ">>";
            }
            
            virtual void evalCommand( std::string command )
            {
                
                // Establish connection
                au::CommandLine cmdLine;
                cmdLine.parse(command);
                
                if( cmdLine.get_num_arguments() == 0 )
                    return;
                
                std::string main_command = cmdLine.get_argument(0);
                
                if( main_command == "disconnect" )
                {
                    disconnect();
                    return;
                }
                
                if( main_command == "connect" )
                {
                    if( cmdLine.get_num_arguments() < 2 )
                    {
                        writeErrorOnConsole("Usage: connect host");
                        return;
                    }
                    connect( cmdLine.get_argument(1) );
                    return;
                }
                
                if( !socket_connection )
                {
                    writeErrorOnConsole("Not connected to any host. Type connect 'host'");
                    return;
                }
                else
                {
                    // Write command to the server
                    au::gpb::ConsolePacket m;
                    m.set_command( command );
                    if(! write(&m) )
                        return;
                    
                    // Read answer
                    au::gpb::ConsolePacket *answer;
                    
                    if( !read( &answer ) )
                        return;
                    
                    
                    // Transform into a au::ErrorManager
                    au::ErrorManager error;
                    fill_message( answer , &error );
                    delete answer;
                    
                    // Write all messages on console
                    Console::write( &error );
                    
                }
                
            }
            
            virtual void autoComplete( ConsoleAutoComplete* info )
            {
                
                // Options for connection and disconnection...
                if( info->completingFirstWord() )
                {
                    info->add( "connect" );
                    info->add( "disconnect" );
                }
                
                // Prepare message to be send to server
                au::gpb::ConsolePacket m;
                m.set_auto_complettion_command( info->getCurrentCommand() );
                
                // Send to server
                if(! write(&m) )
                    return;
                
                // Read answer from server
                au::gpb::ConsolePacket *answer;                
                if( !read( &answer ) )
                    return;
                
                // Fill info structure with received information
                for ( int i = 0 ; i < answer->auto_completion_alternatives_size() ; i++ )
                {
                    std::string label = answer->auto_completion_alternatives(i).label();
                    std::string command = answer->auto_completion_alternatives(i).command();
                    bool add_space = answer->auto_completion_alternatives(i).add_space_if_unique();
                    info->add(label, command, add_space );
                }
                
                
            }
            
            void addEspaceSequence( std::string sequence )
            {
                
            }
            
            virtual void process_escape_sequence( std::string sequence )
            { 
            }
            
            
        };
        
        // Service based on a remote console
        
        class ConsoleService : public Service
        {
            
        public:
            
            ConsoleService( int port ) : Service ( port )
            {
                
            }
            
            virtual void runCommand( std::string command , au::Environment* environment ,au::ErrorManager* error )
            {
                // Do soemthing...
            }
            
            virtual void autoComplete( ConsoleAutoComplete* info )
            {
                
            }
            
            void fill_message( au::ErrorManager* error , au::gpb::ConsolePacket* message )
            {
                for ( size_t i = 0 ; i < error->getNumItems() ; i++ )
                {
                    ErrorMessage* error_message = error->getItem(i);
                    
                    au::gpb::Message* m = message->add_message();
                    m->set_txt( error_message->getMultiLineMessage() );
                    
                    switch ( error_message->getType() ) 
                    {
                        case item_message:
                            m->set_type(au::gpb::Message::message);
                            break;
                        case item_warning:
                            m->set_type(au::gpb::Message::warning);
                            break;
                        case item_error:
                            m->set_type(au::gpb::Message::error);
                            break;
                    }
                }
                
            }
            
            void run( SocketConnection * socket_connection , bool *quit )
            {
                // Environment for this connection
                au::Environment environment;
                
                while( true )
                {
                    
                    // Read command line from the other side
                    au::gpb::ConsolePacket *message = NULL;;
                    au::Status s = readGPB( socket_connection->getFd() , &message, -1);
                    
                    // Finish connection if not possible to read a message
                    if( s != OK )
                    {
                        LM_W(("ConsoleService: Could not read message from client correctly (%s).Closing connection" , status(s) ));
                        socket_connection->close();
                        if( message )
                            delete message;
                        return; 
                    }
                    
                    // Message that will be used in the answer...
                    au::gpb::ConsolePacket answer_message;
                    
                    
                    if( message->has_auto_complettion_command() )
                    {
                        // Auto completion request....
                        ConsoleAutoComplete info( message->auto_complettion_command() );
                        autoComplete( &info );
                        
                        // Fill answer message with alternatives
                        for ( size_t i = 0 ; i < info.getNumAlternatives() ; i++ )
                        {
                            ConsoleAutoCompleteAlternative alternative = info.getAlternative(i);
                            au::gpb::AutoCompletionAlternative* a = answer_message.add_auto_completion_alternatives();
                            a->set_command( alternative.command );
                            a->set_label( alternative.label );
                            a->set_add_space_if_unique( alternative.add_space_if_unique );
                        }
                        
                        
                    }
                    else
                    {
                        // Run this command message...
                        au::ErrorManager error;
                        runCommand( message->command() , &environment , &error );
                        fill_message( &error , &answer_message );
                        
                    }
                    
                    
                    // Create the message to answer back to the client
                    // Finish connection if not possible to read a message
                    s = writeGPB(socket_connection->getFd(), &answer_message);
                    if( s != OK )
                    {
                        LM_W(("ConsoleService: Could not send message back to client correctly (%s).Closing connection" , status(s) ));
                        socket_connection->close();
                        return; 
                    }
                }
                
            }
        };
    }
}

#endif