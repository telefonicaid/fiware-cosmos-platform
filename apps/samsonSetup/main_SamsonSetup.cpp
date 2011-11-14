
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "parseArgs/parseArgs.h"
#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "au/CommandLine.h"                     // au::CommandLine
#include "au/Console.h"                         // au::Console

#include "samson/common/SamsonSetup.h"			// samson::SamsonSetup
#include "samson/common/coding.h"				// samson::FormatHeader
#include "samson/common/samsonVars.h"           // SAMSON_ARG_VARS SAMSON_ARGS
#include "samson/module/KVFormat.h"             // samson::KVFormat
#include "samson/module/ModulesManager.h"		// samson::ModulesManager
#include "samson/common/samsonVersion.h"




/* ****************************************************************************
 *
 * Option variables
 */

SAMSON_ARG_VARS;
bool check;

/* ****************************************************************************
 *
 * parse arguments
 */

PaArgument paArgs[] =
{
	SAMSON_ARGS,
    
	{ "-check",   &check,    "",     PaBool,    PaOpt,  false,  false,   true,  "Check if setup file is correct" },
    
	PA_END_OF_ARGS
};

int logFd = -1;
bool modified = false;  // Global variable to know if we have to save something before quiting..

std::string getHelpMessage()
{    
    std::ostringstream output;
    
    output << "-------------------------------------\n";
    output << "Help for samsonSetup tool\n";
    output << "-------------------------------------\n";
    output << "\n";
    output << " show                     Show all setup outputs\n";
    output << " set property value       Set a particular value\n";
    output << " save                     Save modified values\n";
    output << " check                    Check if current values are coherent\n";
    output << " edit                     Edit all property values\n";
    output << " set_desktop              Set typical valies for desktop 2Gb & 2 cores\n";
    output << " quit                     Quit samsonSetup tool\n";

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

        if( main_command == "set_desktop" )
        {
            samson::SamsonSetup::shared()->resetToDefaultValues();
            samson::SamsonSetup::shared()->setValueForParameter( "general.memory" , "2000000000" );
            samson::SamsonSetup::shared()->setValueForParameter( "general.num_processess" , "2" );
            samson::SamsonSetup::shared()->setValueForParameter( "general.shared_memory_size_per_buffer" , "64000000" );

            writeWarningOnConsole("Properties general.memory                        = 2Gb");
            writeWarningOnConsole("Properties general.num_processess                = 2");
            writeWarningOnConsole("Properties general.shared_memory_size_per_buffer = 64Mb");
            return;
        }
        
        
        if( main_command == "save" )
        {
            int res = samson::SamsonSetup::shared()->save();  // Save a new file with the current setup

            std::string fileName = samson::SamsonSetup::shared()->setupFileName();

            if( !res )
            {
                writeWarningOnConsole(  au::str("Saved file at %s", fileName.c_str() ) );
                modified = false;
            }
            else
            {
                writeErrorOnConsole(  au::str("Error saving file at %s", fileName.c_str() ) );
            }
            
            
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
                    std::string fileName = samson::SamsonSetup::shared()->setupFileName();
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





/* ****************************************************************************
 *
 * man texts -
 */
static const char* manSynopsis         = " [OPTION]";
static const char* manShortDescription = "samsonSetup is a tool to define setup parameters in a SAMSON system.\n\n";
static const char* manDescription      =
"samsonSetup [-check] : Samson configuration tool.\n"
"\n"
"\t -check : Verify current setup\n"
"\n"
"\t samsonSetup is an interactive tool to specify all the setup values in a Samson deployment\n"
"\t Just type help inside samsonSetup to get a list of valid commands\n"
"\n";

static const char* manExitStatus    = "0      if OK\n 1-255  error\n";
static const char* manAuthor        = "Written by Andreu Urruela, Ken Zangelin and J.Gregorio Escalada.";
static const char* manReportingBugs = "bugs to samson-dev@tid.es\n";
static const char* manCopyright     = "Copyright (C) 2011 Telefonica Investigacion y Desarrollo";
static const char* manVersion       = SAMSON_VERSION;



/* ****************************************************************************
 *
 * main - 
 */
int main(int argC, const char *argV[])
{
	paConfig("builtin prefix",                (void*) "SS_SETUP_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE@TIME  EXEC: TEXT");
	paConfig("log to file",                   (void*) true);
    
	paConfig("man synopsis",                  (void*) manSynopsis);
	paConfig("man shortdescription",          (void*) manShortDescription);
	paConfig("man description",               (void*) manDescription);
	paConfig("man exitstatus",                (void*) manExitStatus);
	paConfig("man author",                    (void*) manAuthor);
	paConfig("man reportingbugs",             (void*) manReportingBugs);
	paConfig("man copyright",                 (void*) manCopyright);
	paConfig("man version",                   (void*) manVersion);
    
	paParse(paArgs, argC, (char**) argV, 1, false);
    

    // SamsonSetup init
	samson::SamsonSetup::init( samsonHome , samsonWorking  );
    samson::SamsonSetup::shared()->createWorkingDirectories();      // Create working directories
    
    if( check )
    {
        std::cout << "Checking setup file ( " << samson::SamsonSetup::shared()->setupFileName() << " ) ...";
        
        // Check everything looks ok
        au::ErrorManager errorManager;
        samson::SamsonSetup::shared()->check(&errorManager);
        
        if( errorManager.isActivated() ) 
            std::cout << "Error checking setup: " << errorManager.getMessage();
        else
            std::cout << "OK!\n";
        
        return 0;
    }
    
    std::cout << "\nType help to get a list of valid command\n\n";
    SamsonConfigConsole console;
    console.runConsole();
    
    // Show samson setup
    //std::cout << samson::SamsonSetup::shared()->str();
	
}
