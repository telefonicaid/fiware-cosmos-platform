#ifndef _AU_CONSOLE_COMMAND_HISTORY
#define _AU_CONSOLE_COMMAND_HISTORY

#include <vector>
#include <string>
#include <sstream>
#include "au/namespace.h"

NAMESPACE_BEGIN(au)

class ConsoleCommand;

class ConsoleCommandHistory
{
    std::vector<ConsoleCommand*> commands;
    size_t pos;
    
    std::string file_name;
    
public:
    
    ConsoleCommandHistory();
    ~ConsoleCommandHistory();
    
    ConsoleCommand* current();
    
    
    void recover_history( );
    void save_history();
    
    void move_up();
    void move_down();
    void new_command();
  
    
    std::string history_string()
    {
        std::ostringstream output;

        size_t pos = 0;
        if ( commands.size() > 10 )
            pos = commands.size() - 10;

        for ( ; pos < commands.size() ; pos++)
            output << commands[pos]->getCommand() << "\n";
        
        return output.str();
    }
    
};

NAMESPACE_END

#endif