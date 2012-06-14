/* ****************************************************************************
 *
 * FILE                     DelilahConsole.h
 *
 * DESCRIPTION			   Console terminal for delilah
 *
 */

#include <iostream>
#include <iomanip>
#include <sys/stat.h>					// stat(.)
#include <dirent.h>						// DIR directory header	


#include "au/CommandLine.h"				// au::CommandLine
#include "au/string.h"					// au::Format
#include "au/Descriptors.h"             // au::Descriptors
#include "au/ThreadManager.h"
#include "au/containers/StringVector.h"

#include "au/tables/Tree.h"                    // au::tables::TreeItem
#include "au/tables/Table.h"

#include "au/log/LogToServer.h" 

#include "engine/MemoryManager.h"                   // samson::MemoryManager
#include "engine/Notification.h"                   // samson::Notification

#include "au/tables/pugi.h"                  // pugi::Pugi
#include "au/tables/pugixml.hpp"             // pugi:...

#include "au/log/log_server_common.h"

#include "samson/common/EnvironmentOperations.h"	// Environment operations (CopyFrom)
#include "samson/common/NotificationMessages.h"

#include "samson/network/Packet.h"						// ss:Packet

#include "samson/client/SamsonClient.h"

#include "samson/module/ModulesManager.h"           // samson::ModulesManager
#include "samson/module/samsonVersion.h"            // SAMSON_VERSION

#include "samson/stream/BlockManager.h"             // samson::stream::BlockManager

#include "samson/delilah/SamsonDataSet.h"
#include "samson/delilah/Delilah.h"					// samson::Delailh
#include "samson/delilah/DelilahConsole.h"			// Own interface
#include "samson/delilah/RepeatDelilahComponent.h"

#include "PushDelilahComponent.h"                   // samson::PushDelilahComponent
#include "PushDelilahComponent.h"                   // PushDataComponent
#include "PopDelilahComponent.h"


#define DEF1             "TYPE:EXEC/FUNC: TEXT"

extern size_t delilah_random_code;

namespace samson
{	
    const char* general_description = \
    "SAMSON is a distributed platform for efficient processing of unbounded streams of big data";
    
    const char* auths = "Andreu Urruela, Grant Croker, J.Gregorio Escalada & Ken Zangelin";
    
    DelilahConsole::DelilahConsole( ) : log_client( AU_LOG_SERVER_QUERY_PORT )
    {
        // Default values
        show_local_logs = false;
        show_server_logs = false;
        show_alerts = false;
        verbose = true;
        
        mode = mode_normal; // Normal mode by default

        // Schedule a notification to review repeat-tasks
        engine::Engine::shared()->notify( new engine::Notification( notification_delilah_review_repeat_tasks  ) , 1 );
        
        // Cool stuff
        addEspaceSequence( "samson" );
        addEspaceSequence( "q" );  // ls
        addEspaceSequence( "d" );  // Database mode...
        addEspaceSequence( "l" );  // logs mode...
        addEspaceSequence( "n" );  // normal mode...
        
        // By default no save traces
        trace_file = NULL;
        
        simple_output = false;
        no_output = false;
        
        // Aliases
        //add_alias( "aso" , "add_stream_operation" );

        // Inform about random code for this delilah
        writeWarningOnConsole(au::str("Random delilah id generated [%s]" , au::code64_str( delilah_random_code ).c_str()));
        
    }

    void DelilahConsole::add_alias( std::string key , std::string value )
    {
        aliases.insertInMap( key ,  value );
    }
    
    void DelilahConsole::remove_alias( std::string key)
    {
        if( aliases.isInMap(key) )
            aliases.extractFromMap(key);
    }
                                                          
    
    
    DelilahConsole::~DelilahConsole()
    {
    }
    
    std::string DelilahConsole::getPrompt()
    {
        if( mode == mode_database )
            return "Database >";

        if( mode == mode_logs )
            return log_client.getPrompt();
        
        return  au::str( "[%s] Delilah>" , getConnectionInformation().c_str() );
    }
	
	void DelilahConsole::evalCommand(std::string command)
	{
		size_t _delilah_id = runAsyncCommand(command);
        
        if( _delilah_id != 0 )
        {
            au::Cronometer cronometer;
            while( true )
            {
                au::ConsoleEntry entry;
                std::string message;
                if( cronometer.diffTimeInSeconds() > 1 )
                {
                    message = au::str("[ %s ] Waiting process %lu : %s ... [ b: background c: cancel ]" 
                                      , cronometer.strClock().c_str()
                                      , _delilah_id 
                                      , command.c_str()
                                      );
                }
                
                int s = waitWithMessage( message , 0.2 , &entry );
                
                if( !isActive( _delilah_id ) )
                {
                    // Print output
                    refresh();
                    writeOnConsole( getOutputForComponent(_delilah_id) );
                    return;
                }
                
                if ( s == 0 ) 
                {
                    // To something with the key
                    
                    if( entry.isChar( 'c' ) )
                    {
                        refresh();
                        writeWarningOnConsole( au::str("Canceling process %lu : %s" , _delilah_id , command.c_str() ));
                        cancelComponent( _delilah_id );
                        return;
                    }
                    else if( entry.isChar( 'b' ) )
                    {
                        refresh(); // Refresh console
                        return;
                    }
                }
            }
            
        }
        
	}

    void DelilahConsole::autoCompleteOperations( au::ConsoleAutoComplete* info )
    {
        std::vector<std::string> operation_names = getOperationNames();
        
        for ( size_t i = 0 ;  i < operation_names.size() ; i++)
            info->add( operation_names[i] );
    }

    void DelilahConsole::autoCompleteOperations( au::ConsoleAutoComplete* info , std::string type )
    {
        std::vector<std::string> operation_names = getOperationNames( type );
        
        for ( size_t i = 0 ;  i < operation_names.size() ; i++)
            info->add( operation_names[i] );
    }
    
    void DelilahConsole::autoCompleteQueues(au::ConsoleAutoComplete* info )
    {
        au::tables::Table* table = database.getTable("queues");
        if( !table )
            return;
        for ( size_t r = 0 ; r <  table->getNumRows() ; r++ )
            info->add(  table->getValue(r, "name") );
        delete table;
    }      
    
    void DelilahConsole::autoCompleteQueueWithFormat(
                                                     au::ConsoleAutoComplete* info  ,  
                                                     std::string key_format , 
                                                     std::string value_format 
                                                     )
    {
        au::tables::Table* table = database.getTable("queues");
        
        if( !table )
            return;
        
        for ( size_t r = 0 ; r <  table->getNumRows() ; r++ )
        {
            if( table->getValue(r, "format/key_format") == key_format )
                if( table->getValue(r, "format/value_format") == value_format )
                    info->add(  table->getValue(r, "name") );
        }
        
        delete table;
        
    }  
    
    void DelilahConsole::autoCompleteQueueForOperation( au::ConsoleAutoComplete* info , std::string operation_name , int argument_pos )
    {
        
        // Search in the operations
        Operation* operation = ModulesManager::shared()->getOperation(operation_name);
        if ( !operation )
            return;

        if( argument_pos < operation->getNumInputs() )
        {
            autoCompleteQueueWithFormat(
                                        info 
                                        , operation->inputFormats[argument_pos].keyFormat 
                                        , operation->inputFormats[argument_pos].valueFormat 
                                        );
        }
        else 
        {
            argument_pos -= operation->getNumInputs();
            if( argument_pos < operation->getNumOutputs() )
            {
                autoCompleteQueueWithFormat(
                                            info 
                                            , operation->outputFormats[argument_pos].keyFormat 
                                            , operation->outputFormats[argument_pos].valueFormat 
                                            );
            }
            
        }
        
    }
    
    
    void DelilahConsole::autoComplete( au::ConsoleAutoComplete* info )
    {
        
        // Common autocomplete
        if( info->completingFirstWord() )
            info->add("set_mode"); // always available
                                         
        
        if( info->completingSecondWord("set_mode") )
        {
            info->add("normal");
            info->add("database");
            info->add("logs");
        }
        
        if( mode == mode_database )
        {
            if ( info->completingFirstWord() )
                info->add("set_database_mode off");
            
            autoCompleteForDatabaseCommand( info );
            return;
        }
        
        if( mode == mode_logs )
        {
            return log_client.autoComplete(info);
        }
            
        if ( info->completingFirstWord() )
        {
            // Add console commands
            delilah_command_catalogue.autoComplete( info );
            
            return;
        }

        // Options for ls
        if (info->completingSecondWord("ls") )
        {
            info->add("-rates");
            info->add("-properties");
            info->add("-blocks");
            
            autoCompleteQueues( info );
        }

        // Options for ls_workers
        if (info->completingSecondWord("ls_workers") )
        {
            info->add("-engine");
            info->add("-disk");
        }
        
        if( info->firstWord() == "rm" )
        {
            autoCompleteQueues( info );
        }
        
        if( info->firstWord() == "push_queue" )
        {
            autoCompleteQueues( info );
        }
        
        if (info->completingSecondWord("ls_stream_operations") )
        {
            info->add("-properties");
            info->add("-running");
            info->add("-in");
            info->add("-out");
        }
        
        /*
        if (info->completingSecondWord("stop_repeat") )
        {
            // Add all ps repeat tasks...
            au::map<size_t , DelilahComponent>::iterator it_components;			
            for( it_components = components.begin() ; it_components != components.end() ; it_components++ )
            {
                DelilahComponent * component = it_components->second;
                if( component->type == DelilahComponent::repeat )
                    info->add(  au::str("%lu",component->id ) );
            }

            
        }
         */
        
        // Options for the cluster command
        if (info->completingSecondWord("cluster") )
        {
            info->add("info");
            info->add("pending");
            info->add("add");
            info->add("remove");
            info->add("connect");
            info->add("connections");
            info->add("get_my_id");
        }
        
        if (info->completingSecondWord("help") )
        {
            // Add console commands
            delilah_command_catalogue.autoComplete( info );
            
            // Independent option
            info->add("all");
            info->add("categories");
            info->add("-category"); // To help type 
            
            
            // Add categories as well...
            au::StringVector categories = delilah_command_catalogue.getCategories();
            for ( size_t i = 0 ; i < categories.size() ; i++ )
                info->add( categories[i] );
            
        }
        
        
        
        if (info->completingThirdWord("help","-category") )
        {
            au::StringVector categories = delilah_command_catalogue.getCategories();
            for ( size_t i = 0 ; i < categories.size() ; i++ )
                info->add( categories[i] );
        }
        
        if (info->completingSecondWord("run") )
        {
            // Add operations
            autoCompleteOperations( info );
            return;
        }
        
        
        // Suggest name of queues....
        if( info->firstWord() == "run" )
        {
            // Suggest queues... ( no format at the moment )
            autoCompleteQueueForOperation( info , info->secondWord() , info->completingWord() - 2 );
        }
        
        
        if (info->completingSecondWord( "init_stream" ) )
        {
            autoCompleteOperations( info , "script" );
            return;
        }
        
        if (info->completingThirdWord("init_stream","*") )
        {
            // Add operations
            autoCompleteOperations( info , "script" );
            return;
        }
        
        
        
        if (info->completingSecondWord( "add_stream_operation" ) )
        {
            info->setHelpMessage("Enter name of the stream operation...");
            return;
        }
        
        if (info->completingSecondWord( "repeat" ) )
        {
            // Add console commands
            delilah_command_catalogue.autoComplete( info );
            return;
        }
        
        if (info->completingThirdWord( "add_stream_operation" , "*" ) )
        {
            // Add operations
            autoCompleteOperations( info );
            return;
        }
        
        if (info->completingSecondWord("ls_operations") )
        {
            // Add operations
            autoCompleteOperations( info );
            return;
        }
        
        
        if (info->completingSecondWord("alerts") )
        {
            if (show_alerts)
                info->add("off");
            else
                info->add("on");
        }

        if (info->completingSecondWord("local_logs") )
        {
            info->add("off");
            info->add("on");
        }
        
        if (info->completingSecondWord("server_logs") )
        {
            info->add("off");
            info->add("on");
        }
        
        if (info->completingSecondWord("trace") )
        {
            info->add("off");
            info->add("on");
        }

        if (info->completingSecondWord("wtrace") )
        {
            info->add("off");
            info->add("set");
            info->add("get");
            info->add("add");
            info->add("remove");
        }

        if (info->completingSecondWord("wverbose") )
        {
            info->add("off");
            info->add("0");
            info->add("1");
            info->add("2");
            info->add("3");
            info->add("4");
            info->add("5");
            info->add("get");
        }

        if (info->completingSecondWord("wdebug") )
        {
            info->add("on");
            info->add("off");
        }

        if (info->completingSecondWord("wreads") )
        {
            info->add("on");
            info->add("off");
        }

        if (info->completingSecondWord("wwrites") )
        {
            info->add("on");
            info->add("off");
        }

        if (info->completingSecondWord("log"))
        {
            info->add("FileName=");
            info->add("Type=");
        }

        if (info->completingSecondWord("verbose") )
        {
            if (verbose)
                info->add("off");
            else
                info->add("on");
        }
        
        
        if (info->completingSecondWord("set_database_mode") )
            info->add("on"); // It can only be on
        

        if ( info->completingSecondWord("push_module") )
        {
            info->auto_complete_files("");
            return;
        }
        
        
        // Push operation
        // ------------------------------------------------------------------------
        if ( info->completingSecondWord("push") )
        {
            info->auto_complete_files("");
            return;
        }
        if (info->completingThirdWord( "push" , "*" ) )
        {
            autoCompleteQueues( info );
            return;
        }
        // ------------------------------------------------------------------------
        
        
        // Pop operation
        // ------------------------------------------------------------------------
        
        if ( info->completingSecondWord("pop") )
        {
            autoCompleteQueues( info );
            return;
        }
        
        
    }
    
    void DelilahConsole::run()
    {
        
        // If command-file is provided
        if ( commandFileName.length() > 0 )
        {
            FILE *f = fopen( commandFileName.c_str() , "r" );
            if( !f )
            {
                LM_E(("Error opening commands file %s", commandFileName.c_str()));
                exit(0);
            }
            
            char line[1024];
            
            //LM_M(("Processing commands file %s", commandFileName ));
            while( fgets(line, sizeof(line), f) )
            {
                // Remove the last return of a string
                while( ( strlen( line ) > 0 ) && ( line[ strlen(line)-1] == '\n') > 0 )
                    line[ strlen(line)-1]= '\0';
                
                //LM_M(("Processing line: %s", line ));
                size_t id = runAsyncCommand( line );
                
                if( id != 0)
                {
                    //LM_M(("Waiting until delilah-component %ul finish", id ));
                    // Wait until this operation is finished
                    while ( isActive( id ) )
                        sleep(1);
                }
            }
            
            fclose(f);
            
            return;
        }
        else
            runConsole();
    }
    /*
     // Generic list of information from the xml document
     std::string generic_node_to_string_function( const pugi::xml_node& node )
     {
     std::ostringstream output;
     pugi::str( node , 0 ,  output , 1000 );
     return output.str();
     }
     */
    std::string string_for_list( const char* list[] )
    {
        std::ostringstream output; 
        int i = 0;
        while( list[i] != NULL )
        {
            output << list[i];;
            if( list[i+1] != NULL )
            {
                if( ((i+1)%4) == 0 )
                    output << "\n";
                else
                    output << ", ";
            }
            i++;
        }
        output << "\n";
        
        return output.str();
    }
    
    void add( const char* list[] , std::vector<std::string> &commands )
    {
        int i = 0;
        while( list[i] != NULL )
        {
            commands.push_back( list[i] );
            i++;
        }
        
    }
    
    size_t DelilahConsole::runAsyncCommand( std::string command )
	{
		
        // Change if command is in alias
        if( aliases.isInMap(command) )
            command = aliases.findInMap(command);
        
		au::CommandLine commandLine;
		commandLine.set_flag_string("user", "anonymous");
		commandLine.set_flag_string("password", "anonymous");
        commandLine.set_flag_int("port", SAMSON_WORKER_PORT); // Default port for SAMSON
        
		commandLine.set_flag_string("name", "null");
		commandLine.set_flag_string("begin", "null");           
		commandLine.set_flag_boolean("plain");              // Flag to indicate plain ( not used ) 
		commandLine.set_flag_boolean("gz");                 // Flag to indicate compression ( not used )
		commandLine.set_flag_int("threads",4);              // Specify number of threads ( not used )
        commandLine.set_flag_boolean("force");              // Force to remove directory if exist before in pop operations
        commandLine.set_flag_boolean("show");               // Show data after poping data
        commandLine.set_flag_boolean("clear");              // Used in the ps command
        commandLine.set_flag_boolean("header");             // Used in the show_local_queue command
        commandLine.set_flag_int("limit", 10);
		commandLine.set_flag_string("category", "");
		commandLine.parse( command );
        
        
		if( commandLine.get_num_arguments() == 0)
			return 0;	// Zero means no pending operation to check

        std::string mainCommand = commandLine.get_argument(0);
        
        LM_M(("runAsyncCommand command:%s", command.c_str()));

        // Common command in all modes
        if( mainCommand == "set_mode" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole("Usage: set_mode normal/database/logs\n");
                return 0;
            }
            
            if( commandLine.get_argument(1) == "normal" )
            {
                mode = mode_normal;
                writeWarningOnConsole("Normal mode activated\n");
            } 
            else if( commandLine.get_argument(1) == "database" )
            {
                mode = mode_database;
                writeWarningOnConsole("Database mode activated\n");
            }
            else if( commandLine.get_argument(1) == "logs" )
            {
                mode = mode_logs;
                writeWarningOnConsole("logs mode activated\n");
            }
            else
                writeErrorOnConsole("Usage: set_mode normal/database/logs\n");
            
            return 0;
        }
        
        // Spetial mode
        if( mode == mode_logs )
        {
            au::ErrorManager error;
            log_client.evalCommand(command, &error);
            write( &error ); // Console method to write all the answers
            return 0;
        }
        
        if( mode == mode_database )
        {
            // Run data base command
            std::string result = runDatabaseCommand(command);
            writeOnConsole( au::strToConsole(result) );
            return 0;
        }

        
        if( mainCommand == "connect" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                showErrorMessage("Usage: connect host [-port X] [-user X] [-password X]");
                return 0;
            }
            
            std::string host = commandLine.get_argument(1);
            std::string user = commandLine.get_flag_string("user");
            std::string password = commandLine.get_flag_string("password");
            int port = commandLine.get_flag_int("port");

            // Show message on console
            showMessage( au::str("Connecting to %s:%d (user %s)..." , host.c_str() , port , user.c_str() ));

            // Connect to this host if possible
            connect(host, port, user, password);
            
            return 0;
        }

        if( mainCommand == "disconnect" )
        {
            au::ErrorManager error;
            delilah_disconnect( &error );
            
            if( error.isActivated() )
                write( &error );
            else
                writeWarningOnConsole("OK");
                
            return 0;
        }
        

        
        if( mainCommand == "history" )
        {
            int limit = 0;
            
            if( commandLine.get_num_arguments() > 1 )
                limit = ::atoi( commandLine.get_argument(1).c_str() );
            showMessage( str_history(limit) );
            return 0;
        }
        
        if( mainCommand == "reload_modules" )
        {
            ModulesManager::shared()->reloadModules();
            writeWarningOnConsole("Modules at delilah client have been reloaded.");
        }
        
        if ( mainCommand == "cluster" )
        {
            // Interact with the network layer
            au::ErrorManager error;
            std::string message = runClusterCommand( command , &error );
            
            if( error.isActivated() )
                writeErrorOnConsole( error.getMessage() );
            else
                writeOnConsole( message + "\n" );

            return 0;
        }
        
		if ( commandLine.isArgumentValue(0,"help","h") )
		{
			
            std::string category = commandLine.get_flag_string("category");
            if ( category != "" )
            {
                writeOnConsole( delilah_command_catalogue.getCommandsTable(category) );                    
                return 0;
            }
            
            if( commandLine.get_num_arguments() == 1 )
            {
                
                std::ostringstream output;
                output << "\n";
                output << au::lineInConsole('=') << "\n";
                output << " SAMSON v " << SAMSON_VERSION << "\n";
                output << au::lineInConsole('=') << "\n";
                output << "\n";
                output << au::str_indent( general_description ) << "\n";
                output << "\n";
                output << au::str_indent( au::str("Authors: %s", auths ) ) << "\n";
                output << "\n";
                output << au::str_indent( au::str("Telefonica I+D 2010-2012" ) ) << "\n";
                output << "\n";
                output << au::lineInConsole('-') << "\n";
                output << "\n";
                output << "\thelp all .................. get a list of all available commands\n";
                output << "\thelp categories ........... get a list of command categories\n";
                output << "\thelp <command> ............ get detailed information for a command\n";
                output << "\thelp <category> ........... get list of commands for a particular categoriy\n";
                output << "\n";
                output << au::lineInConsole('-') << "\n";
                output << "\n";
                
                
                std::string text = output.str();
                
                writeOnConsole( text );
                return 0;
            }
            else
            {
                
                std::string command = commandLine.get_argument(1);
                
                if ( command == "all" )
                    writeOnConsole( delilah_command_catalogue.getCommandsTable() );                    
                else
                    writeOnConsole( delilah_command_catalogue.getHelpForCommand(command) );
                
                return 0;
            }
            
		}
        
        if ( commandLine.isArgumentValue(0, "quit", "") )
        {
            Console::quitConsole();	// Quit the console
            return 0;
        }

        if ( commandLine.isArgumentValue(0, "threads", "") )
        {
            writeOnConsole( au::ThreadManager::shared()->str() );
            return 0;
        }
        
        
        if ( mainCommand == "set")
        {
            if ( commandLine.get_num_arguments() == 1)
            {
                // Only set, we show all the defined parameters
                au::tables::Table table( au::StringVector("Property" , "Value") );
                table.setTitle("Environent variables");
                
                std::map<std::string,std::string>::iterator it_environment;	
                for( it_environment = environment.environment.begin() 
                    ; it_environment != environment.environment.end() 
                    ; it_environment++ )
                {
                    table.addRow( au::StringVector( it_environment->first , it_environment->second ) );
                }
                
                writeOnConsole( table.str( ) );
                return 0;
			}
			
			if ( commandLine.get_num_arguments() < 3 )
			{
				writeErrorOnConsole("Usage: set name value.\n");
				return 0;
			}
			
			// Set a particular value
			std::string name = commandLine.get_argument(1);
			std::string value = commandLine.get_argument(2);
			
			environment.set( name , value );
			std::ostringstream o;
			o << "Environment variable " << name << " set to " << value << "\n";
			writeOnConsole( o.str() );
			return 0;
        }
        
        if ( mainCommand == "unset")
        {
            if ( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole("Usage: unset name.\n");
                return 0;
            }
            
            // Set a particular value
            std::string name = commandLine.get_argument(1);
            
            if ( !environment.isSet(name ) )
            {
                writeWarningOnConsole( au::str("Variable %s is not set", name.c_str() ) );
                return 0;
            }
            
            environment.unset( name );
            
            std::ostringstream o;
            o << "Environment variable " << name << " removed\n";
            writeOnConsole( o.str() );
            
            return 0;
        }	

        if( mainCommand == "example_warning" )
        {
            LM_W(("This is just an example of warning"));
            return 0;
        }
        
        if( mainCommand == "local_logs" )
        {
            if ( commandLine.get_num_arguments() == 1)
            {
                if( show_local_logs )
                    writeOnConsole( "Local logs are activated\n" );
                else
                    writeOnConsole( "Local logs are NOT activated\n" );
                return 0;
            }
            
            if( commandLine.get_argument(1) == "on" )
            {
                if( !show_local_logs )
                {
                    // Connect plugin
                    add_log_plugin( this );
                }
                
                show_local_logs = true;
                writeOnConsole( "Local logs are now activated\n" );
                return 0;
            }
            if( commandLine.get_argument(1) == "off" )
            {
                if( show_local_logs )
                {
                    // Disconnec plugin
                    remove_log_plugin( this );
                }
                
                show_local_logs = false;
                writeOnConsole( "Local logs are now NOT activated\n" );
                return 0;
            }
            
            writeErrorOnConsole("Usage: alerts on/off\n");
            return 0;
            
        }

        
        if ( mainCommand == "alerts")
        {
            
            if ( commandLine.get_num_arguments() == 1)
            {
                if( show_alerts )
                    writeOnConsole( "Alerts are activated\n" );
                else
                    writeOnConsole( "Alerts are NOT activated\n" );
                return 0;
            }
            
            if( commandLine.get_argument(1) == "on" )
            {
                show_alerts = true;
                writeOnConsole( "Alerts are now activated\n" );
                return 0;
            }
            if( commandLine.get_argument(1) == "off" )
            {
                show_alerts = false;
                writeOnConsole( "Alerts are now NOT activated\n" );
                return 0;
            }
            
            writeErrorOnConsole("Usage: alerts on/off\n");
            return 0;
        }
        
        if ( mainCommand == "verbose")
        {
            
            if ( commandLine.get_num_arguments() == 1)
            {
                if( verbose )
                    writeOnConsole( "verbose mode is activated\n" );
                else
                    writeOnConsole( "verbose mode is NOT activated\n" );
                return 0;
            }
            
            if( commandLine.get_argument(1) == "on" )
            {
                verbose = true;
                writeOnConsole( "verbose mode is now activated\n" );
                return 0;
            }
            if( commandLine.get_argument(1) == "off" )
            {
                verbose = false;
                writeOnConsole( "verbose mode is now NOT activated\n" );
                return 0;
            }
            
            writeErrorOnConsole("Usage: verbose on/off\n");
            return 0;
        }
        
        if( mainCommand == "show_alerts" )
        {
            std::string txt = trace_colleciton.str();
            writeOnConsole( au::strToConsole( txt ) );
            return 0;
        }
        
        if ( mainCommand == "open_alerts_file" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole("USAGE: open_alerts_file local_file ");
                return 0;
            }
            
            if( trace_file )
            {
                writeErrorOnConsole(
                au::str("Please close previous alerts file (%s) with command 'close_alerts_file'\n",trace_file_name.c_str())
                                    );
                return 0;
            }
            
            trace_file_name = commandLine.get_argument(1);
            trace_file = fopen(trace_file_name.c_str(), "w");
            if ( !trace_file )
            {
                writeErrorOnConsole( au::str("Error opening file '%s' to store alerts (%s)\n"
                                             ,trace_file_name.c_str()
                                             , strerror(errno) )
                                    );
                return 0;
            }

            writeOnConsole(au::str("Saving alerts to file '%s'\n" , trace_file_name.c_str()));
            return 0;
        }
        
        if ( mainCommand == "close_alerts_file" )
        {
            if( !trace_file )
            {
                writeErrorOnConsole("There is no opened alerts file. Open one with command 'open_alerts_file'\n");
                return 0;
            }
            
            fclose(trace_file);
            writeOnConsole("Stop saving alerts to file '%s'.\nRemeber you can open a new alerts file with command 'open_alerts_file\n'");
            return 0;
        }
        
        if ( mainCommand == "clear_components" )
        {
            // Clear completed upload and download process
            clearComponents();
            
            writeOnConsole("Clear components OK");
            
            return 0;
        }
        
        
        if ( mainCommand == "ps" )
        {
            
            if( commandLine.get_flag_bool("clear") )
                clearComponents();
            
            if( commandLine.get_num_arguments() > 1 )
            {
                size_t id = atoll( commandLine.get_argument(1).c_str() );
                
                DelilahComponent *component = components.findInMap( id );
                if( !component )
                    writeErrorOnConsole( au::str("Unknown process with id %d", id ) );
                else
                {
                    std::ostringstream output;
                    output << "================================================\n";
                    output << " Process " << au::code64_str( delilah_random_code ) << "_" << id << " ";
                    
                    if( component->isComponentFinished() )
                        output << "FINISHED "  << component->cronometer.str();
                        else
                            output << " RUNNING " << component->cronometer.str();
                    output << "\n";
                    
                    output << "================================================\n";

                    if( component->error.isActivated() )
                    {
                        output << "ERROR: " << component->error.getMessage() << "\n";
                        output << "================================================\n";
                    }
                    output << component->getStatus();
                    
                    writeOnConsole(output.str());
                    
                }
                
                return 0;
            }
            
            std::string txt = getListOfComponents();
            writeOnConsole( au::strToConsole( txt ) );
            return 0;
            
        }
        
        /*
        if( mainCommand == "stop_repeat" )
        {
            
            if ( commandLine.get_num_arguments() < 2 )
            {
                stop_all_repeat();
                // Stop all repeated operations
                writeOnConsole("All repeat operations stoped");
            }
            else
            {
                size_t id_process = ::atoll( commandLine.get_argument(1).c_str() );
                Status s = stop_repeat( id_process );
                if( s == OK )
                    writeOnConsole( au::str("Stopped repeate process %lu" , id_process) );
                else
                    writeErrorOnConsole( au::str("Problems stopping repeate process %lu" , id_process) );
                    
                
            }
            
            
            
            return 0;
        }
         
        if( mainCommand == "repeat" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole("Usage: repeat command [ops]");
                return 0;
            }
            
            std::string repeat_command = commandLine.get_argument(1);
            if( !delilah_command_catalogue.isValidCommand( repeat_command ) )
            {
                writeErrorOnConsole( 
                    au::str("Not possible to repeat '%s' since it is not a valid command", repeat_command.c_str())
                                    );
                return 0;
            }
            
            size_t pos = command.find("repeat");
            if( pos != std::string::npos )
            {
                std::string repeat_command = command.substr( pos+6 , std::string::npos );

                // Check the main command exist
                std::string main_command_to_repeat = commandLine.get_argument(1);
                
                
                RepeatDelilahComponent* component =  new RepeatDelilahComponent( repeat_command , 2 );
                size_t id = addComponent( component );
                component->run();
                return id;
                
            }
            else
                LM_W(("Strange behaviour with repeat command..."));
            
            return 0;
            
        }
        */
         
         
        // Push data to a queue
        
        if( mainCommand == "push" )
        {
            if( commandLine.get_num_arguments() < 3 )
            {
                writeErrorOnConsole("Usage: push file <file2> .... <fileN> queue1,queue2,queue3\n");
                return 0;
            }
            
            std::vector<std::string> fileNames;
            for (int i = 1 ; i < (commandLine.get_num_arguments()-1) ; i++)
            {
                std::string fileName = commandLine.get_argument(i);
                
                struct stat buf;
                stat( fileName.c_str() , &buf );
                
                if( S_ISREG(buf.st_mode) )
                {
                    if( verbose )
                    {
                        
                        std::ostringstream message;
                        message << "Including regular file " << fileName << " with " <<  au::str( (size_t) buf.st_size ) <<" Bytes\n";
                        showMessage( message.str() );
                    }
                    
                    fileNames.push_back( fileName );
                }
                else if ( S_ISDIR(buf.st_mode) )
                {
                    if( verbose )
                    {
                        std::ostringstream message;
                        message << "Including directory " << fileName << "\n";
                        writeOnConsole( message.str() );
                    }
                    
                    {
                        // first off, we need to create a pointer to a directory
                        DIR *pdir = opendir (fileName.c_str()); // "." will refer to the current directory
                        struct dirent *pent = NULL;
                        if (pdir != NULL) // if pdir wasn't initialised correctly
                        {
                            while ((pent = readdir (pdir))) // while there is still something in the directory to list
                                if (pent != NULL)
                                {
                                    std::ostringstream localFileName;
                                    localFileName << fileName << "/" << pent->d_name;
                                    
                                    struct stat buf2;
                                    stat( localFileName.str().c_str() , &buf2 );
                                    
                                    if( S_ISREG(buf2.st_mode) )
                                        fileNames.push_back( localFileName.str() );
                                    
                                }
                            // finally, let's close the directory
                            closedir (pdir);						
                        }
                    }
                } 
                else
                {
                    if( verbose )
                    {
                        std::ostringstream message;
                        message << "Skipping " << fileName;
                        writeOnConsole( message.str() );
                    }
                }
            }
            
            std::string queues_txt = commandLine.get_argument( commandLine.get_num_arguments()-1 );
            
            std::vector<std::string> queues = au::split( queues_txt , ',' );
            
            size_t id = addPushData(fileNames, queues );
            
            return id;
        }
        
        
        // Push data to a queue
        
        if( mainCommand == "pop" )
        {
            if( commandLine.get_num_arguments() < 3 )
            {
                writeErrorOnConsole("Usage: pop queue fileName\n");
                return 0;
            }
            
            std::string queue_name  = commandLine.get_argument(1);
            std::string fileName    = commandLine.get_argument(2);
            
            bool force_flag = commandLine.get_flag_bool("force");
            bool show_flag = commandLine.get_flag_bool("show");
            
            size_t id = addPopData( queue_name ,  fileName , force_flag , show_flag );
            
            return id;
        }
        
        
        // WorkerCommands
        std::string main_command = commandLine.get_argument(0);
        
       
        // Some checks for some operations
        if( main_command == "rm" )
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole( au::str("Usage: rm queue\n" ) );
                return 0;
            }
        
        // Command to remove queues 
        if( main_command == "set_queue_property" )
            if( commandLine.get_num_arguments() < 4 )
            {
                writeErrorOnConsole( au::str("Usage: set_queue_propert queue property value\n" ) );
                return 0;
            }

        if( main_command == "unset_queue_property" )
            if( commandLine.get_num_arguments() < 3 )
            {
                writeErrorOnConsole( au::str("Usage: unset_queue_propert queue property\n" ) );
                return 0;
            }
        
        
        // Command to remove queues 
        if( main_command == "push_queue" )
            if( commandLine.get_num_arguments() < 3 )
            {
                writeErrorOnConsole( au::str("Usage: push_queue form_queue to_queue" ) );
                return 0;
            }
        
        // Command to play / pause statess
        if( main_command == "pause_queue" )
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole( au::str("Usage: pause_queue queue\n" ) );
                return 0;
            }
        
        if( main_command == "play_queue" )
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole( au::str("Usage: play_queue queue" ) );
                return 0;
            }
        
        if( mainCommand == "times" )
        {
            writeOnConsole( updateTimeString() );
            return 0;
        }
        
        if( main_command == "rm_local" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole( "Usage: rm_local <dir>");
                return 0;
            }
            
            au::ErrorManager error;
            au::removeDirectory( commandLine.get_argument(1) , error );
            
            if( error.isActivated() )
                writeErrorOnConsole( error.getMessage() );
            else
                writeWarningOnConsole("OK");
            return 0;
        }
        
        if( mainCommand == "ls_local" )
        {
            std::string pattern ="*";
            if( commandLine.get_num_arguments() > 1 )
                pattern = commandLine.get_argument(1);
            
            writeOnConsole( getLsLocal( pattern , false ) ); 
            return 0;
        }

        if( mainCommand == "ls_local_queues" )
        {
            std::string pattern ="*";
            if( commandLine.get_num_arguments() > 1 )
                pattern = commandLine.get_argument(1);
            
            writeOnConsole( getLsLocal( pattern , true ) ); 
            return 0;
        }
        
        
        if ( mainCommand == "show_local_queue" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole("Usage:show_local_queue <local_dir>\n");
                return 0;
            }

            const char *file_name = commandLine.get_argument(1).c_str();
            int limit = commandLine.get_flag_int("limit" );
            
            struct stat filestatus;
            stat( file_name , &filestatus );
            
            if ( S_ISREG( filestatus.st_mode ) )
            {
                // Open a single file
                samson::SamsonFile samsonFile( file_name );
                
                if( samsonFile.hasError() )
                {
                    writeErrorOnConsole( au::str("%s", samsonFile.getErrorMessage().c_str() ) );
                    return 0;
                }
                
                if( commandLine.get_flag_bool("header") )
                {
                    std::ostringstream output;
                    output << samsonFile.header.str() << " " << " [ " << samsonFile.header.info.kvs << " kvs in " << samsonFile.header.info.size << "bytes\n";  
                    writeOnConsole(output.str());
                    return 0;
                }
                
                std::ostringstream output;
                samsonFile.printContent( limit , output );
                std::string txt =  output.str();
                writeOnConsole( txt );
                return 0;
            }
            else if( S_ISDIR( filestatus.st_mode ) )
            {
                samson::SamsonDataSet samsonDataSet( file_name );
                
                if( samsonDataSet.error.isActivated() )
                {
                    writeErrorOnConsole( au::str("%s", samsonDataSet.error.getMessage().c_str() ) );
                    return 0;
                }

                if( commandLine.get_flag_bool("header") )
                {
                    std::ostringstream output;
                    output << "Total: " << samsonDataSet.info.strDetailed() << "\n"; 
                    samsonDataSet.printHeaders(output);
                    std::string txt =  output.str();
                    writeOnConsole( txt );
                    return 0;
                }

                
                std::ostringstream output;
                samsonDataSet.printContent( limit , output );
                std::string txt =  output.str();
                writeOnConsole( txt );
                return 0;
            } 
            else
            {
                writeErrorOnConsole( au::str("%s is not a file or a directory\n",file_name) );
                return 0;
            }

            // Not possible to get here...
            return 0;
        }
        
        if( mainCommand == "push_module" )
        {
            
            if( commandLine.get_num_arguments() < 3 )
            {
                writeErrorOnConsole( "Usage: push_module <file> <module_name>");
                return 0;
            }

            std::string file_name = commandLine.get_argument(1);
            std::string module_name = commandLine.get_argument(2);
            
            struct ::stat info;
            if( stat(file_name.c_str(), &info) != 0 )
            {
                writeErrorOnConsole( au::str("Error reading file %s (%s)" , file_name.c_str() , strerror(errno) ) );
                return 0;
            }
            // Size of the file
            size_t file_size = info.st_size;
            engine::Buffer * buffer = engine::MemoryManager::shared()->createBuffer("push_module" , "delilah", file_size );
            buffer->setSize(file_size);
            
            // Load the file
            FILE* file = fopen( file_name.c_str(), "r");
            if( fread(buffer->getData(), file_size, 1, file) != 1 )
                LM_W(("Errro reading file %s" , file_name.c_str() ));
            fclose(file);
            
            size_t tmp_id = sendWorkerCommand( au::str("push_module %s" , module_name.c_str() ) , buffer );
            
            // Release the buffer we have just created
            buffer->release();
            
            return tmp_id;
        }
        
        
        // By default, it is considered a worker command
        return sendWorkerCommand( command , NULL );
        return 0;
    }
    
    int DelilahConsole::_receive( Packet* packet )
    {
        switch ( packet->msgCode ) 
        {
            case Message::Alert:
            {
                
                std::string _text     = packet->message->alert().text();
                std::string _type     = packet->message->alert().type();
                std::string _context  = packet->message->alert().context();
            
                // Add to the local collection of traces
                trace_colleciton.add( packet->from, _type, _context, _text);                

                // Write to disk if required
                if( trace_file )
                {
                    std::string trace_message =  au::str("%s %s %s %s\n" 
                                                         , packet->from.str().c_str()
                                                         , _type.c_str() 
                                                         , _context.c_str() 
                                                         , _text.c_str() 
                                                         );
                    fwrite(trace_message.c_str(), trace_message.length(), 1, trace_file);
                }
                
                if( show_alerts )
                {
                    
                    au::tables::Table table( "Concept|Value,left" );
                    table.setTitle("ALERT");
                    
                    table.addRow( au::StringVector( "From" , packet->from.str() ) );
                    table.addRow( au::StringVector( "Type" , _type ) );
                    table.addRow( au::StringVector( "Context" , _context ) );
                    table.addRow( au::StringVector( "Message" , _text ) );

                    std::string trace_message =table.str();
                                            
                    if( _type == "error" )
                        writeErrorOnConsole( trace_message );
                    else if( _type == "warning" )
                        writeWarningOnConsole( trace_message );
                    else
                        writeOnConsole( trace_message );
                }
                
            }
                break;
                
            default:
                LM_X(1, ("Unknown packet received at delilahConsole"));
                break;
        }
        
        
        return 0;
    }	
    
    void DelilahConsole::delilahComponentStartNotification( DelilahComponent *component )
    {
        if ( component->hidden )
            return; // No notification for hidden processes

        if( verbose )
        {
            std::ostringstream o;
            
            o << "Process started: " << au::code64_str(delilah_random_code) << "_" <<  component->getId() << " " << component->getConcept() << "\n";
            if( component->error.isActivated() )
                showErrorMessage( o.str() );        
            else
                showWarningMessage( o.str() );        
        }
    }
    
    void DelilahConsole::delilahComponentFinishNotification( DelilahComponent *component )
    {
        if ( component->hidden )
            return; // No notification for hidden processes
        
        if( verbose )
        {
            if( !component->error.isActivated() )
                showWarningMessage( au::str( "Process finished: %s_%lu %s\n" 
                                               , au::code64_str(delilah_random_code).c_str()
                                               , component->getId()
                                               , component->getConcept().c_str() ) );
            else
            {
                showErrorMessage( au::str( "Process finished with error: %s_%lu %s\nERROR: %s\n" 
                                             , au::code64_str(delilah_random_code).c_str()
                                             , component->getId()
                                             , component->getConcept().c_str()
                                             , component->error.getMessage().c_str()
                                             ) 
                                    );
            }
        }
        
        
        // Extra work
        if ( component->type == DelilahComponent::pop )
        {
            
            PopDelilahComponent* popComponent = (PopDelilahComponent*) component;
            if ( popComponent->show_flag )
            {
                // Add a command to show this conetnt
                runAsyncCommand( au::str("show_local_queue %s" , popComponent->fileName.c_str() ) );
            }
            
        }
        
    }
    

    // Process received packets with data
    
    void DelilahConsole::receive_buffer_from_queue( std::string queue , engine::Buffer* buffer )
    {
        size_t counter = stream_out_queue_counters.appendAndGetCounterFor( queue );
        size_t packet_size = buffer->getSize();
        
        std::string directory_name = au::str("stream_out_%s" , queue.c_str() );
        
        if( ( mkdir( directory_name.c_str() , 0755 ) != 0 ) && ( errno != EEXIST ) )
        {
            showErrorMessage(au::str("It was not possible to create directory %s to store data from queue %s" , directory_name.c_str() , queue.c_str() ));
            showErrorMessage(au::str("Rejecting a %s data from queue %s" 
                                     , au::str(packet_size,"B").c_str()
                                     , queue.c_str() ));
            return;
        }
        
        std::string fileName = au::str( "%s/block_%l05u" , directory_name.c_str() , counter );

        if (verbose)
        {
            
            // Show the first line or key-value
            SamsonClientBlock samson_client_block( buffer );  // Not remove buffer at destrutor
            
            std::ostringstream output;
            output << "====================================================================\n";
            output << au::str("Received stream data for queue %s\n" , queue.c_str() ); 
            output << au::str("Stored at file %s\n" , fileName.c_str() );
            output << samson_client_block.get_header_content();
            output << "====================================================================\n";
            output << samson_client_block.get_content( 5 );
            output << "====================================================================\n";
            
            showMessage( output.str() );
        }
        
        
        // Disk operation....
        engine::DiskOperation* operation = engine::DiskOperation::newWriteOperation( buffer ,  fileName , getEngineId()  );
        engine::DiskManager::shared()->add( operation );        
        operation->release();
    }
    
    void DelilahConsole::runAsyncCommandAndWait( std::string command )
    {
        LM_M(("runAsyncCommandAndWait command:%s", command.c_str()));
        size_t tmp_id = runAsyncCommand(command);
        
        if( tmp_id == 0 )
            return; // Sync command
        
        while( true )
        {
            if ( !isActive(tmp_id) )
                return;
            usleep(10000);
        }
    }

    
    
}
