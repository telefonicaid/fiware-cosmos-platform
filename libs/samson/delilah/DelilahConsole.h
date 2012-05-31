#ifndef _H_DelilahConsole
#define _H_DelilahConsole

/* ****************************************************************************
 *
 * FILE                     DelilahConsole.h
 *
 * DESCRIPTION			   Console terminal for delilah
 *
 * Portions Copyright (c) 1997 The NetBSD Foundation, Inc. All rights reserved
 */
#include <cstdlib>				// atexit

#include <sstream>                  // std::ostringstream
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <algorithm>

#include "logMsg/logMsg.h"				

#include "au/network/ConsoleService.h"

#include "au/mutex/TokenTaker.h"                  // au::TokenTake
#include "au/console/Console.h"                     // au::Console
#include "au/console/ConsoleAutoComplete.h"

#include "au/tables/Select.h"
#include "au/tables/Table.h"

#include "samson/delilah/Delilah.h"			// samson::Delilah
#include "samson/delilah/DelilahCommandCatalogue.h"
#include "samson/delilah/AlertCollection.h"

namespace samson {

/**
	 Main class for the DelilahConsole program
 */

class DelilahConsole : public au::Console , public Delilah
{

    typedef enum
    {
        mode_normal,        // Normal mode ( interacting with all workers )
        mode_database,      // Local database working
        mode_logs           // logClient connection
    }DelilahConsoleMode;

    DelilahConsoleMode mode; // Internal mode ( different interaction with console )

    std::string commandFileName;

    DelilahCommandCatalogue delilah_command_catalogue;

    AlertCollection trace_colleciton;    // Collection of traces for monitoring
    std::string trace_file_name;         // Name of the traces file ( if any )
    FILE *trace_file;                    // FILE to store traces if defined with save_traces

    // Counter for the received stream buffers
    au::CounterCollection<std::string> stream_out_queue_counters;

    // Flag to indicate if we are shoing traces
    bool show_alerts;
    bool show_local_logs;
    bool show_server_logs;

    // Flag to show on screen certain messages
    bool verbose;

    // Flag to just visualize content on screen ( delilah -command  or -f XX )
    bool simple_output;

    // Flag to avoid any message visualization
    bool no_output;

    // List of alias
    au::simple_map< std::string , std::string > aliases;

    // LogClient used when working in log_client mode
    au::network::ConsoleServiceClientBase log_client;

    public:

    DelilahConsole( );
    ~DelilahConsole();

    // Intermediate function to connect
    void connect( std::string host , int port , std::string user , std::string password )
    {
        au::ErrorManager error;
        delilah_connect("console", host, port, user , password , &error );

        if ( error.isActivated() )
        {
            LM_E(("Error connecting to host:%s at port:%d, user:%s, passwd:%s, with errorMessage:%s", host.c_str(), port, user.c_str(), password.c_str(), error.getMessage().c_str()));
            showErrorMessage( error.getMessage() );
        }
    }



    // Main run command
    void run();

    // Set the command-file
    void setCommandfileName( std::string _commandFileName)
    {
        commandFileName = _commandFileName;
    }

    void setSimpleOutput()
    {
        simple_output = true;
    }

    void setNoOutput()
    {
        no_output = true;
    }

    // Eval a command from the command line
    virtual std::string getPrompt();
    virtual void evalCommand( std::string command );
    virtual void autoComplete( au::ConsoleAutoComplete* info );

    void autoCompleteOperations( au::ConsoleAutoComplete* info );
    void autoCompleteOperations( au::ConsoleAutoComplete* info , std::string type );

    void autoCompleteQueueForOperation( au::ConsoleAutoComplete* info , std::string operation_name , int argument_pos );
    void autoCompleteQueueWithFormat(au::ConsoleAutoComplete* info  ,  std::string key_format , std::string value_format);
    void autoCompleteQueues(au::ConsoleAutoComplete* info );

    virtual void process_escape_sequence( std::string sequence )
    {
        if( sequence == "samson" )
        {
            writeWarningOnConsole("SAMSON's cool ;)");
        }

        if( sequence == "q" )
        {
            appendToCommand("ls");
        }

        if( sequence == "d" )
        {
            mode = mode_database;
            refresh(); // refresh console
        }

        if( sequence == "l" )
        {
            mode = mode_logs;
            refresh(); // refresh console
        }

        if( sequence == "n" )
        {
            mode = mode_normal;
            refresh(); // refresh console
        }

    }


    // Run asynch command and returns the internal operation in delilah
    size_t runAsyncCommand( std::string command );

    // Run asynch command and wait to finish ( only used in tests )
    void runAsyncCommandAndWait( std::string command );
    
    // Functions overloaded from Delilah
    // --------------------------------------------------------

    void delilahComponentFinishNotification( DelilahComponent *component);
    void delilahComponentStartNotification( DelilahComponent *component);

    // Function to process messages from network elements not handled by Delila class
    int _receive( Packet* packet );

    // Process buffers of data received in streaming from SAMSON
    void receive_buffer_from_queue( std::string queue , engine::Buffer* buffer );


    // Notify that an operation hash finish
    virtual void notifyFinishOperation( size_t id )
    {
        std::ostringstream output;
        output << "Finished local delilah process with : " << id ;
        showWarningMessage( output.str() );
    }

    // Show a message on screen
    void showMessage( std::string message)
    {
        if( no_output )
        {
            LM_V(("%s" , message.c_str() ));
            return;
        }

        if( simple_output )
        {
            std::cout << message;
            return;
        }

        writeOnConsole( au::strToConsole( message ) );
    }

    void showWarningMessage( std::string message)
    {
        if( no_output )
        {
            LM_V(("%s" , au::str( au::purple , "%s" , message.c_str() ).c_str() ));
            return;
        }

        if( simple_output )
        {
            std::cout << au::str( au::purple , "%s" , message.c_str() );
            return;
        }

        writeWarningOnConsole( au::strToConsole( message ) );
    }

    void showErrorMessage( std::string message)
    {
        if( no_output )
        {
            LM_V(("%s" , au::str( au::red , "%s" , message.c_str() ).c_str() ));
            return;
        }

        if( simple_output )
        {
            std::cout << au::str( au::red , "%s" , message.c_str() );
            return;
        }

        writeErrorOnConsole( au::strToConsole( message ) );
    };


    virtual void showTrace( std::string message)
    {
        if( show_alerts )
            writeWarningOnConsole( message );
    }

    void add_alias( std::string key , std::string value );
    void remove_alias( std::string key );


};


}


#endif
