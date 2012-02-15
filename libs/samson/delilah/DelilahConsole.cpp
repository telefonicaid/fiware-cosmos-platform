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

#include "tables/Tree.h"                    // au::tables::TreeItem

#include "engine/MemoryManager.h"                   // samson::MemoryManager
#include "engine/Notification.h"                   // samson::Notification

#include "tables/pugi.h"                  // pugi::Pugi
#include "tables/pugixml.hpp"             // pugi:...

#include "samson/common/Info.h"                     // samson::Info
#include "samson/common/EnvironmentOperations.h"	// Environment operations (CopyFrom)
#include "samson/common/NotificationMessages.h"

#include "samson/network/Packet.h"						// ss:Packet

#include "samson/module/ModulesManager.h"           // samson::ModulesManager
#include "samson/module/samsonVersion.h"            // SAMSON_VERSION

#include "samson/stream/BlockManager.h"             // samson::stream::BlockManager

#include "samson/delilah/SamsonDataSet.h"
#include "samson/delilah/Delilah.h"					// samson::Delailh
#include "samson/delilah/DelilahConsole.h"			// Own interface
#include "UploadDelilahComponent.h"                 // samson::DelilahLoadDataProcess
#include "DownloadDelilahComponent.h"               // samson::DelilahLoadDataProcess
#include "PushDelilahComponent.h"                   // samson::PushDelilahComponent
#include "PushDelilahComponent.h"                   // PushDataComponent


#define DEF1             "TYPE:EXEC/FUNC: TEXT"

namespace samson
{	
    const char* general_description = \
    "SAMSON is a distributed platform for efficient processing of unbounded streams of big data";
    
    const char* auths = "Andreu Urruela , Gregorio Sardina & Ken Zangelin";
    
    DelilahConsole::DelilahConsole( NetworkInterface *network ) : Delilah( network )
    {
        trace_on = false;
        
        database_mode = false;  // By default we are not in this mode
        
        // Schedule a notification to review repeat-tasks
        engine::Engine::shared()->notify( new engine::Notification( notification_delilah_review_repeat_tasks  ) , 1 );
        
        // Cool stuff
        addEspaceSequence( "samson" );
        addEspaceSequence( "d" );  // Data base mode...
    }
    
    DelilahConsole::~DelilahConsole()
    {
    }
    
    std::string DelilahConsole::getPrompt()
    {
        
        if( database_mode )
            return "Database >";
        
        int t = getUpdateSeconds();
        if ( t > 10 )
            return au::str("[%s] Delilah>", au::time_string(t).c_str() );
        else
            return  "Delilah>";
    }
	
	void DelilahConsole::evalCommand(std::string command)
	{
		runAsyncCommand(command);
	}

    void autoCompleteOperations( au::ConsoleAutoComplete* info )
    {
        if( global_delilah )
        {
            std::vector<std::string> operation_names = global_delilah->getOperationNames();
            
            for ( size_t i = 0 ;  i < operation_names.size() ; i++)
                info->add( operation_names[i] );
        }
    }

    void autoCompleteOperations( au::ConsoleAutoComplete* info , std::string type )
    {
        if( global_delilah )
        {
            std::vector<std::string> operation_names = global_delilah->getOperationNames( type );
            
            for ( size_t i = 0 ;  i < operation_names.size() ; i++)
                info->add( operation_names[i] );
        }
    }
    
    
    void autoCompleteQueues( au::ConsoleAutoComplete* info )
    {
        if( global_delilah) 
        {
            std::vector<std::string> queue_names = global_delilah->getQueueNames();
            
            for ( size_t i = 0 ;  i < queue_names.size() ; i++)
                info->add( queue_names[i] );
        }
    }        
    
    void autoCompleteDataSets( au::ConsoleAutoComplete* info  )
    {
        if( global_delilah ) 
        {
            std::vector<std::string> queue_names = global_delilah->getDataSetsNames();
            
            for ( size_t i = 0 ;  i < queue_names.size() ; i++)
                info->add( queue_names[i] );
        }
    }        
    
    void autoCompleteQueueWithFormat(au::ConsoleAutoComplete* info  ,  std::string key_format , std::string value_format )
    {
        /*
         au::TokenTaker tt( &token_xml_info );
         std::string c = au::str( "//controller//queue[format/key_format=\"%s\"][format/value_format=\"%s\"]" 
         , key_format.c_str() , value_format.c_str() ); 
         
         std::vector<std::string> queue_names = pugi::values( doc , c );
         
         for ( size_t i = 0 ;  i < queue_names.size() ; i++)
         addOption( queue_names[i] );
         */
    }  
    
    void autoCompleteQueueForOperation( au::ConsoleAutoComplete* info , std::string mainCommand , int argument_pos )
    {
        /*
         pugi::xpath_node_set node_set;
         
         {
         au::TokenTaker tt( &token_xml_info );
         std::string c = au::str( "//controller//operation[name=\"%s\"]/input_formats/format[%d]" ,  mainCommand.c_str() , argument_pos+1 );
         node_set = pugi::select_nodes(doc, c );
         }
         
         if( node_set.size() > 0 )
         {
         std::string key_format = pugi::get( node_set[0].node() , "key_format" );
         std::string value_format = pugi::get( node_set[0].node() , "value_format" );
         
         addQueueOptions(key_format, value_format);
         } 
         */
    }
    
    
    void DelilahConsole::autoComplete( au::ConsoleAutoComplete* info )
    {
        if( database_mode )
        {
            if ( info->completingFirstWord() )
                info->add("set_database_mode off");
            
            autoCompleteForDatabaseCommand( info );
            return;
        }
        
        if ( info->completingFirstWord() )
        {
            // Add console commands
            delilah_command_catalogue.autoComplete( info );
            
            return;
        }

        // Options for the cluter command
        if (info->completingSecondWord("cluster") )
        {
            info->add("info");
            info->add("add");
            info->add("remove");
            info->add("connect");
            info->add("connections");
        }
        
        if (info->completingSecondWord("help") )
        {
            // Add console commands
            delilah_command_catalogue.autoComplete( info );
            
            // Independent option
            info->add("all");
            info->add("-category"); // To help type 
        }
        
        
        
        if (info->completingThirdWord("help","-category") )
        {
            au::StringVector categories = delilah_command_catalogue.getCategories();
            for ( size_t i = 0 ; i < categories.size() ; i++ )
                info->add( categories[i] );
        }
        
        if (info->completingSecondWord("run_stream_operation") )
        {
            // Add operations
            autoCompleteOperations( info );
            return;
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
        
        
        if (info->completingSecondWord("trace") )
        {
            if (trace_on)
                info->add("off");
            else
                info->add("on");
        }
        
        if (info->completingSecondWord("set_database_mode") )
            info->add("on"); // It can only be on
        
        
        // Upload
        // ------------------------------------------------------------------------
        
        if ( info->completingSecondWord("upload") )
        {
            info->auto_complete_files("");
            return;
        }
        if (info->completingThirdWord( "upload" , "*" ) )
        {
            autoCompleteDataSets( info );
            return;
        }
        
        // Download operation
        // ------------------------------------------------------------------------
        
        if ( info->completingSecondWord("download") )
        {
            autoCompleteDataSets( info );
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
		
		au::CommandLine commandLine;
		commandLine.set_flag_string("name", "null");
		commandLine.set_flag_string("begin", "null");           
		commandLine.set_flag_boolean("plain");              // Flag to indicate plain ( not used ) 
		commandLine.set_flag_boolean("gz");                 // Flag to indicate compression ( not used )
		commandLine.set_flag_int("threads",4);              // Specify number of threads ( not used )
        commandLine.set_flag_boolean("force");              // Force to remove directory if exist before in pop operations
        commandLine.set_flag_boolean("clear");              // Used in the ps command
        commandLine.set_flag_int("limit", 1000);
		commandLine.set_flag_string("category", "");
		commandLine.parse( command );
        
		std::string mainCommand;
        
		if( commandLine.get_num_arguments() == 0)
			return 0;	// Zero means no pending operation to check
		else
			mainCommand = commandLine.get_argument(0);
        
        
        if( database_mode )
        {
            
            if( mainCommand == "set_database_mode" )
            {
                if( commandLine.get_num_arguments() < 2 )
                {
                    writeErrorOnConsole("Usage: set_database_mode on/off");
                    return 0;
                }
                
                if( commandLine.get_argument(1) == "on" )
                {
                    database_mode = true;  
                    writeWarningOnConsole("Database mode activated");
                }
                else if( commandLine.get_argument(1) == "off" )
                {
                    database_mode = false;  
                    writeWarningOnConsole("Database mode deactivated");
                }
                else
                    writeErrorOnConsole("Usage: set_database_mode on/off");
                
                return 0;
            }
            
            // Run data base command
            std::string result = runDatabaseCommand(command);
            writeOnConsole( au::strToConsole(result) );
            return 0;
        }
        
        if ( mainCommand == "cluster" )
        {
            // Print network status
            writeOnConsole( network->cluster_command( command ) );
            return 0;
        }
        
        if( mainCommand == "set_database_mode" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole("Usage: set_database_mode on/off");
                return 0;
            }
            
            if( commandLine.get_argument(1) == "on" )
            {
                database_mode = true;  
                writeWarningOnConsole("Database mode activated");
            }
            else if( commandLine.get_argument(1) == "off" )
            {
                database_mode = false;  
                writeWarningOnConsole("Database mode deactivated");
            }
            else
                writeErrorOnConsole("Usage: set_database_mode on/off");
            
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
                output << au::indent( general_description ) << "\n";
                output << "\n";
                output << au::indent( au::str("Auths: %s", auths ) ) << "\n";
                output << "\n";
                output << au::indent( au::str("Telefonica I+D 2010" ) ) << "\n";
                output << "\n";
                output << au::lineInConsole('-') << "\n";
                output << "\n";
                output << "\tType help all [-category category_name] to get help for all available commands\n";
                output << "\t\tCurrent categories: " << delilah_command_catalogue.getCategories().str() << "\n";
                output << "\n";
                output << "\tType help <command> to get more concrete information for a command\n";
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
                std::ostringstream output;
                output << "Environent variables:\n";
                output << "------------------------------------\n";
                output << environment.toString();
                output << "\n";
                output << "\n";
                writeOnConsole( output.str() );
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
        
        if ( mainCommand == "trace")
        {
            if ( commandLine.get_num_arguments() == 1)
            {
                if( trace_on )
                    writeOnConsole( "Traces are activated" );
                else
                    writeOnConsole( "Traces are NOT activated" );
                return 0;
            }
            
            if( commandLine.get_argument(1) == "on" )
            {
                trace_on = true;
                writeOnConsole( "Traces are now activated" );
                return 0;
            }
            if( commandLine.get_argument(1) == "off" )
            {
                trace_on = false;
                writeOnConsole( "Traces are now NOT activated" );
                return 0;
            }
            
            writeErrorOnConsole("Usage: trace on/off");
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
                size_t id = atoi( commandLine.get_argument(1).c_str() );
                
                DelilahComponent *component = components.findInMap( id );
                if( !component )
                    writeErrorOnConsole( au::str("Unknown process with id %d", id ) );
                else
                {
                    std::ostringstream output;
                    output << "------------------------------------------------\n";
                    output << " Process " << id << " " << component->cronometer.str() << "\n";
                    output << "------------------------------------------------\n";
                    output << "\n";
                    if( component->error.isActivated() )
                        output << "ERROR: " << component->error.getMessage() << "\n";
                    output << "\n";
                    output << component->getStatus();
                    output << "\n";
                    writeOnConsole(output.str());
                    
                }
                
                return 0;
            }
            
            std::ostringstream output;
            output << getListOfComponents();
            writeOnConsole(output.str());
            
            return 0;
            
        }
        
        if( mainCommand == "stop_repeat" )
        {
            // Stop all repeat commands
            engine::Engine::shared()->notify( new engine::Notification(notification_delilah_stop_repeat_tasks ) );
            
            showWarningMessage("All repeat operations stoped");
            
            return 0;
        }
        
        if( mainCommand == "repeat" )
        {
            size_t pos = command.find("repeat");
            if( pos != std::string::npos )
            {
                std::string repeat_command = command.substr( pos+6 , std::string::npos );
                
                RepeatDelilahComponent* component =  new RepeatDelilahComponent( repeat_command , 2 );
                size_t id = addComponent( component );
                component->run();
                return id;
                
            }
            else
                LM_W(("Strange behaviour with repeat command..."));
            
            return 0;
            
        }
        
        // Push data to a queue
        
        if( mainCommand == "push" )
        {
            if( commandLine.get_num_arguments() < 3 )
            {
                writeErrorOnConsole("Usage: push file <file2> .... <fileN> queue1,queue2,queue3 ");
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
                    if( trace_on )
                    {
                        
                        std::ostringstream message;
                        message << "Including regular file " << fileName;
                        writeOnConsole( message.str() );
                    }
                    
                    fileNames.push_back( fileName );
                }
                else if ( S_ISDIR(buf.st_mode) )
                {
                    if( trace_on )
                    {
                        std::ostringstream message;
                        message << "Including directory " << fileName;
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
                    if( trace_on )
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
                writeErrorOnConsole("Usage: pop queue fileName");
                return 0;
            }
            
            std::string queue_name  = commandLine.get_argument(1);
            std::string fileName    = commandLine.get_argument(2);
            
            bool force_flag = commandLine.get_flag_bool("force");
            
            size_t id = addPopData( queue_name ,  fileName , force_flag );
            
            return id;
        }
        
        
        // WorkerCommands
        std::string main_command = commandLine.get_argument(0);
        
        if( main_command == "reload_modules" )
        {
            return sendWorkerCommand( command , NULL );
        }
        
        if( main_command == "run_stream_operation" )
        {
            return sendWorkerCommand( command , NULL );
        }
        
        if( main_command == "init_stream" )
        {
            return sendWorkerCommand( command , NULL );
        }
        
        if( main_command == "add_stream_operation" )
        {
            return sendWorkerCommand( command , NULL );
        }
        
        if( main_command == "rm_stream_operation" )
        {
            return sendWorkerCommand( command , NULL );
        }
        
        if( main_command == "set_stream_operation_property" )
        {
            return sendWorkerCommand( command , NULL );
        }
        
        if( main_command == "remove_all_stream" )
        {
            return sendWorkerCommand( command , NULL );
        }
        
        if( main_command == "connect_to_queue" )
        {
            return sendWorkerCommand( command , NULL );
        }
        
        if( main_command == "disconnect_form_queue" )
        {
            return sendWorkerCommand( command , NULL );
        }
        
        if( main_command == "wait" )
        {
            return sendWorkerCommand( command , NULL );
        }

        if( main_command == "run_stream_update_state" )
        {
            return sendWorkerCommand( command , NULL );
        }
        
        // Command to remove queues 
        if( main_command == "rm_queue" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole( au::str("Usage: rm_queue queue" ) );
                return 0;
            }
            else
                return sendWorkerCommand( command , NULL );
        }
        
        // Command to remove queues 
        if( main_command == "set_queue_property" )
        {
            if( commandLine.get_num_arguments() < 4 )
            {
                writeErrorOnConsole( au::str("Usage: set_queue_propert queue property value" ) );
                return 0;
            }
            else
                return sendWorkerCommand( command , NULL );
        }
        
        // Command to remove queues 
        if( main_command == "cp_queue" )
        {
            if( commandLine.get_num_arguments() < 3 )
            {
                writeErrorOnConsole( au::str("Usage: cp_queue form_queue to_queue" ) );
                return 0;
            }
            else
                return sendWorkerCommand( command , NULL );
        }
        
        
        
        // Command to play / pause statess
        if( main_command == "pause_queue" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole( au::str("Usage: pause_queue queue" ) );
                return 0;
            }
            else
                return sendWorkerCommand( command , NULL );
            
        }
        
        if( main_command == "play_queue" )
        {
            if( commandLine.get_num_arguments() < 2 )
            {
                writeErrorOnConsole( au::str("Usage: play_queue queue" ) );
                return 0;
            }
            else
                return sendWorkerCommand( command , NULL );
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

        if( mainCommand == "automatic_update" )
        {
            automatic_update = true;
            writeOnConsole("OK\n");
            return 0;
        }
        
        if( mainCommand == "cancel_automatic_update" )
        {
            automatic_update = true;
            writeOnConsole("OK\n");
        }
        
        // New way to get information ( Worker Command )
        if(
           ( mainCommand == "ls_queues" )     ||
           ( mainCommand == "ls_operations" ) ||
           ( mainCommand == "ls_datas" )      ||
           ( mainCommand == "ls_modules" )    ||
           ( mainCommand == "ps_stream" )
           )
            return sendWorkerCommand( command , NULL );
       
        if( mainCommand == "ls_local" )
        {
            std::string pattern ="*";
            if( commandLine.get_num_arguments() > 1 )
                pattern = commandLine.get_argument(1);
            
            writeOnConsole( getLsLocal( pattern ) ); 
            return 0;
        }
        
        
        // Old information mechanism...
        /*
        if( ( main_command.substr(0,2) == "ls" ) || ( main_command.substr(0,2) == "ps" ) )
        {
            std::string text = info( command );
            writeOnConsole( text );
            return 0;
        }
         */
        
        writeErrorOnConsole( au::str("Unknown command '%s'\n" , main_command.c_str() ) );
        
        return 0;
    }
    
    int DelilahConsole::_receive( Packet* packet )
    {
        std::ostringstream  txt;
        
        switch ( packet->msgCode ) 
        {
                
            case Message::Trace:
            {
                std::string _text   = packet->message->trace().text();
                std::string file    = packet->message->trace().file();
                std::string fname   = packet->message->trace().fname();
                std::string stre    = packet->message->trace().stre();
                
                //writeOnConsole( au::str("Trace: %s", _text.c_str() ) );
                if( trace_on )
                {
                    
                    lmFdRegister(1, DEF1, "DEF", "stdout", NULL);
                    
                    
                    //if (lmOk(packet->message->trace().type(), packet->message->trace().tlev() ) == LmsOk)
                    {
                        
                        lmOut(
                              (char*)_text.c_str(),   
                              (char) packet->message->trace().type() , 
                              file.c_str(), 
                              packet->message->trace().lineno() , 
                              fname.c_str(), 
                              packet->message->trace().tlev() , 
                              stre.c_str() 
                              );
                    }
                    
                    //std::cerr << "TRACE: " << file << " ( " << fname << " ): " << _text << "\n";  
                    
                    lmFdUnregister(1);
                    
                    //return 0;    
                }
                
            }
                break;
                
            default:
                txt << "Unknwn packet received\n";
                
                LM_X(1, ("Unknown packet received at delilahConsole"));
                break;
        }
        
        
        return 0;
    }	
    
    void DelilahConsole::delilahComponentStartNotification( DelilahComponent *component )
    {
        if ( component->hidden )
            return; // No notification for hidden processes
        
        std::ostringstream o;
        o << "Local process started: " << component->getIdAndConcept() << "\n";
        
        if( component->error.isActivated() )
            writeErrorOnConsole(o.str());        
        else
            writeWarningOnConsole(o.str());        
    }
    
    void DelilahConsole::delilahComponentFinishNotification( DelilahComponent *component )
    {
        if ( component->hidden )
            return; // No notification for hidden processes
        
        if( !component->error.isActivated() )
            writeWarningOnConsole( au::str( "Local process finished: %s\n" , component->getIdAndConcept().c_str() ) );
        else
        {
            writeErrorOnConsole( au::str( "Local process finished with error: %s\nERROR: %s\n" 
                                           , component->getIdAndConcept().c_str()
                                           , component->error.getMessage().c_str()
                                           ) 
                                  );
        }
        
    }
    
    
}
