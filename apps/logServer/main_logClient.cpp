

#include "parseArgs/parseArgs.h"

#include "au/console/Console.h"
#include "au/utils.h"
#include "au/console/ConsoleAutoComplete.h"
#include "au/network/SocketConnection.h"


#include "au/log/log_server_common.h"


char command[1024];
char target_host[1024];
char format[1024];
int target_port;

#define LOC "localhost"
#define LS_PORT LOG_SERVER_DEFAULT_QUERY_CHANNEL_PORT
#define DEF_FORMAT "TYPE:DATE:EXEC-AUX/FILE[LINE](PID)(TID) FUNC: TEXT"



PaArgument paArgs[] =
{
    { "-command",   command,       "", PaString, PaOpt, _i "",    PaNL, PaNL,  "Command to be executed"    },
    { "-format",    format,        "", PaString, PaOpt, _i DEF_FORMAT, PaNL, PaNL, "Log file to scan" },
    { "",           target_host,   "", PaString, PaOpt, _i LOC,   PaNL, PaNL,  "Log server hostname"       },
    { "",           &target_port,  "", PaInt,    PaOpt, LS_PORT,  1,    99999, "Log server port"           },
    
    PA_END_OF_ARGS
};

static const char* manSynopsis         = "logClient [host] [port] [-command X]";
static const char* manShortDescription =  "logClient is a command line utility to interact with a logServer\n";
static const char* manDescription      =
"\n"
"logClient is a command line utility to interact with a logServer\n"
"\n";

static const char* manExitStatus    = "0      if OK\n 1-255  error\n";
static const char* manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char* manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char* manCopyright     = "Copyright (C) 2011 Telefonica Digital";
static const char* manVersion       = "0.1";


class LogClient : public au::Console
{
    // Main socket connection with the server
    au::SocketConnection * socket_connection;

    // Server & port
    std::string host;
    int port;
    
public:

    LogClient()
    {
        socket_connection = NULL;
    }
    
    void init( std::string _host , int _port )
    {
        host = _host;
        port = _port;
        
        au::Status s = au::SocketConnection::newSocketConnection(host, port, &socket_connection );
        if (s != au::OK )
            LM_X(1, ("No possible to connect with log server at %s:%d (%s)" , host.c_str() ,  port , au::status(s) ));
        
    }
    
    
    // au::Console
    std::string getPrompt()
    {
        return au::str("logClient [%s:%d]> " , host.c_str() , port );
    }
    void evalCommand( std::string command )
    {
        if( command.length() == 0 )
            return; // Nothing to do if it just press return
        
        // Append format if not specified in comamnd line
        {
            au::CommandLine cmdLine;
            cmdLine.set_flag_string("format", "no_format");
            cmdLine.parse(command);
            if( cmdLine.get_flag_string("format") == "no_format" )
            {
                command.append(" -format \"");
                command.append(format);
                command.append("\"");
            }
        }
        
        // Take the command and send to the socket
        command.append("\n");
        au::Status s = socket_connection->writeLine(command.c_str(), 100, 1, 0);
        if( s != au::OK )
            LM_X(1, ("Error connecting with log server (%s)" , au::status(s) ));
        
        // Read answer until "END_OF_COMMAND"
        
        char header[1024];
        char* message = (char*) malloc( 1000000 );

        // Read header of answer
        s = socket_connection->readLine(header, sizeof(header), 100);
        
        if( s != au::OK )
            LM_X(1, ("Error connecting with log server (%s)" , au::status(s) ));
            
        // Remove returns at the end of the command
        au::remove_return_chars( header );

        // Show header to the user
        //writeWarningOnConsole(au::str("Received answer '%s'" , header ));
        
        // Check protocol
        au::CommandLine cmdLine;
        cmdLine.parse(header);
            
        if( cmdLine.get_num_arguments() != 2 )
            LM_X(1, ("Error connecting with log server (Wrong header)" ));
        if( cmdLine.get_argument(0) != "LogServer")
            LM_X(1, ("Error connecting with log server (Wrong header)" ));
        
            // Get length from the header
        size_t message_size = atoll( cmdLine.get_argument(1).c_str() );
        if ( message_size > 1000000 )
            LM_X(1, ("Error connecting with log server (Excesive message size %lu)" , message_size ));
                
        // Read the rest of the message
        //writeWarningOnConsole(au::str("Reading message of %lu bytes" , message_size ));
        s = socket_connection->partRead(message, message_size, "LogServer message", 300);
        
        if( s != au::OK )
            LM_X(1, ("Error connecting with log server (%s)" , au::status(s) ));
        
        // End of message
        message[message_size] = '\0';
            
        // Write on console
        writeOnConsole(message);
        
    }
    
    void autoComplete( au::ConsoleAutoComplete* info )
    {
        if( info->completingFirstWord() )
        {
            info->add("show");
            info->add("show_connections");
            info->add("show_channels");
            info->add("show_fields");
        }
    }
};

int logFd=-1;
LogClient log_client;

int main(int argC, const char *argV[])
{
    paConfig("prefix",                        (void*) "LOG_CLIENT_");
    paConfig("usage and exit on any warning", (void*) true);
    paConfig("log to screen",                 (void*) true);
    paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
    paConfig("screen line format",            (void*) "TYPE: TEXT");
    paConfig("log to file",                   (void*) false);
    paConfig("log to stderr",                 (void*) true);
    
    paConfig("man synopsis",                  (void*) manSynopsis);
    paConfig("man shortdescription",          (void*) manShortDescription);
    paConfig("man description",               (void*) manDescription);
    paConfig("man exitstatus",                (void*) manExitStatus);
    paConfig("man author",                    (void*) manAuthor);
    paConfig("man reportingbugs",             (void*) manReportingBugs);
    paConfig("man copyright",                 (void*) manCopyright);
    paConfig("man version",                   (void*) manVersion);
    
    paParse(paArgs, argC, (char**) argV, 1, true);
    //lmAux((char*) "father");
    logFd = lmFirstDiskFileDescriptor();
    
    LM_V(("Connecting to log server at %s:%d" , target_host , target_port ));
    
    // Init main log client
    log_client.init( target_host , target_port );

    if( strcmp(command, "") != 0 )
    {
        // Single command mode...
        
        exit(0);
    }
    
    // Run console in iterative mode
    log_client.runConsole();
    
   return 0;
}
