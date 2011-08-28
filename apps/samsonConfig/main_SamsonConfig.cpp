
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "au/CommandLine.h"                     // au::CommandLine
#include "au/Console.h"                         // au::Console

#include "samson/common/SamsonSetup.h"			// samson::SamsonSetup
#include "samson/common/coding.h"				// samson::FormatHeader

#include "samson/module/KVFormat.h"             // samson::KVFormat
#include "samson/module/ModulesManager.h"		// samson::ModulesManager

int logFd = -1;

//char* progName = (char*) "samsonCat";

bool modified = false;

std::string getHelpMessage()
{    
    std::ostringstream output;
    
    output << "-------------------------------------\n";
    output << "Help for samsonConfig tool\n";
    output << "-------------------------------------\n";
    output << "\n";
    output << " show                     Show all setup outputs\n";
    output << " set property value       Set a particular value\n";
    output << " save                     Save modified values\n";
    output << " check                    Check if current values are coherent\n";
    output << " edit                     Edit all property values\n";
    output << " quit                     Quit samsonConfig tool\n";

    return output.str();
}


class SamsonConfigConsole : public au::Console
{
    
public:
    
    std::string getPrompt()
    {
        return "SamsonConfig > ";
    }
    
    //!< function to process a command instroduced by user	
    void evalCommand( std::string command )
    {
        
        au::CommandLine cmd;
        cmd.parse( command );
        
        if( cmd.get_num_arguments() == 0)
            return;
        
        std::string main_command = cmd.get_argument(0);
        
        if( main_command == "help" )
        {
            writeOnConsole( getHelpMessage() );
            return;
        }
        
        if( main_command == "set" )
        {
            if( cmd.get_num_arguments() < 3 )
                writeOnConsole("Usage: set property value");
            
            std::string property = cmd.get_argument(1);
            std::string value = cmd.get_argument(2);

            if( !samson::SamsonSetup::shared()->isParameterDefined( property ) )
            {
                writeErrorOnConsole(au::str("Paramter '%s' not defined" , property.c_str()) );
                return;
            }
            
            samson::SamsonSetup::shared()->setValueForParameter( property , value );
            modified = true;
            writeWarningOnConsole(au::str("Property '%s' set to '%s'", property.c_str() , value.c_str() ));
            
            return;

        }

        
        if( main_command == "save" )
        {
            samson::SamsonSetup::shared()->save();  // Save a new file with the current setup
            std::string fileName = samson::SamsonSetup::shared()->setupFile;
            writeWarningOnConsole(  au::str("Saved file at %s", fileName.c_str() ) );
            
            modified = false;
            
            return;
        }
        

        if( main_command == "check" )
        {
            writeWarningOnConsole( "Checking setup..." );
            
            // Check everything looks ok
            au::ErrorManager errorManager;
            samson::SamsonSetup::shared()->check(&errorManager);
            
            if( errorManager.isActivated() )
                writeErrorOnConsole(errorManager.getMessage());
            else
                writeWarningOnConsole("OK\n");
            
            return;
        }
        
        
        if( main_command == "quit" )
        {
            if( modified )
            {
                std::cout << "Setup file modified.... save (y/n)? ";
                char line[1024];
                if (fgets(line, 1024, stdin)== NULL)
                {
                    writeWarningOnConsole("Read nothing");
                }
                if( strcmp(line, "y") || strcmp(line, "Y") )
                {
                    samson::SamsonSetup::shared()->save();  // Save a new file with the current setup
                    std::string fileName = samson::SamsonSetup::shared()->setupFile;
                    writeWarningOnConsole(  au::str("Saved file at %s", fileName.c_str() ) );
                }
                
            }
            
            quitConsole();
            return;
        }
        
        if ( main_command == "show" )
        {
            std::cout << samson::SamsonSetup::shared()->str();
            return;
        }
        
        if ( main_command == "edit" )
        {
            modified = true;
            samson::SamsonSetup::shared()->edit();  // Enter in edit mode from command line
            return;
        }
        
        writeErrorOnConsole(au::str("Unknown command '%s'" , main_command.c_str() ));
        writeWarningOnConsole("Type help to get a list of valid commands ");
        
    }
    
};


int main(int argc, const char *argv[])
{
	au::CommandLine cmdLine;
	cmdLine.set_flag_string("working",SAMSON_DEFAULT_WORKING_DIRECTORY);
    cmdLine.set_flag_boolean("check");
    cmdLine.set_flag_boolean("help");
    cmdLine.set_flag_boolean("-help");
	cmdLine.parse(argc , argv);

	samson::SamsonSetup::init(  );
    samson::SamsonSetup::shared()->setWorkingDirectory( cmdLine.get_flag_string("working") );

    
    if( cmdLine.get_flag_bool("check") )
    {
        std::cout << "Checking setup file ( " << samson::SamsonSetup::shared()->setupFile << " ) ...";
        
        // Check everything looks ok
        au::ErrorManager errorManager;
        samson::SamsonSetup::shared()->check(&errorManager);
        
        if( errorManager.isActivated() ) 
            std::cout << "Error checking setup: " << errorManager.getMessage();
        else
            std::cout << "OK!\n";
        
        return 0;
    }
    
    if( cmdLine.get_flag_bool("help") || cmdLine.get_flag_bool("-help") )
    {
        std::cout << "\n";
        std::cout << "samsonConfig [-check] : Samson configuration tool.\n";
        std::cout << "\n";
        std::cout << "\t -check : Verify current setup\n";
        std::cout << "\n";
        std::cout << "\t samsonConfig is an iterative tool to specify all the setup values in a Samson deployment\n";
        std::cout << "\t Just type help inside samsonConfig to get a list of valid commands\n";
        std::cout << "\n";
        return 0;
    }
    
    std::cout << "\nType help to get a list of valid command\n\n";
    SamsonConfigConsole console;
    console.runConsole();
    
    // Show samson setup
    //std::cout << samson::SamsonSetup::shared()->str();
	
}
