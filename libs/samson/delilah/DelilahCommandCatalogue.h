#ifndef _H_DELILAH_COMMAND_CATALOGE
#define _H_DELILAH_COMMAND_CATALOGE

#include <cstdlib>				// atexit

#include <sstream>                  // std::ostringstream
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <algorithm>

#include "logMsg/logMsg.h"				

#include "au/mutex/TokenTaker.h"                  // au::TokenTake
#include "au/console/Console.h"                     // au::Console
#include "au/console/ConsoleAutoComplete.h"

#include "au/tables/Select.h"
#include "au/tables/Table.h"


#include "samson/delilah/Delilah.h"			// samson::Delilah

namespace samson {
	
    // Class used to store a valid command in delilah console
    
    class DelilahCommand
    {
        
    public:
        
        std::string name;
        std::string category;
        
        std::string short_description;
        
        std::string description;
        std::string usage;
        
        DelilahCommand(
                       std::string _name , 
                       std::string _category="general" , 
                       std::string _short_description = "",
                       std::string _usage = "" 
                       )
        {
            name = _name;
            category = _category;
            short_description = _short_description;
            usage = _usage;
        }
        
    };
    
    // Collection of valid commands used in delilah
    
    class DelilahCommandCatalogue
    {
        
    public:
        
        // Vector of available commands
        std::vector<DelilahCommand> commands;

        // Construtor
        DelilahCommandCatalogue();

        // Add elements adn extra description
        void add( std::string name , std::string category , std::string short_description = "" , std::string usage = "" );
        void add_description( std::string name , std::string description );

        // Autocompletion functionality ( see au::Console )
        void autoComplete( au::ConsoleAutoComplete* info );
        
        // Get a table with available command ( optional filter per category )
        std::string getCommandsTable( std::string category="" );

        // Get help message for an individual command
        std::string getHelpForCommand( std::string name );
        
        // Get list of cagtegories to be displayed in help message
        au::StringVector getCategories();

        // Check if a command is valid ( used in repeat command )
        bool isValidCommand( std::string command );
        
        // Check if is a category
        bool isValidCategory( std::string category );
        
    };    
    
    
}

#endif